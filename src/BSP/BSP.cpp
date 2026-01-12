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
#include <SPI.h>           /* SPI bus */
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
SPIClass HWManager::_SSPIBUS(HSPI);
/** @brief See BSP.h */
bool HWManager::_SSPIINIT = false;

/** @brief Decimal to Hexadecimal convertion table */
static const char spkHexTable[16] = {
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};


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
            PANIC("Failed to retrieve the firmware MAC address.\n");
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
            PANIC("Failed to retrieve the firmware MAC address.\n");
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
    return (uint64_t)esp_timer_get_time() * 1000ULL;
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

SPIClass* HWManager::GetSPIBus(void) noexcept {
    /* Initialize the bus */
    if (!HWManager::_SSPIINIT) {
        HWManager::_SSPIBUS.begin(
            GPIO_SPI_SCK,
            GPIO_SPI_MISO,
            GPIO_SPI_MOSI,
            GPIO_SPI_CS_SD
        );

        LOG_DEBUG("Initializes the SPI bus.\n");

        HWManager::_SSPIINIT = true;
    }

    /* Return the bus */
    return &HWManager::_SSPIBUS;
}

void HWManager::Reboot(void) noexcept {
    LOG_DEBUG("Rebooting compute.\n");

    /* Flush logger */
    LOG_FLUSH();
    DelayExecNs(500000000);

    /* Restart */
    ESP.restart();
    while (true) {
        LOG_ERROR("Failed to restart.\n");
    }
}