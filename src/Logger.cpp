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
#include <cstdint>   /* Standard Int Types */
#include <Arduino.h> /* Serial service */

/* Header file */
#include <Logger.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/** @brief Serial Baudrate */
#define LOGGER_SERIAL_BAUDRATE 115200

/** @brief The log buffer size in bytes. */
#define LOGGER_BUFFER_SIZE 256

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
E_LogLevel Logger::_SLOGLEVEL = E_LogLevel::LOG_LEVEL_NONE;

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

void Logger::Init(const E_LogLevel kLoglevel)
{
    if (!_SISINIT) {
        Serial.begin(LOGGER_SERIAL_BAUDRATE);

        _SISINIT   = true;
        _SLOGLEVEL = kLoglevel;
    }
}

void Logger::LogLevel(const E_LogLevel kLevel,
                      const char*      pkFile,
                      const uint32_t   kLine,
                      const char*      pkStr,
                      ...)
{
    va_list argptr;
    size_t  len;
    char    pTag[32];
    char*   pBuffer;

    if (_SISINIT && _SLOGLEVEL >= kLevel) {

        /* Allocate buffer */
        pBuffer = new char[LOGGER_BUFFER_SIZE];
        if (pBuffer == nullptr) {
            return;
        }

        /* Print TAG */
        if (kLevel == LOG_LEVEL_ERROR) {
            memcpy(pTag, "[ERROR - %16llu] %s:%d -\0", 26);
        }
        else if (kLevel == LOG_LEVEL_INFO) {
            memcpy(pTag, "[INFO - %16llu]\0", 17);
        }
        else if (kLevel == LOG_LEVEL_DEBUG) {
            memcpy(pTag, "[DBG -  %16llu] %s:%d -\0", 24);
        }
        else {
            memcpy(pTag, "[UNKN - %16llu] %s:%d -\0", 25);
        }

        /* Setup message */
        len = snprintf(
            pBuffer,
            LOGGER_BUFFER_SIZE,
            pTag,
            esp_timer_get_time() * 1000, // TODO: Once the HAL is ready, replace by HAL call
            pkFile,
            kLine
        );
        pBuffer[len++] = ' ';

        /* Add message formating */
        va_start(argptr, pkStr);
        len += vsnprintf(
            pBuffer + len,
            LOGGER_BUFFER_SIZE - len,
            pkStr,
            argptr
        );
        va_end(argptr);

        /* Terminate and print */
        pBuffer[len] = 0;
        Serial.printf("%s", pBuffer);

        /* Release memory */
        delete[] pBuffer;
    }
}