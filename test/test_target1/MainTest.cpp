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
    /* Init system state */
    spSystemState = SystemState::GetInstance();

    /* Initialize the Health Monitor */
    spHealthMon = new HealthMonitor();
    if (nullptr == spHealthMon) {
        HWManager::Reboot(false);
    }
    spSystemState->SetHealthMonitor(spHealthMon);

    /* Initialize the setting manager */
    spSettings = new Settings();
    spSystemState->SetSettings(spSettings);

    UNITY_BEGIN();

    HealthMonitorTests();

    UNITY_END();
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}