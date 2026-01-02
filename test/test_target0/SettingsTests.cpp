#include <Arduino.h>
#include <unity.h>
#include <Settings.h>
#include <SystemState.h>


void test_not_found(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->GetSettings("unknown_test", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);
}

void test_valid(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->SetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->GetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
}

void test_invalid(void) {
    E_Return  result;
    uint8_t   buffer;
    uint32_t  bufferu32;
    Settings* pSettings;

    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->GetSettings("unknown_test_too_long", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->SetSettings("unknown_test_too_long", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_INVALID, result);

    result = pSettings->SetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->GetSettings("uint8_val", (uint8_t*)&bufferu32, sizeof(uint32_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetSettings("uint8_val_not", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
}

void test_clear(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    buffer = 24;

    result = pSettings->SetSettings("uint8_clear", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->ClearCache();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    buffer = 0;
    result = pSettings->GetSettings("uint8_clear", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);
    TEST_ASSERT_NOT_EQUAL(24, buffer);
}

void test_commit(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    buffer = 24;

    result = pSettings->SetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->Commit();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->ClearCache();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    buffer = 0;
    result = pSettings->GetSettings("uint8_val", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(24, buffer);
}

void test_default(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;
    char      ssidBuff[32];
    char      passBuff[32];
    char      ipBuff[32];
    uint16_t  uint16Buff;


    pSettings = SystemState::GetInstance()->GetSettings();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->GetDefault("unknown_test_too_long", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetDefault("unknown_test", &buffer, sizeof(uint8_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetDefault("is_ap", &buffer, sizeof(uint32_t));
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    /* Check all default values */
    result = pSettings->GetDefault("is_ap", &buffer, sizeof(bool));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(true, buffer);
    result = pSettings->GetDefault("node_static", &buffer, sizeof(bool));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(false, buffer);
    result = pSettings->GetDefault("node_ssid", (uint8_t*)ssidBuff, 32);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("RTHR_NODE", ssidBuff);
    result = pSettings->GetDefault("node_pass", (uint8_t*)passBuff, 32);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("RTHR_PASS", passBuff);
    result = pSettings->GetDefault("web_port", (uint8_t*)&uint16Buff, sizeof(uint16_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(80, uint16Buff);
    result = pSettings->GetDefault("api_port", (uint8_t*)&uint16Buff, sizeof(uint16_t));
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(8333, uint16Buff);
    result = pSettings->GetDefault("node_st_ip", (uint8_t*)ipBuff, 15);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("192.168.1.200", ipBuff);
    result = pSettings->GetDefault("node_st_gate", (uint8_t*)ipBuff, 15);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("192.168.1.100", ipBuff);
    result = pSettings->GetDefault("node_st_subnet", (uint8_t*)ipBuff, 15);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("255.255.255.0", ipBuff);
    result = pSettings->GetDefault("node_st_pdns", (uint8_t*)ipBuff, 15);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("1.1.1.1", ipBuff);
    result = pSettings->GetDefault("node_st_sdns", (uint8_t*)ipBuff, 15);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL_STRING("4.4.4.4", ipBuff);
}

void SettingsTests(void) {
    RUN_TEST(test_not_found);
    RUN_TEST(test_valid);
    RUN_TEST(test_invalid);
    RUN_TEST(test_clear);
    RUN_TEST(test_commit);
    RUN_TEST(test_default);
}