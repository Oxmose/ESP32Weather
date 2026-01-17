#include <Arduino.h>
#include <unity.h>
#include <Timeout.h>
#include <BSP.h>
#include <Errors.h>

static volatile uint32_t valueHandle = 55;

static void wdHandling(void) {
    valueHandle = 42;
}

static void wdHandlingDestroy(void) {
    valueHandle = 420;
}

void test_timeout_base(void) {
    Timeout timeout(100000000);
    uint32_t i;

    TEST_ASSERT_EQUAL(false, timeout.HasTimedOut());

    for (i = 0; i < 1000; ++i) {
        HWManager::DelayExecNs(1000000);
        TEST_ASSERT_EQUAL(false, timeout.HasTimedOut());
        timeout.Notify();
    }
    TEST_ASSERT_EQUAL(false, timeout.HasTimedOut());

    for (i = 0; i < 1000; ++i) {
        HWManager::DelayExecNs(1000000);
    }
    TEST_ASSERT_EQUAL(true, timeout.HasTimedOut());
}

void test_timeout_wd(void) {
    Timeout timeoutWd(1000000, 1000000000, wdHandling);
    uint32_t i;

    valueHandle = 55;
    for (i = 0; i < 500; ++i) {
        HWManager::DelayExecNs(10000000);
        timeoutWd.Notify();
    }
    TEST_ASSERT_EQUAL_UINT32(55, valueHandle);

    HWManager::DelayExecNs(2000000000);
    TEST_ASSERT_EQUAL_UINT32(42, valueHandle);
}

void test_timeout_destroy(void) {
    Timeout* timeoutWd = new Timeout(1000000, 1000000000, wdHandlingDestroy);

    valueHandle = 55;
    HWManager::DelayExecNs(1500000000);
    TEST_ASSERT_EQUAL_UINT32(420, valueHandle);

    delete timeoutWd;

    valueHandle = 55;
    HWManager::DelayExecNs(1500000000);
    TEST_ASSERT_EQUAL_UINT32(55, valueHandle);
}

void TimeoutTests(void) {
    RUN_TEST(test_timeout_base);
    RUN_TEST(test_timeout_destroy);
    RUN_TEST(test_timeout_wd);
}