#include <Arduino.h>
#include <unity.h>
#include <BSP.h>
#include <Logger.h>
#include <HealthMonitor.h>
#include <SystemState.h>
#include <Settings.h>

#ifndef HM_TEST_EVENT
#error "Environment does not define HM_TEST_EVENT"
#endif

/** @brief Stores the Health Monitor instance. */
static HealthMonitor* spHealthMon;
/** @brief Stores the WiFi module instance. */
static WiFiModule*    spWifiModule;
/** @brief Stores the Settings instance. */
static Settings*      spSettings;
/** @brief Stores the System State instance. */
static SystemState*   spSystemState;

extern void HealthMonitorTests();

void setup(void) {
    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(50000000);

    /* Init system state */
    spSystemState = SystemState::GetInstance();

    /* Initialize the Health Monitor */
    spHealthMon = new HealthMonitor();
    if (nullptr == spHealthMon) {
        HWManager::Reboot();
    }
    spSystemState->SetHealthMonitor(spHealthMon);

    /* Initialize the setting manager */
    spSettings = new Settings();
    if (nullptr == spSettings) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_SETTINGS_CREATE, ERR_UNKNOWN);
    }
    spSystemState->SetSettings(spSettings);

    UNITY_BEGIN();

    HealthMonitorTests();

    UNITY_END();
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}