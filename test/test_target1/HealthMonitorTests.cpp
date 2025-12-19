#include <Arduino.h>
#include <unity.h>
#include <HealthMonitor.h>
#include <Timeout.h>
#include <BSP.h>
#include <Logger.h>

static volatile uint32_t valueHandle = 55;
static volatile bool isHmEventTriggered = false;
static volatile uint32_t degradedTimes = 0;
static volatile uint32_t unhealthyTimes = 0;
static volatile bool startFail = false;

class TestHMReporter : public HMReporter {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Initializes the Health Reporter.
         *
         * @details Initializes the Health Reporter. The next check period
         * will be started after the object initialization.
         *
         * @param[in] kCheckPeriodNs The health check period in nanoseconds.
         * @param[in] kFailToDegrade The number of times the health check should
         * fail before entering degraded state. Must be greater than 0. If
         * greater or equal to to kFailToUnhealthy, this parameter is ignored.
         * @param[in] kFailToUnhealthy The number of times the health check
         * should fail before entering unhealthy state. Must be greater than 0.
         * @param[in] krName The name of the check for reports.
         *
         */
        TestHMReporter(const uint64_t     kCheckPeriodNs,
                                 const uint32_t     kFailToDegrade,
                                 const uint32_t     kFailToUnhealthy,
                                 const std::string& krName) noexcept :
            HMReporter(kCheckPeriodNs, kFailToDegrade, kFailToUnhealthy, krName) {
        }
        /**
         * @brief TestHMReporter Interface Destructor.
         *
         * @details TestHMReporter Interface Destructor. Release the
         * used memory.
         */
        virtual ~TestHMReporter(void) noexcept {

        }

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /**
         * @brief Action executed on degraded health.
         *
         * @details Action executed on degraded health. Must be implemented by
         * the inherited class.
         */
        virtual void OnDegraded(void) noexcept {
            ++degradedTimes;
        }
        /**
         * @brief Action executed on unhealthy health.
         *
         * @details Action executed on unhealthy health. Must be implemented by
         * the inherited class.
         */
        virtual void OnUnhealthy(void) noexcept {
            ++unhealthyTimes;
        }
        /**
         * @brief Checks the current health for the report.
         *
         * @details  Checks the current health for the report. Must be
         * implemented by the inherited class.
         *
         * @return The function returns the check status: passed is True,
         * otherwise False.
         */
        virtual bool PerformCheck(void) noexcept {
            return !startFail;
        }

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:

};

static void wdHandling(void) {
    valueHandle = 42;
}

static void wdDummy(void) {

}

void test_hm_event_handler(void* pParam) {
    S_HMParamSettingAccess* pSettingParam;

    pSettingParam = (S_HMParamSettingAccess*)pParam;

    TEST_ASSERT_EQUAL_STRING("TEST_NAME", pSettingParam->kpSettingName);
    TEST_ASSERT_EQUAL((void*)42, pSettingParam->pSettingBuffer);
    TEST_ASSERT_EQUAL(555, pSettingParam->settingBufferSize);

    isHmEventTriggered = true;
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

void test_event(void) {
    HealthMonitor* pHM;
    S_HMParamSettingAccess param;

    pHM = HealthMonitor::GetInstance();
    TEST_ASSERT_NOT_NULL(pHM);

    param.kpSettingName = "TEST_NAME";
    param.pSettingBuffer = (void*)42;
    param.settingBufferSize = 555;

    pHM->ReportHM(
        E_HMEvent::HM_EVENT_TEST,
        (void*)&param
    );

    TEST_ASSERT_EQUAL(true, isHmEventTriggered);
}

void test_reporter_standalone(void) {
    unhealthyTimes = 0;
    degradedTimes = 0;
    startFail = false;

    uint64_t initTime;
    TestHMReporter reporter(10000000, 5, 10, "TestReporter");
    TestHMReporter reporter2(10000000, 5, 5, "TestReporter2");

    /* Basic Tests */
    initTime = HWManager::GetTime();
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    TEST_ASSERT_EQUAL_STRING("TestReporter", reporter.GetName().c_str());
    TEST_ASSERT_EQUAL_STRING("TestReporter2", reporter2.GetName().c_str());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter2.GetStatus());
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    reporter.HealthCheck(initTime);
    reporter2.HealthCheck(initTime);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter2.GetStatus());

    reporter.HealthCheck(HWManager::GetTime() + 10000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 10000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 20000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 20000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 30000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 30000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());

    /* Degraded and Unhealthy */
    startFail = true;
    reporter.HealthCheck(HWManager::GetTime() + 40000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(1, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 50000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(2, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 60000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(3, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 70000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(4, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 80000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(5, reporter.GetFailureCount());
    while (1 != degradedTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter.HealthCheck(HWManager::GetTime() + 90000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(6, reporter.GetFailureCount());
    while (2 != degradedTimes) {}
    HWManager::DelayExecNs(1000000);
    reporter.HealthCheck(HWManager::GetTime() + 100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(7, reporter.GetFailureCount());
    while (3 != degradedTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter.HealthCheck(HWManager::GetTime() + 110000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(8, reporter.GetFailureCount());
    while (4 != degradedTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter.HealthCheck(HWManager::GetTime() + 120000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(9, reporter.GetFailureCount());
    while (5 != degradedTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter.HealthCheck(HWManager::GetTime() + 130000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(10, reporter.GetFailureCount());
    while (1 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter.HealthCheck(HWManager::GetTime() + 140000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(11, reporter.GetFailureCount());
    while (2 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);

    reporter2.HealthCheck(HWManager::GetTime() + 40000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(1, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 50000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(2, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 60000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(3, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 70000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(4, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 80000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(5, reporter2.GetFailureCount());
    while (3 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 90000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(6, reporter2.GetFailureCount());
    while (4 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(7, reporter2.GetFailureCount());
    while (5 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 110000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(8, reporter2.GetFailureCount());
    while (6 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 120000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(9, reporter2.GetFailureCount());
    while (7 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 130000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(10, reporter2.GetFailureCount());
    while (8 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);
    reporter2.HealthCheck(HWManager::GetTime() + 140000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(11, reporter2.GetFailureCount());
    while (9 != unhealthyTimes) {}
    HWManager::DelayExecNs(10000000);

    startFail = false;

    reporter.HealthCheck(HWManager::GetTime() + 150000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 160000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 170000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    reporter.HealthCheck(HWManager::GetTime() + 180000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    reporter2.HealthCheck(HWManager::GetTime() + 150000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 160000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 170000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());
    reporter2.HealthCheck(HWManager::GetTime() + 180000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());

    /* Check counts */
    TEST_ASSERT_EQUAL(5, degradedTimes);
    TEST_ASSERT_EQUAL(9, unhealthyTimes);
}

void test_reporter0(void) {
    unhealthyTimes = 0;
    degradedTimes = 0;
    startFail = false;

    E_Return result;
    uint32_t reporterId;
    TestHMReporter reporter(100000000, 5, 10, "TestReporter");

    /* Basic Tests */
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    TEST_ASSERT_EQUAL_STRING("TestReporter", reporter.GetName().c_str());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter.GetStatus());

    /* Add the Reporter with 500ms of interval */
    result = HealthMonitor::GetInstance()->AddReporter(&reporter, reporterId);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(0, reporterId);


    /* Basic checks */
    HWManager::DelayExecNs(50000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DISABLED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    /* Degraded and Unhealthy */
    startFail = true;

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(1, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(2, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(3, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(4, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(5, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(6, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(7, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(8, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_DEGRADED, reporter.GetStatus());
    TEST_ASSERT_EQUAL(9, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(10, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(11, reporter.GetFailureCount());

    startFail = false;

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    /* Check counts */
    TEST_ASSERT_EQUAL(5, degradedTimes);
    TEST_ASSERT_EQUAL(2, unhealthyTimes);

    result = HealthMonitor::GetInstance()->RemoveReporter(reporterId);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    result = HealthMonitor::GetInstance()->RemoveReporter(reporterId);
    TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);
}

void test_reporter1(void) {
    unhealthyTimes = 0;
    degradedTimes = 0;
    startFail = false;

    E_Return result;
    uint32_t reporterId;
    uint32_t reporter2Id;
    TestHMReporter reporter(100000000, 5, 10, "TestReporter");


    /* Add the two Reporter with 50ms of interval */
    result = HealthMonitor::GetInstance()->AddReporter(&reporter, reporterId);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(1, reporterId);

    HWManager::DelayExecNs(50000000);

    TestHMReporter reporter2(100000000, 5, 5, "TestReporter2");

    result = HealthMonitor::GetInstance()->AddReporter(&reporter2, reporter2Id);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    TEST_ASSERT_EQUAL(2, reporter2Id);

    HWManager::DelayExecNs(500000000);

    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());

    /* Degraded and Unhealthy */
    startFail = true;

    HWManager::DelayExecNs(1060000000);

    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(11, reporter.GetFailureCount());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_UNHEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(11, reporter2.GetFailureCount());

    startFail = false;

    HWManager::DelayExecNs(100000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());

    HWManager::DelayExecNs(500000000);
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter.GetFailureCount());
    TEST_ASSERT_EQUAL(E_HMStatus::HM_HEALTHY, reporter2.GetStatus());
    TEST_ASSERT_EQUAL(0, reporter2.GetFailureCount());

    /* Check counts */
    TEST_ASSERT_EQUAL(5, degradedTimes);
    TEST_ASSERT_EQUAL(9, unhealthyTimes);

    result = HealthMonitor::GetInstance()->RemoveReporter(reporterId);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    result = HealthMonitor::GetInstance()->RemoveReporter(reporterId);
    TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);
    result = HealthMonitor::GetInstance()->RemoveReporter(reporter2Id);
    TEST_ASSERT_EQUAL(E_Return::NO_ERROR, result);
    result = HealthMonitor::GetInstance()->RemoveReporter(reporter2Id);
    TEST_ASSERT_EQUAL(E_Return::ERR_NO_SUCH_ID, result);
}

void HealthMonitorTests(void) {
    RUN_TEST(test_instance);
    RUN_TEST(test_add_wd);
    RUN_TEST(test_remove_wd);
    RUN_TEST(test_exec_wd);
    RUN_TEST(test_clean);
    RUN_TEST(test_event);
    RUN_TEST(test_reporter_standalone);
    RUN_TEST(test_reporter0);
    RUN_TEST(test_reporter1);
}