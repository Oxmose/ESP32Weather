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
#include <BSP.h>             /* Hardware services*/
#include <Logger.h>          /* Firmware logger */
#include <Errors.h>          /* Error codes */
#include <IOTask.h>          /* IO Task manager */
#include <Arduino.h>         /* Arduino library */
#include <version.h>         /* Versionning info */
#include <Storage.h>         /* Storage manager */
#include <Settings.h>        /* Settings services */
#include <WiFiModule.h>      /* WiFi Module driver */
#include <IOLedManager.h>    /* IO Led manager */
#include <ResetManager.h>    /* Reset manager services */
#include <HealthMonitor.h>   /* Health Monitoring */
#include <IOButtonManager.h> /* IO Button manager */

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
static void StartWiFi(void) noexcept;

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
static WiFiModule* spWifiModule;
/** @brief Stores the Settings instance. */
static Settings* spSettings;
/** @brief Stores the IO button manager instance. */
static IOButtonManager* spBtnManager;
/** @brief Stores the IO LED manager instance. */
static IOLedManager* spLedManager;
/** @brief Stores the Storage Manager instance.  */
static Storage* spStorage;
/** @brief Stores the System State instance. */
static SystemState* spSystemState;
/** @brief Stores the Rest Manager instance. */
static ResetManager* spResetManager;
/** @brief Stores the IO task instance. */
static IOTask* spIOTask;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void StartWiFi(void) noexcept {
    E_Return result;

    /* Start WiFi */
    result = spWifiModule->Start();
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to start the WiFi module. Error: %d\n", result);
    }

    /* Start the web servers */
    result = spWifiModule->StartWebServers();
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to start the Web Servers. Error: %d\n", result);
    }
}

#ifndef UNIT_TEST

void setup(void) {
    uint32_t resetActionId;
    E_Return error;

    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(50000000);

    /* Welcome output*/
    LOG_INFO("RTHR Weather Station Booting...\n");
    LOG_INFO("#==============================#\n");
    LOG_INFO("| HWUID: %s   |\n", HWManager::GetHWUID());
    LOG_INFO("| " VERSION "  |\n");
    LOG_INFO("#==============================#\n");

    /* Init system state */
    spSystemState = SystemState::GetInstance();
    if (nullptr == spSystemState) {
        PANIC("Failed to instanciate the System State.\n");
    }

    /* Init system objects */
    spStorage = new Storage();
    if (nullptr == spStorage) {
        PANIC("Failed to instanciate the Storage Manager.\n");
    }
    spHealthMon = new HealthMonitor();
    if (nullptr == spHealthMon) {
        PANIC("Failed to instanciate the Health Monitor.\n");
    }
    spSettings = new Settings();
    if (nullptr == spSettings) {
        PANIC("Failed to instanciate the Settings.\n");
    }
    spWifiModule = new WiFiModule();
    if (nullptr == spWifiModule) {
        PANIC("Failed to instanciate the WiFi Module.\n");
    }
    spBtnManager = new IOButtonManager();
    if (nullptr == spBtnManager) {
        PANIC("Failed to instanciate the IO Button Manager.\n");
    }
    spLedManager = new IOLedManager();
    if (nullptr == spLedManager) {
        PANIC("Failed to instanciate the IO Led Manager.\n");
    }

    /* Setup reset */
    spResetManager = new ResetManager();
    error = spBtnManager->AddAction(spResetManager, resetActionId);
    if (E_Return::NO_ERROR != error) {
        PANIC("Failed to add reset action. Error %d\n", error);
    }

    /* Create the IO task */
    spIOTask = new IOTask();
    if (nullptr == spIOTask) {
        PANIC("Failed to instanciate the IO Task.\n");
    }

    /* Initialize the WiFi */
    StartWiFi();
}

void loop(void) {
    /* Nothing to do, just wait since we cannot delete this task */
    vTaskDelay(1000);
}

#endif /* #ifndef UNIT_TEST */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */