/*******************************************************************************
 * @file Entry.cpp
 *
 * @see Entry.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Main Module entry point.
 *
 * @details Weather Station Main Module entry point. This file contains the main
 * entry point and loop for the weather station firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>           /* Hardware services*/
#include <Logger.h>        /* Firmware logger */
#include <Errors.h>        /* Error codes */
#include <Arduino.h>       /* Arduino library */
#include <version.h>       /* Versionning info */
#include <Settings.h>      /* Settings manager */
#include <WiFiModule.h>    /* WiFi Module driver */
#include <HealthMonitor.h> /* Health Monitoring */

/* Header file */
#include <Entry.h>

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
/**
 * @brief Starts the WiFi module.
 *
 * @details Starts the WiFi module. On error, Health Monitor is triggered.
 */
static void StartWiFi(void);

/**
 * @brief Starts the WiFi web servers.
 *
 * @details Starts the WiFi web servers. On error, Health Monitor is triggered.
 */
static void StartWebServers(void);

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
static HealthMonitor* spHealthMon;
static WiFiModule*    spWifiModule;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void StartWiFi(void) {
    Settings*   pSettings;
    bool        isAp;
    char        networkSSID[SETTING_NODE_SSID_LENGTH + 1];
    char        networkPassword[SETTING_NODE_PASSWORD_LENGTH + 1];
    E_Return    result;
    size_t      length;

    pSettings = Settings::GetInstance();

    memset(networkSSID, 0, SETTING_NODE_SSID_LENGTH + 1);
    memset(networkPassword, 0, SETTING_NODE_PASSWORD_LENGTH + 1);

    /* Check if we should be AP */
    result = pSettings->GetSettings(
        SETTING_IS_AP,
        (uint8_t*)&isAp,
        sizeof(bool)
    );
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR("Failed to get isAP setting\n");
        /* TODO: Health Monitor Notify */
    }

    if (!isAp) {
        /* Get the network and password for node mode */
        result = pSettings->GetSettings(
            SETTING_NODE_SSID,
            (uint8_t*)networkSSID,
            SETTING_NODE_SSID_LENGTH
        );
        if (E_Return::NO_ERROR == result) {
            result = pSettings->GetSettings(
                SETTING_NODE_PASSWORD,
                (uint8_t*)networkPassword,
                SETTING_NODE_PASSWORD_LENGTH
            );
            if (E_Return::NO_ERROR != result) {
                LOG_ERROR("Failed to retrieve the WiFi network password\n");
                /* TODO: Health Monitor Notify */
            }
        }
        else {
            LOG_ERROR("Failed to retrieve the WiFi network name\n");
            /* TODO: Health Monitor Notify */
        }
    }
    else {
        length = strnlen(HWManager::GetHWUID(), SETTING_NODE_SSID_LENGTH);
        memcpy(networkSSID, HWManager::GetHWUID(), length);
        length = strnlen(
            HWManager::GetMacAddress(),
            SETTING_NODE_PASSWORD_LENGTH
        );
        memcpy(networkPassword, HWManager::GetMacAddress(), length);
    }

    /* Initialize the WiFi module */
    spWifiModule = new WiFiModule(networkSSID, networkPassword);
    if (nullptr == spWifiModule) {
        LOG_ERROR("Failed to instanciate the WiFi Module\n");
        /* TODO: Health Monitor Notify */
    }

    if (E_Return::NO_ERROR != spWifiModule->Start(isAp)) {
        LOG_ERROR("Failed to start the WiFi Module\n");
        /* TODO: Health Monitor Notify */
    }
}

static void StartWebServers(void) {
    uint16_t  webPort;
    uint16_t  apiPort;
    Settings* pSettings;
    E_Return  result;

    /* Get the ports from settings */
    pSettings = Settings::GetInstance();

    result = pSettings->GetSettings(
        SETTING_WEB_PORT,
        (uint8_t*)&webPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR("Failed to retrieve the Web Interface port.\n");
        /* TODO: Health Monitor Notify */
    }
    result = pSettings->GetSettings(
        SETTING_API_PORT,
        (uint8_t*)&apiPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR("Failed to retrieve the API Interface port.\n");
        /* TODO: Health Monitor Notify */
    }

    /* TODO: Remove when HM is done and the default settings are provided */
    webPort = 80;
    apiPort = 8333;

    result = spWifiModule->StartWebServers(webPort, apiPort);
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR("Failed to start the Web Servers.\n");
        /* TODO: Health Monitor Notify */
    }
}

#ifndef UNIT_TEST

void setup(void) {
    E_Return result;

    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    LOG_INFO("RTHR Weather Station Booting...\n");
    LOG_INFO("#==============================#\n");
    LOG_INFO("| HWUID: %s   |\n", HWManager::GetHWUID());
    LOG_INFO("| " VERSION "  |\n");
    LOG_INFO("#==============================#\n");

    /* Initialize the Hardware Layer */
    HWManager::Init();
    LOG_INFO("Hardware Layer Initialized.\n");

    /* Initialize the Health Monitor */
    spHealthMon = HealthMonitor::GetInstance();
    spHealthMon->Init();
    LOG_INFO("Health Monitor Initialized.\n");

    /* Initialize the setting manager */
    result = Settings::InitInstance();
    if (E_Return::NO_ERROR != result) {
        /* TODO: Health Monitor Notify */
    }

    /* Initialize the WiFi */
    StartWiFi();
    StartWebServers();

    /* Set system as started */
    spHealthMon->SetSystemState(E_SystemState::EXECUTING);
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}

#endif /* #ifndef UNIT_TEST */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */