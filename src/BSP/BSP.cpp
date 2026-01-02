/*******************************************************************************
 * @file BSP.cpp
 *
 * @see BSP.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the hardware layer.
 *
 * @details This file defines the hardware layer. This layer provides services
 * to interact with the ESP32 module hardware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>          /* Standard string */
#include <cstdint>         /* Standard int types */
#include <Logger.h>        /* Logger services */
#include <Arduino.h>       /* Arduino library */
#include <HealthMonitor.h> /* HM Services*/

/* Header file */
#include <BSP.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
/** @brief See BSP.h */
std::string HWManager::_SHWUID;
/** @brief See BSP.h */
std::string HWManager::_SMACADDR;
/** @brief See BSP.h */
float HWManager::_SCPUCYCLE = 0.0;
/** @brief See BSP.h */
uint64_t HWManager::_STIME_LOW = 0;
/** @brief See BSP.h */
uint64_t HWManager::_STIME_HIGH = 0;
/** @brief See BSP.h */
bool HWManager::_TIMERROLL = false;
/** @brief Decimal to Hexadecimal convertion table */
static const char spkHexTable[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

/** @brief Time spinlock. */
static portMUX_TYPE sTimeSpinlock = portMUX_INITIALIZER_UNLOCKED;


/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
const char* HWManager::GetHWUID(void) noexcept {
    uint8_t value[6];
    uint8_t i;

    /* Check if the HWUID was already generated */
    if(0 == _SHWUID.size()) {
        _SHWUID = "RTHRWS-";

        if (ESP_OK != esp_read_mac(value, ESP_MAC_WIFI_SOFTAP)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HW_MAC_NOT_AVAIL, nullptr);
        }

        for (i = 0; 6 > i; ++i) {
            _SHWUID += std::string(1, spkHexTable[(value[i] >> 4) & 0xF]) +
                       std::string(1, spkHexTable[value[i] & 0xF]);
        }
    }

    /* Copy HWUID */
    return _SHWUID.c_str();
}

const char* HWManager::GetMacAddress(void) noexcept {
    uint8_t i;
    uint8_t value[6];

    /* Check if the MAC address was already generated */
    if(0 == _SMACADDR.size()) {
        if (ESP_OK != esp_read_mac(value, ESP_MAC_WIFI_SOFTAP)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HW_MAC_NOT_AVAIL, nullptr);
        }

        _SMACADDR = "";
        for (i = 0; 6 > i; ++i) {
            _SMACADDR += std::string(1, spkHexTable[(value[i] >> 4) & 0xF]) +
                         std::string(1, spkHexTable[value[i] & 0xF]);
            if(5 > i) {
                _SMACADDR += ":";
            }
        }
    }

    /* Copy MAC address */
    return _SMACADDR.c_str();
}

uint64_t HWManager::GetTime(void) noexcept {
    uint32_t newTime;
    uint64_t resultTime;

    taskENTER_CRITICAL(&sTimeSpinlock);

    /* Initialize CPU cycle time if needed */
    if (0.0 == _SCPUCYCLE) {
        _SCPUCYCLE = (double)1000.0 / (double)getCpuFrequencyMhz();
    }

    /* Get cpu time */
    newTime = portGET_RUN_TIME_COUNTER_VALUE();

    /* Manage rollover */
    if (newTime < _STIME_LOW && _STIME_LOW > 4000000000ULL) {
        if (_TIMERROLL) {
            ++_STIME_HIGH;
        }
        else {
            _TIMERROLL = true;
        }
    }
    _STIME_LOW = newTime;


    resultTime =(_STIME_HIGH << 32) | _STIME_LOW;

    taskEXIT_CRITICAL(&sTimeSpinlock);

    return (uint64_t)((double)resultTime * _SCPUCYCLE);
}

void HWManager::DelayExecNs(const uint64_t kDelayNs) noexcept {
    uint64_t passiveTime;
    uint64_t startTime;
    uint64_t delaysUs;

    startTime = (uint64_t)esp_timer_get_time();

    delaysUs = kDelayNs / 1000ULL;

    /* Calculate how much we can spend in passive wait */
    passiveTime = (delaysUs / 1000ULL) / portTICK_PERIOD_MS;

    /* Check if we can use a precise wait */
    if (0 != passiveTime) {
        vTaskDelay(passiveTime);
    }

    /* Spend the rest for active time */
    while ((uint64_t)esp_timer_get_time() < startTime + delaysUs) {
    }
}

void HWManager::Reboot(void) noexcept {
    /* Wait a bit for all buffer to be flushed */
    DelayExecNs(500000000ULL);
    ESP.restart();
    while (true) {}
}