#include <Arduino.h>
#include <unity.h>
#include <BSP.h>
#include <Logger.h>
#include <HealthMonitor.h>

extern void BSPTests();
extern void SettingsTests();
extern void TimeoutTests();

void test_hm_event_handler(void* pParam) {
    /* Dummy */
    (void)pParam;
}

void setup(void) {
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    HealthMonitor::GetInstance()->Init();

    UNITY_BEGIN();

    BSPTests();
    SettingsTests();
    TimeoutTests();

    UNITY_END();
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}