#include <Arduino.h>
#include <unity.h>
#include <BSP.h>

void test_hwuid() {
    const char* hwuid = HWManager::GetHWUID();
    TEST_ASSERT_NOT_NULL(hwuid);
    TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE("RTHRWS-66E83354", hwuid, 15, "None");
}

void test_mac_addr() {
    const char* mac = HWManager::GetMacAddress();
    TEST_ASSERT_NOT_NULL(mac);
    TEST_ASSERT_EQUAL_STRING_LEN_MESSAGE("66:E8:33:54:59:B0", mac, 17, "None");
}

void test_gettime() {
    uint64_t time;
    time = HWManager::GetTime();

    TEST_ASSERT_GREATER_THAN_UINT32(0, (uint32_t)time);
}

void test_delay() {
    uint64_t time;
    uint64_t newTime;

    time = HWManager::GetTime();
    HWManager::DelayExecNs(10000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(5000, 10000, (uint32_t)(newTime - time));

    time = HWManager::GetTime();
    HWManager::DelayExecNs(100000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(5000, 100000, (uint32_t)(newTime - time));

    time = HWManager::GetTime();
    HWManager::DelayExecNs(1000000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(1000000, 1000000, (uint32_t)(newTime - time));

    time = HWManager::GetTime();
    HWManager::DelayExecNs(10000000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(1000000, 10000000, (uint32_t)(newTime - time));

    time = HWManager::GetTime();
    HWManager::DelayExecNs(100000000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(1000000, 100000000, (uint32_t)(newTime - time));

    time = HWManager::GetTime();
    HWManager::DelayExecNs(1000000000);
    newTime = HWManager::GetTime();
    TEST_ASSERT_UINT32_WITHIN(1000000, 1000000000, (uint32_t)(newTime - time));
}

void BSPTests(void) {

    RUN_TEST(test_hwuid);
    RUN_TEST(test_mac_addr);
    RUN_TEST(test_gettime);
    RUN_TEST(test_delay);

}