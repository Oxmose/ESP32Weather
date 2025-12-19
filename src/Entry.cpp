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
/** @brief Stores the Health Monitor Instance. */
static HealthMonitor* spHealthMon;
/** @brief Stores the WiFi module instance. */
static WiFiModule*    spWifiModule;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void StartWiFi(void) {
    Settings*              pSettings;
    bool                   isAp;
    char                   networkSSID[SETTING_NODE_SSID_LENGTH + 1];
    char                   networkPassword[SETTING_NODE_PASS_LENGTH + 1];
    E_Return               result;
    size_t                 length;
    S_HMParamSettingAccess hmAccessParam;

    pSettings = Settings::GetInstance();

    memset(networkSSID, 0, SETTING_NODE_SSID_LENGTH + 1);
    memset(networkPassword, 0, SETTING_NODE_PASS_LENGTH + 1);

    /* Check if we should be AP */
    result = pSettings->GetSettings(
        SETTING_IS_AP,
        (uint8_t*)&isAp,
        sizeof(bool)
    );
    if (E_Return::NO_ERROR != result) {
        hmAccessParam.kpSettingName = SETTING_IS_AP;
        hmAccessParam.pSettingBuffer = (void*)&isAp;
        hmAccessParam.settingBufferSize = sizeof(bool);
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,
            &hmAccessParam
        );
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
                SETTING_NODE_PASS,
                (uint8_t*)networkPassword,
                SETTING_NODE_PASS_LENGTH
            );
            if (E_Return::NO_ERROR != result) {
                hmAccessParam.kpSettingName = SETTING_NODE_PASS;
                hmAccessParam.pSettingBuffer = (void*)networkPassword;
                hmAccessParam.settingBufferSize = SETTING_NODE_PASS_LENGTH;
                HealthMonitor::GetInstance()->ReportHM(
                    E_HMEvent::HM_EVENT_SETTINGS_LOAD,
                    &hmAccessParam
                );
            }
        }
        else {
            hmAccessParam.kpSettingName = SETTING_NODE_SSID;
            hmAccessParam.pSettingBuffer = (void*)networkSSID;
            hmAccessParam.settingBufferSize = SETTING_NODE_SSID_LENGTH;
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_SETTINGS_LOAD,
                &hmAccessParam
            );
        }
    }
    else {
        length = strnlen(HWManager::GetHWUID(), SETTING_NODE_SSID_LENGTH);
        memcpy(networkSSID, HWManager::GetHWUID(), length);
        length = strnlen(
            HWManager::GetMacAddress(),
            SETTING_NODE_PASS_LENGTH
        );
        memcpy(networkPassword, HWManager::GetMacAddress(), length);
    }

    /* Initialize the WiFi module */
    spWifiModule = new WiFiModule(networkSSID, networkPassword);
    if (nullptr == spWifiModule) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WIFI_CREATE,
            (void*)0
        );
    }

    /* Start WiFi */
    result = spWifiModule->Start(isAp);
    if (E_Return::NO_ERROR != result) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WIFI_CREATE,
            (void*)result
        );
    }
}

static void StartWebServers(void) {
    uint16_t               webPort;
    uint16_t               apiPort;
    Settings*              pSettings;
    E_Return               result;
    S_HMParamSettingAccess hmAccessParam;

    /* Get the ports from settings */
    pSettings = Settings::GetInstance();

    result = pSettings->GetSettings(
        SETTING_WEB_PORT,
        (uint8_t*)&webPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        hmAccessParam.kpSettingName = SETTING_WEB_PORT;
        hmAccessParam.pSettingBuffer = (void*)&webPort;
        hmAccessParam.settingBufferSize = sizeof(uint16_t);
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,
            &hmAccessParam
        );
    }
    result = pSettings->GetSettings(
        SETTING_API_PORT,
        (uint8_t*)&apiPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        hmAccessParam.kpSettingName = SETTING_API_PORT;
        hmAccessParam.pSettingBuffer = (void*)&apiPort;
        hmAccessParam.settingBufferSize = sizeof(uint16_t);
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,
            &hmAccessParam
        );
    }

    result = spWifiModule->StartWebServers(webPort, apiPort);
    if (E_Return::NO_ERROR != result) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WEB_START,
            (void*)result
        );
    }
}

#ifndef UNIT_TEST

void setup(void) {
    E_Return result;

    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    /* Initialize the Health Monitor */
    spHealthMon = HealthMonitor::GetInstance();
    spHealthMon->Init();
    LOG_INFO("Health Monitor Initialized.\n");

    LOG_INFO("RTHR Weather Station Booting...\n");
    LOG_INFO("#==============================#\n");
    LOG_INFO("| HWUID: %s   |\n", HWManager::GetHWUID());
    LOG_INFO("| " VERSION "  |\n");
    LOG_INFO("#==============================#\n");

    /* Initialize the setting manager */
    result = Settings::InitInstance();
    if (E_Return::NO_ERROR != result) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_CREATE,
            (void*)result
        );
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