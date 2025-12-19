#include <Arduino.h>
#include <unity.h>
#include <BSP.h>
#include <Logger.h>

#ifndef HM_TEST_EVENT
#error "Environment does not define HM_TEST_EVENT"
#endif


extern void HealthMonitorTests();

void setup(void) {
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    UNITY_BEGIN();

    HealthMonitorTests();

    UNITY_END();
}

void loop(void) {
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}