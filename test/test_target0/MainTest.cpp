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
#include <unity.h>

extern void BSPTests();
extern void SettingsTests();
extern void TimeoutTests();
extern void ValidatorTest();

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

void test_hm_event_handler(void* pParam) {
    /* Dummy */
    (void)pParam;
}

void setup(void) {
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

    UNITY_BEGIN();

    BSPTests();
    SettingsTests();
    TimeoutTests();
    ValidatorTest();

    UNITY_END();
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}