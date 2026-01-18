/*******************************************************************************
 * @file NTPManager.cpp
 *
 * @see NTPManager.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 17/01/2026
 *
 * @version 1.0
 *
 * @brief NTP client manager class.
 *
 * @details NTP client manager class. Provides the time through an NTP client
 * and maintains time between refreshes.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>         /* Hardware services s*/
#include <cstdint>       /* Standard Integer Definitions */
#include <Logger.h>      /* Logging services */
#include <WiFiUdp.h>     /* WiFi UDP manager */
#include <Timeout.h>     /* Timeout manager */
#include <Settings.h>    /* Settings manager */
#include <NTPClient.h>   /* NTP client */
#include <WiFiModule.h>  /* WiFi module services */
#include <SystemState.h> /* System state */

/* Header file */
#include <NTPManager.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the NTP pool server name. */
#define NTP_POOL_SERVER_NAME "pool.ntp.org"
/** @brief Defines the NTP update period in nanoseconds. */
#define NTP_MGR_UPDATE_PERIOD_NS 30000000000ULL
/** @brief Defines the NTP degraded update period in nanoseconds. */
#define NTP_MGR_UPDATE_DEGRADED_PERIOD_NS 15000000000ULL
/** @brief Defines the NTP unhealthy update period in nanoseconds. */
#define NTP_MGR_UPDATE_UNHEALTHY_PERIOD_NS 50000000000ULL
/** @brief NTP loop period tolerance in nanoseconds. */
#define NTP_TASK_PERIOD_TOLERANCE_NS 500000ULL
/** @brief NTP loop watchdog timeout in nanoseconds. */
#define NTP_TASK_WD_TIMEOUT_NS (2 * NTP_MGR_UPDATE_PERIOD_NS)
/** @brief NTP Task name. */
#define NTP_TASK_NAME "NTP_TASK"
/** @brief NTP Task stack size in bytes. */
#define NTP_TASK_STACK 4096
/** @brief NTP Task priority. */
#define NTP_TASK_PRIO (configMAX_PRIORITIES - 5)
/** @brief NTP Task mapped core ID. */
#define NTP_TASK_CORE 0
/**
 * @brief Defines the period in nanoseconds at which the HM should check the
 * status of the NTP manager.
 */
#define NTP_HM_REPORT_PERIOD_NS 60000000000ULL
/** @brief Defines the number of HM checks before setting degraded status. */
#define NTP_HM_REPORT_FAIL_TO_DEGRADE 1
/** @brief Defines the number of HM checks before setting unhealthy status. */
#define NTP_HM_REPORT_FAIL_TO_UNHEALTHY 10
/** @brief Defines the HM reporter name for the NTP manager. */
#define NTP_HM_REPORT_NAME "HM_NTPMGR"

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
NTPManager::NTPManager(void) noexcept {
    int32_t      timeOffset;
    SystemState* pSystemState;
    Settings*    pSettings;
    E_Return     error;
    BaseType_t   result;

    pSystemState = SystemState::GetInstance();
    pSettings = pSystemState->GetSettings();

    /* Get the time offset */
    error = pSettings->GetSettings(
        SETTING_TIME_OFFSET,
        (uint8_t*)&timeOffset,
        sizeof(int32_t)
    );
    if (E_Return::ERR_SETTING_NOT_FOUND == error) {
        LOG_ERROR(
            "Failed to get setting %s. Trying to get default.\n",
            SETTING_TIME_OFFSET
        );
        error = pSettings->GetDefault(
            SETTING_TIME_OFFSET,
            (uint8_t*)&timeOffset,
            sizeof(int32_t)
        );
        if (E_Return::NO_ERROR != error) {
            PANIC(
                "Failed to get setting %s. Error: %d\n",
                SETTING_TIME_OFFSET,
                error
            );
        }
    }
    else {
        PANIC(
            "Failed to get setting %s. Error: %d\n",
            SETTING_TIME_OFFSET,
            error
        );
    }

    /* Create the NTP client */
    this->_pClient = new NTPClient(
        this->_ntpUDP,
        NTP_POOL_SERVER_NAME,
        timeOffset,
        0xFFFFFFFFUL
    );
    this->_pClient->begin();

    /* Setup update time */
    this->_updateFailed = 0;
    this->_periodNs = NTP_MGR_UPDATE_PERIOD_NS;

    /* Add the health reporter */
    this->_pReporter = new NTPManagerHealthReporter(
        S_HMReporterParam {
            NTP_HM_REPORT_PERIOD_NS,
            NTP_HM_REPORT_FAIL_TO_DEGRADE,
            NTP_HM_REPORT_FAIL_TO_UNHEALTHY,
            NTP_HM_REPORT_NAME
        },
        this
    );
    if (nullptr == this->_pReporter) {
        PANIC("Failed to create WiFi module HM reporter.\n");
    }

    /* Set system state */
    SystemState::GetInstance()->SetNTPManager(this);

    /* Create the timeout */
    this->_pTimeout = new Timeout(
        NTP_MGR_UPDATE_PERIOD_NS + NTP_TASK_PERIOD_TOLERANCE_NS,
        NTP_TASK_WD_TIMEOUT_NS,
        NTPManager::DeadlineMissHandler
    );
    if (nullptr == this->_pTimeout) {
        PANIC("Failed to create the NTP task deadline manager.\n");
    }

    /* Create the task */
    result = xTaskCreatePinnedToCore(
        NTPManager::NTPTaskRoutine,
        NTP_TASK_NAME,
        NTP_TASK_STACK,
        this,
        NTP_TASK_PRIO,
        &this->_taskHandle,
        NTP_TASK_CORE
    );
    if (pdPASS != result) {
        PANIC("Failed to create the NTP task routine task.\n");
    }

    LOG_DEBUG("Created the NTP manager.\n");
}

NTPManager::~NTPManager(void) noexcept {
    PANIC("Tried to destroy the NTP Manager.\n");
}

void NTPManager::Update(void) noexcept {
    if (!this->_pClient->update()) {
        ++this->_updateFailed;
    }
    else {
        this->_updateFailed = 0;
        /* Set back the correct refresh period on success. */
        this->_periodNs = NTP_MGR_UPDATE_PERIOD_NS;
    }
}

uint8_t NTPManager::GetDay(void) const noexcept {
    return this->_pClient->getDay();
}

uint8_t NTPManager::GetHours(void) const noexcept {
    return this->_pClient->getHours();
}

uint8_t NTPManager::GetMinutes(void) const noexcept {
    return this->_pClient->getMinutes();
}

uint8_t NTPManager::GetSeconds(void) const noexcept {
    return this->_pClient->getSeconds();
}

uint64_t NTPManager::GetTimestamp(void) const noexcept {
    return this->_pClient->getEpochTime();
}

void NTPManager::SetTimeOffset(const int32_t kOffset) noexcept {
    this->_pClient->setTimeOffset(kOffset);
}

NTPManagerHealthReporter::NTPManagerHealthReporter(
    const S_HMReporterParam& krParam,
    NTPManager*              pModule) noexcept :
    HMReporter(krParam) {
    this->_pModule = pModule;
}

NTPManagerHealthReporter::~NTPManagerHealthReporter(void) noexcept {
    /* Nothing to do */
}

void NTPManagerHealthReporter::OnDegraded(void) noexcept {
    LOG_ERROR(
        "NTP Manager is degraded, expecting to recover before unhealthy"
        " status.\n"
    );

    /* Set the degraded refresh period */
    this->_pModule->_periodNs = NTP_MGR_UPDATE_DEGRADED_PERIOD_NS;
}

void NTPManagerHealthReporter::OnUnhealthy(void) noexcept {
    LOG_ERROR(
        "NTP Manager is unhelathy. Time might be inacurate.\n"
    );

    /* Set the unhelathy refresh period */
    this->_pModule->_periodNs = NTP_MGR_UPDATE_UNHEALTHY_PERIOD_NS;
}

bool NTPManagerHealthReporter::PerformCheck(void) noexcept {
    bool          checkResult;
    SystemState*  pSystemState;
    S_WiFiConfig  config;

    /* Get the current WiFi configuration */
    pSystemState = SystemState::GetInstance();
    pSystemState->GetWiFiModule()->GetConfiguration(&config);

    checkResult = true;
    if (!config.isAP) {
        /* Check WiFi status */
        if (WiFi.isConnected()) {
            /* Endure we did not fail the updates */
            if (0 != this->_pModule->_updateFailed) {
                checkResult = false;
            }
        }
    }

    return checkResult;
}

void NTPManager::NTPTaskRoutine(void* pInstance) noexcept {
    BaseType_t  result;
    TickType_t  lastWakeTime;
    NTPManager* pManager;

    /* Get instance */
    pManager = (NTPManager*)pInstance;

    /* First tick */
    pManager->_pTimeout->Notify();
    lastWakeTime = xTaskGetTickCount();

    while (true) {

        /* Manage deadline miss */
        if (pManager->_pTimeout->HasTimedOut()) {
            PANIC("NTP task deadline miss.\n");
        }
        pManager->_pTimeout->Notify();

        /* Update the instance */
        pManager->Update();

        /* Wait for period */
        result = xTaskDelayUntil(
            &lastWakeTime,
            pManager->_periodNs / 1000000 / portTICK_PERIOD_MS
        );
        if (pdPASS != result) {
            PANIC("NTP task periodic wait failed. Error %d\n", result);
        }
    }
}

void NTPManager::DeadlineMissHandler(void) noexcept {
    PANIC("NTP task watchdog triggered.\n");
}