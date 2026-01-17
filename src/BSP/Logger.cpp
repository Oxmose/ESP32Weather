/*******************************************************************************
 * @file Logger.cpp
 *
 * @see Logger.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the logging module.
 *
 * @details This file defines the logging module. This comprises a set of
 * functions used to log at different verbose levels.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <BSP.h>         /* Hardware services*/
#include <cstdint>       /* Standard Int Types */
#include <Arduino.h>     /* Serial service */
#include <Storage.h>     /* Storage manager */
#include <SystemState.h> /* System state services */
#include <ModeManager.h> /* Mode management */

/* Header file */
#include <Logger.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/** @brief Serial Baudrate */
#define LOGGER_SERIAL_BAUDRATE 115200

/** @brief The log buffer size in bytes. */
#define LOGGER_BUFFER_SIZE 512

/** @brief Log file path. */
#define LOG_JOURNAL_PATH "rthr_logs"

/** @brief Ram log buffer size. */
#define LOG_RAM_BUFFER_SIZE 0x200000

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/

/* None */

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
/** @brief Singleton instance. */
Logger* Logger::_SPINSTANCE = nullptr;

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
Logger* Logger::GetInstance(void) noexcept {
    if (nullptr == Logger::_SPINSTANCE) {
        Logger::_SPINSTANCE = new Logger();

        HWManager::DelayExecNs(50000000);

        if (nullptr == Logger::_SPINSTANCE) {
            Serial.begin(LOGGER_SERIAL_BAUDRATE);
            HWManager::DelayExecNs(50000000);
            Serial.printf("Failed to initialize logger.\n");
            ESP.restart();
        }
    }

    return Logger::_SPINSTANCE;
}

void Logger::LogLevel(const E_LogLevel kLevel,
                      const char*      pkFile,
                      const uint32_t   kLine,
                      const char*      pkStr,
                      ...) noexcept
{
    va_list      argptr;
    size_t       len;
    char         pTag[32];
    SystemState* pSysState;
    ModeManager* pModeMgr;

    if (LOG_LEVEL >= kLevel) {
        /* Print TAG */
        if (LOG_LEVEL_CRITICAL == kLevel) {
            memcpy(pTag, "[CRIT  - %16llu] %s:%d -\0", 26);
        }
        else if (LOG_LEVEL_ERROR == kLevel) {
            memcpy(pTag, "[ERROR - %16llu] %s:%d -\0", 26);
        }
        else if (LOG_LEVEL_INFO == kLevel) {
            memcpy(pTag, "[INFO  - %16llu]\0", 17);
        }
        else if (LOG_LEVEL_DEBUG == kLevel) {
            memcpy(pTag, "[DBG   - %16llu] %s:%d -\0", 26);
        }
        else {
            memcpy(pTag, "[UNKN  - %16llu] %s:%d -\0", 26);
        }

        /* Setup message */
        len = snprintf(
            this->_logBuffer,
            LOGGER_BUFFER_SIZE,
            pTag,
            HWManager::GetTime(),
            pkFile,
            kLine
        );
        this->_logBuffer[len++] = ' ';

        /* Add message formating */
        va_start(argptr, pkStr);
        len += vsnprintf(
            this->_logBuffer + len,
            LOGGER_BUFFER_SIZE - len,
            pkStr,
            argptr
        );
        va_end(argptr);

        /* Terminate and print */
        this->_logBuffer[len] = 0;
        Serial.printf("%s", this->_logBuffer);

        /* Log to journal */
        WriteRamJournal(this->_logBuffer, len);
        WritePersistentJournal(this->_logBuffer, len);

        /* On critical, reboot */
        if (LOG_LEVEL_CRITICAL == kLevel) {
            pSysState = SystemState::GetInstance();
            /* Get the system state */
            if (nullptr != pSysState) {
                /* Set the maintenance mode */
                pModeMgr = pSysState->GetModeManager();
                if (nullptr != pModeMgr) {
                    pModeMgr->SetMode(E_Mode::MODE_MAINTENANCE);
                }
            }
            HWManager::Reboot();
        }
    }
}

void Logger::Flush(void) noexcept {
    Serial.flush();
}

FsFile Logger::OpenPersistenJournal(void) const noexcept {
    SystemState* pSysState;
    Storage*     pStorage;
    FsFile       file;

    pSysState = SystemState::GetInstance();
    if (nullptr != pSysState) {
        pStorage = pSysState->GetStorage();
        if (nullptr != pStorage) {
            /* Open the log file */
            file = pStorage->Open(LOG_JOURNAL_PATH, O_RDONLY);
        }
    }

    return file;
}

void Logger::ClearPersistentJournal(void) const noexcept {
    SystemState* pSysState;
    Storage*     pStorage;

    pSysState = SystemState::GetInstance();
    if (nullptr != pSysState) {
        pStorage = pSysState->GetStorage();
        if (nullptr != pStorage) {
            /* remove the log file */
            pStorage->Remove(LOG_JOURNAL_PATH);
        }
    }
}

void Logger::OpenRamJournal(S_RamJournalDescriptor* pDesc) const noexcept {
    pDesc->pCursor = 0;
}

size_t Logger::ReadRamJournal(uint8_t*                pBuffer,
                              size_t                  length,
                              S_RamJournalDescriptor* pDesc) const noexcept {
    size_t   available;
    size_t   toCopy;
    size_t   leftToCopy;
    uint8_t* pCopyCursor;
    size_t   cursorOff;

    /* Get the max content */
    if (this->_logJournalRam.hasCircled) {
        available = (uint32_t)this->_logJournalRam.pEndAddress -
                    (uint32_t)this->_logJournalRam.pStartAddress;
    }
    else {
        available = (uint32_t)this->_logJournalRam.pCursor -
                    (uint32_t)this->_logJournalRam.pStartAddress;
    }

    /* Get the amount to copy */
    if (pDesc->pCursor < available) {
        /* Get the size to copy */
        toCopy = available - pDesc->pCursor;
        toCopy = toCopy < length ? toCopy : length;

        /* Get the offset cursor position */
        cursorOff = (uint32_t)this->_logJournalRam.pCursor -
                    (uint32_t)this->_logJournalRam.pStartAddress;

        if (pDesc->pCursor > cursorOff) {
            pCopyCursor = this->_logJournalRam.pEndAddress -
                          (pDesc->pCursor - cursorOff);
        }
        else {
            pCopyCursor = this->_logJournalRam.pCursor - pDesc->pCursor;
        }

        /* Set the start of the copy */
        cursorOff = (uint32_t)pCopyCursor -
                    (uint32_t)this->_logJournalRam.pStartAddress;
        if (toCopy > cursorOff) {
            pCopyCursor = this->_logJournalRam.pEndAddress -
                          (toCopy - cursorOff);
        }
        else {
            pCopyCursor -= toCopy;
        }

        /* Check if we have a copy rollover */
        if (pCopyCursor + toCopy > this->_logJournalRam.pEndAddress) {
            /* Copy first part */
            leftToCopy = this->_logJournalRam.pEndAddress - pCopyCursor;
            memcpy(pBuffer, pCopyCursor, leftToCopy);

            /* Update left to copy info */
            pBuffer += leftToCopy;
            leftToCopy = toCopy - leftToCopy;
            pCopyCursor = this->_logJournalRam.pStartAddress;
        }
        else {
            leftToCopy = toCopy;
        }

        memcpy(pBuffer, pCopyCursor, leftToCopy);
    }
    else {
        toCopy = 0;
    }

    pDesc->pCursor += toCopy;

    return toCopy;
}
void Logger::SeekRamJournal(S_RamJournalDescriptor* pDesc,
                            const size_t            kOffset) const noexcept{
    size_t available;

    /* Get the max content */
    if (this->_logJournalRam.hasCircled) {
        available = (uint32_t)this->_logJournalRam.pEndAddress -
                    (uint32_t)this->_logJournalRam.pStartAddress;
    }
    else {
        available = (uint32_t)this->_logJournalRam.pCursor -
                    (uint32_t)this->_logJournalRam.pStartAddress;
    }

    if (kOffset < available) {
        pDesc->pCursor = kOffset;
    }
    else {
        pDesc->pCursor = available;
    }
}

void Logger::ClearRamJournal(void) noexcept {
    /* Resets the RAM logs */
    this->_logJournalRam.hasCircled = false;
    this->_logJournalRam.pCursor = this->_logJournalRam.pStartAddress;
}

void Logger::WritePersistentJournal(const char* kpStr, size_t len)
noexcept {
    SystemState* pSysState;
    Storage*     pStorage;

    if (!this->_logfile.isOpen()) {
        pSysState = SystemState::GetInstance();
        if (nullptr != pSysState) {
            pStorage = pSysState->GetStorage();
            if (nullptr != pStorage) {
                /* Open the log file and append */
                this->_logfile = pStorage->Open(
                    LOG_JOURNAL_PATH,
                    O_RDWR | O_CREAT | O_APPEND
                );
            }
        }
    }

    if (this->_logfile.isOpen()) {
        this->_logfile.write(kpStr, len);
    }
}

void Logger::WriteRamJournal(const char* kpStr, size_t len) noexcept {

    size_t toWrite;

    while (0 < len) {
        /* Check bounds */
        if (this->_logJournalRam.pCursor + len >
            this->_logJournalRam.pEndAddress) {
            toWrite = this->_logJournalRam.pEndAddress -
                      this->_logJournalRam.pCursor;
        }
        else {
            toWrite = len;
        }

        /* Copy data */
        memcpy(this->_logJournalRam.pCursor, kpStr, toWrite);

        /* Update cursors */
        kpStr += toWrite;
        len -= toWrite;
        this->_logJournalRam.pCursor += toWrite;

        /* Check for rollover */
        if (this->_logJournalRam.pCursor == this->_logJournalRam.pEndAddress) {
            this->_logJournalRam.pCursor = this->_logJournalRam.pStartAddress;
            this->_logJournalRam.hasCircled = true;
        }
    }
}

Logger::Logger() noexcept
{
    /* Init serial */
    Serial.begin(LOGGER_SERIAL_BAUDRATE);

    /* Init buffer */
    this->_logBuffer = new char[LOGGER_BUFFER_SIZE];
    if (nullptr == this->_logBuffer) {
        Serial.printf("Failed to allocate logger buffer.\n");
        HWManager::Reboot();
    }

    /* Init RAM journal */
    this->_logJournalRam.pStartAddress = (uint8_t*)ps_calloc(
        LOG_RAM_BUFFER_SIZE,
        sizeof(uint8_t)
    );
    if (nullptr == this->_logJournalRam.pStartAddress) {
        Serial.printf("Failed to allocate logger journal buffer.\n");
        HWManager::Reboot();
    }
    this->_logJournalRam.pEndAddress = this->_logJournalRam.pStartAddress +
                                       LOG_RAM_BUFFER_SIZE - 1;
    this->_logJournalRam.pCursor     = this->_logJournalRam.pStartAddress;
    this->_logJournalRam.hasCircled  = false;
}
