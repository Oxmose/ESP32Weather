#include <Arduino.h>
#include <unity.h>
#include <BSP.h>
#include <Logger.h>

extern void BSPTests();
extern void SettingsTests();
extern void HealthMonitorTests();
extern void TimeoutTests();

void setup(void) {
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    UNITY_BEGIN();

    BSPTests();
    SettingsTests();
    HealthMonitorTests();
    TimeoutTests();

    UNITY_END();
}

void loop(void) {
    // Not used for unit testing
}