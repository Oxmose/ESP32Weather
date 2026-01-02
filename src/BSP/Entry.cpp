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
#include <Settings.h>      /* Settings services */
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

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
/** @brief Stores the Health Monitor instance. */
static HealthMonitor* spHealthMon;
/** @brief Stores the WiFi module instance. */
static WiFiModule*    spWifiModule;
/** @brief Stores the Settings instance. */
static Settings*      spSettings;
/** @brief Stores the System State instance. */
static SystemState*   spSystemState;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void StartWiFi(void) {
    E_Return result;

    /* Initialize the WiFi module */
    spWifiModule = new WiFiModule();
    if (nullptr == spWifiModule) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_WIFI_CREATE, 0);
    }

    /* Set to system state */
    spSystemState->SetWiFiModule(spWifiModule);

    /* Start WiFi */
    result = spWifiModule->Start();
    if (E_Return::NO_ERROR != result) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_WIFI_CREATE, result);
    }

    /* Start the web servers */
    result = spWifiModule->StartWebServers();
    if (E_Return::NO_ERROR != result) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_WEB_START, result);
    }
}

#ifndef UNIT_TEST

void setup(void) {
    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(50000000);

    /* Init system state */
    spSystemState = SystemState::GetInstance();

    /* Initialize the Health Monitor */
    spHealthMon = new HealthMonitor();
    if (nullptr == spHealthMon) {
        LOG_ERROR("Failed to create the Health Monitor instance.\n");
        HWManager::Reboot();
    }
    spSystemState->SetHealthMonitor(spHealthMon);
    LOG_INFO("Health Monitor Initialized.\n");

    /* Welcome output*/
    LOG_INFO("RTHR Weather Station Booting...\n");
    LOG_INFO("#==============================#\n");
    LOG_INFO("| HWUID: %s   |\n", HWManager::GetHWUID());
    LOG_INFO("| " VERSION "  |\n");
    LOG_INFO("#==============================#\n");

    /* Initialize the setting manager */
    spSettings = new Settings();
    if (nullptr == spSettings) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_SETTINGS_CREATE, ERR_UNKNOWN);
    }
    spSystemState->SetSettings(spSettings);

    /* Initialize the WiFi */
    StartWiFi();

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