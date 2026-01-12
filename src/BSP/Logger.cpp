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
/** @brief See Logger.h */
bool Logger::_SISINIT = false;

/** @brief See Logger.h */
E_LogLevel Logger::_SLOGLEVEL = E_LogLevel::LOG_LEVEL_CRITICAL;

/** @brief See Logger.h */
char* Logger::_SPBUFFER = nullptr;

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

void Logger::Init(const E_LogLevel kLoglevel) noexcept
{
    if (!_SISINIT) {
        Serial.begin(LOGGER_SERIAL_BAUDRATE);

        _SISINIT = true;
        _SLOGLEVEL = kLoglevel;
        _SPBUFFER = new char[LOGGER_BUFFER_SIZE];
        if (nullptr == _SPBUFFER) {
            Serial.printf("Failed to allocate logger buffer.\n");
            HWManager::Reboot();
        }
    }
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

    if (_SISINIT && _SLOGLEVEL >= kLevel) {
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
            memcpy(pTag, "[DBG   -  %16llu] %s:%d -\0", 26);
        }
        else {
            memcpy(pTag, "[UNKN  - %16llu] %s:%d -\0", 26);
        }

        /* Setup message */
        len = snprintf(
            _SPBUFFER,
            LOGGER_BUFFER_SIZE,
            pTag,
            HWManager::GetTime(),
            pkFile,
            kLine
        );
        _SPBUFFER[len++] = ' ';

        /* Add message formating */
        va_start(argptr, pkStr);
        len += vsnprintf(
            _SPBUFFER + len,
            LOGGER_BUFFER_SIZE - len,
            pkStr,
            argptr
        );
        va_end(argptr);

        /* Terminate and print */
        _SPBUFFER[len] = 0;
        Serial.printf("%s", _SPBUFFER);

        /* On critical, reboot */
        if (LOG_LEVEL_CRITICAL == kLevel) {
            /* Get the system state */
            pSysState = SystemState::GetInstance();
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

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */