#include <Arduino.h>
#include <unity.h>
#include <HealthMonitor.h>
#include <Timeout.h>
#include <BSP.h>

static volatile uint32_t valueHandle = 55;

static void wdHandling(void) {
    valueHandle = 42;
}

static void wdDummy(void) {
}

void test_instance(void) {
    HealthMonitor* pHM;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    pHM->Init();
}

void test_add_wd(void) {
    HealthMonitor* pHM;
    Timeout timeout(1000000);
    Timeout timeoutWd(1000000, 1000000000, wdDummy);
    E_Return result;
    uint32_t id;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    id = 0xFFFFFFF8;

    result = pHM->AddWatchdog(&timeout, id);
    TEST_ASSERT_EQUAL(E_Return::ERR_INVALID_PARAM, result);
    TEST_ASSERT_EQUAL_UINT32(0xFFFFFFF8, id);

    result = pHM->AddWatchdog(&timeoutWd, id);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_UINT32(1, id);
}

void test_remove_wd(void) {
    HealthMonitor* pHM;
    Timeout timeoutWd(1000000, 1000000000, wdDummy);
    E_Return result;
    uint32_t id;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    id = 0xFFFFFFF8;

    result = pHM->AddWatchdog(&timeoutWd, id);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_UINT32(3, id);

    result = pHM->RemoveWatchdog(4);
    TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);

    result = pHM->RemoveWatchdog(3);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
}

void test_exec_wd(void) {
    HealthMonitor* pHM;
    Timeout timeoutWd(1000000, 1000000000, wdHandling);
    E_Return result;
    uint32_t id;

    valueHandle = 55;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    result = pHM->AddWatchdog(&timeoutWd, id);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_UINT32(5, id);

    HWManager::DelayExecNs(1000000000 * 2);
    TEST_ASSERT_EQUAL_UINT32(42, valueHandle);
}

void test_clean(void) {
    HealthMonitor* pHM;
    uint8_t i;
    E_Return result;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    for (i = 0; i < 50; ++i) {
        result = pHM->RemoveWatchdog(i);
        if (i < 6) {
            if (i == 0 || i == 2 || i == 3 || i == 4) {
                TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);
            }
            else {
                TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
            }
        }
        else {
            TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);
        }
    }

    valueHandle = 55;
    HWManager::DelayExecNs(1000000000 * 2);
    TEST_ASSERT_EQUAL_UINT32(55, valueHandle);
}

void HealthMonitorTests(void) {
    RUN_TEST(test_instance);
    RUN_TEST(test_add_wd);
    RUN_TEST(test_remove_wd);
    RUN_TEST(test_exec_wd);
    RUN_TEST(test_clean);
}