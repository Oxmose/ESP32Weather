#include <Arduino.h>
#include <unity.h>
#include <Settings.h>

void test_init(void) {
    E_Return result;

    /* Initialize the setting manager */
    result = Settings::InitInstance();
    TEST_ASSERT_EQUAL_UINT32(E_Return::NO_ERROR, result);
}

void test_not_found(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = Settings::GetInstance();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->GetSettings("unknown_test", &buffer);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);
}

void test_valid(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = Settings::GetInstance();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->SetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->GetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
}

void test_invalid(void) {
    E_Return  result;
    uint8_t   buffer;
    uint32_t  bufferu32;
    Settings* pSettings;

    pSettings = Settings::GetInstance();
    TEST_ASSERT_NOT_NULL(pSettings);

    result = pSettings->GetSettings("unknown_test_too_long", &buffer);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->SetSettings("unknown_test_too_long", &buffer);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_INVALID, result);

    result = pSettings->SetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->GetSettings("uint8_val", &bufferu32);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetSettings("uint8_val_not", &buffer);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);

    result = pSettings->GetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
}

void test_clear(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = Settings::GetInstance();
    TEST_ASSERT_NOT_NULL(pSettings);

    buffer = 24;

    result = pSettings->SetSettings("uint8_clear", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->ClearCache();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    buffer = 0;
    result = pSettings->GetSettings("uint8_clear", &buffer);
    TEST_ASSERT_EQUAL(E_Return::ERR_SETTING_NOT_FOUND, result);
    TEST_ASSERT_NOT_EQUAL(24, buffer);
}

void test_commit(void) {
    E_Return  result;
    uint8_t   buffer;
    Settings* pSettings;

    pSettings = Settings::GetInstance();
    TEST_ASSERT_NOT_NULL(pSettings);

    buffer = 24;

    result = pSettings->SetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->Commit();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    result = pSettings->ClearCache();
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);

    buffer = 0;
    result = pSettings->GetSettings("uint8_val", &buffer);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(24, buffer);
}

void SettingsTests(void) {
    RUN_TEST(test_init);
    RUN_TEST(test_not_found);
    RUN_TEST(test_valid);
    RUN_TEST(test_invalid);
    RUN_TEST(test_clear);
    RUN_TEST(test_commit);
}