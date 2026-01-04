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
#include <Arduino.h>         /* Arduino library */
#include <version.h>         /* Versionning info */
#include <Timeout.h>         /* Timeout services */
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
/** @brief Main loop period in nanoseconds. */
#define MAIN_LOOP_PERIOD_NS 25000000ULL

/** @brief Main loop period tolerance in nanoseconds. */
#define MAIN_LOOP_PERIOD_TOLERANCE_NS 2000000ULL

/** @brief Main loop watchdog timeout in nanoseconds. */
#define MAIN_LOOP_WD_TIMEOUT_NS (2 * MAIN_LOOP_PERIOD_NS)

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

/**
 * @brief Handles the main loop watchdog trigger.
 *
 * @details Handles the main loop watchdog trigger. Health monitor is used to
 * correct the error.
 */
static void MainLoopWatchdogHandler(void) noexcept;

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
/** @brief Stores the System State instance. */
static SystemState* spSystemState;
/** @brief Stores the Rest Manager instance. */
static ResetManager* spResetManager;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void StartWiFi(void) noexcept {
    E_Return result;

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

    /* Init system objects */
    spHealthMon = new HealthMonitor();
    spSystemState->SetHealthMonitor(spHealthMon);

    spSettings = new Settings();
    spSystemState->SetSettings(spSettings);

    spWifiModule = new WiFiModule();
    spSystemState->SetWiFiModule(spWifiModule);

    spBtnManager = new IOButtonManager();
    spSystemState->SetIOButtonManager(spBtnManager);

    spLedManager = new IOLedManager();
    spSystemState->SetIOLedManager(spLedManager);

    /* Initialize the WiFi */
    StartWiFi();

    // /* Setup reset */
    spResetManager = new ResetManager();
    error = spBtnManager->AddAction(spResetManager, resetActionId);
    if (E_Return::NO_ERROR != error) {
        LOG_ERROR("Failed to add reset action. Error %d\n", error);
        HWManager::Reboot();
    }

    /* Set system as started */
    spHealthMon->SetSystemState(E_SystemState::EXECUTING);
}

void loop(void) {
    /* Main loop timeout manager */
    static Timeout sMainLoopTimeout(
        MAIN_LOOP_PERIOD_NS + MAIN_LOOP_PERIOD_TOLERANCE_NS,
        MAIN_LOOP_WD_TIMEOUT_NS,
        MainLoopWatchdogHandler
    );

    uint64_t startTime;
    uint64_t endTime;

    // if (!sMainLoopTimeout.Check()) {
    //     HM_REPORT_EVENT(HM_EVENT_MAIN_LOOP_DEADLINE_MISS, nullptr);
    // }
    sMainLoopTimeout.Tick();

    /* Get iteration start time */
    startTime = HWManager::GetTime();

    /* Update the IO buttons */
    spBtnManager->Update();

    // /* Update the LED */
    spLedManager->Update();

    /* Get iteration end time and sleep time */
    endTime = HWManager::GetTime();
    vTaskDelay((MAIN_LOOP_PERIOD_NS - (endTime - startTime)) / 1000000ULL /
               portTICK_PERIOD_MS);
}

void MainLoopWatchdogHandler(void) noexcept {
    HM_REPORT_EVENT(HM_EVENT_MAIN_LOOP_DEADLINE_MISS, nullptr);
}

#endif /* #ifndef UNIT_TEST */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */