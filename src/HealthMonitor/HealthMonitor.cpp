/*******************************************************************************
 * @file HealthMonitor.cpp
 *
 * @see HealthMonitor.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief Firmware Health Monitor functionalities.
 *
 * @details Firmware Health Monitor functionalities. Provides watchdogs, error
 * handling and monitoring.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* Hardware services */
#include <string>            /* Standard string */
#include <cstdint>           /* Standard int types */
#include <Logger.h>          /* Logger services */
#include <Arduino.h>         /* Arduino library */
#include <Settings.h>        /* Firmware settings */
#include <APIHandler.h>      /* API Handler errors */

/* Header file */
#include <HealthMonitor.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the real-time task period tolerance in nanoseconds. */
#define HW_RT_TASK_PERIOD_TOLERANCE_NS 500000ULL
/** @brief Real-time task watchdog timeout in nanoseconds. */
#define HW_RT_TASK_WD_TIMEOUT_NS (2 * HW_RT_TASK_PERIOD_NS)

/** @brief Hardware Real-Time Task name. */
#define HW_RT_TASK_NAME "HW-RT_TASK"
/** @brief Hardware Real-Time Task stack size in bytes. */
#define HW_RT_TASK_STACK 4096
/** @brief Hardware Real-Time Task priority. */
#define HW_RT_TASK_PRIO (configMAX_PRIORITIES - 1)
/** @brief Hardware Real-Time Task mapped core ID. */
#define HW_RT_TASK_CORE 0
/** @brief Defines the watchdogs lock timeout in nanoseconds. */
#define WD_LOCK_TIMEOUT_NS 1000000ULL
/** @brief Defines the watchdogs lock timeout in ticks. */
#define WD_LOCK_TIMEOUT_TICKS (pdMS_TO_TICKS(WD_LOCK_TIMEOUT_NS / 1000000ULL))

/** @brief Actions Task name. */
#define HM_ACTIONS_TASK_NAME "HM_ACTIONS_TASK"
/** @brief Actions Task stack size in bytes. */
#define HM_ACTIONS_TASK_STACK 4096
/** @brief Actions Task priority. */
#define HM_ACTIONS_TASK_PRIO (configMAX_PRIORITIES - 2)
/** @brief Actions Task mapped core ID. */
#define HM_ACTIONS_TASK_CORE 0
/** @brief Defines the size of the actions queue. */
#define HM_ACTIONS_TASK_QUEUE_LENGTH 10

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

HealthMonitor::HealthMonitor(void) noexcept {
    this->_lastWDId = 0;
    this->_lastReporterId = 0;

    this->_wdLock = xSemaphoreCreateMutex();
    if (nullptr == this->_wdLock) {
        PANIC("Failed to initialize Health Monitor Watchdogs lock.\n");
    }
    this->_reportersLock = xSemaphoreCreateMutex();
    if (nullptr == this->_reportersLock) {
        PANIC("Failed to initialize Health Monitor Reporters lock.\n");
    }

    /* Add to system state */
    SystemState::GetInstance()->SetHealthMonitor(this);

    /* Initialize the HM tasks */
    RealTimeTaskInit();
    ActionsTaskInit();

    LOG_DEBUG("Health Monitor initialized.\n");
}

HealthMonitor::~HealthMonitor(void) noexcept {
    PANIC("Tried to destroy the Health Monitor.\n");
}

E_Return HealthMonitor::AddWatchdog(Timeout* pTimeout, uint32_t& rId) noexcept {
    E_Return error;

    LOG_DEBUG("Adding HM watchdog.\n");

    /* Check settings */
    if (0 != pTimeout->GetNextWatchdogEvent()) {
        if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
            if (this->_lastWDId < UINT32_MAX) {
                try {
                    this->_watchdogs.emplace(
                        std::make_pair(this->_lastWDId, pTimeout)
                    );
                    rId = this->_lastWDId++;
                    error = E_Return::NO_ERROR;
                }
                catch (std::exception& rExc) {
                    LOG_ERROR(
                        "Failed to add HM watchdog. Error %s.",
                        rExc.what()
                    );
                    error = E_Return::ERR_UNKNOWN;
                }
            }
            else {
                LOG_ERROR("Failed to add HM watchdog. No more memory.\n");
                error = E_Return::ERR_MEMORY;
            }

            if (pdPASS != xSemaphoreGive(this->_wdLock)) {
                PANIC("Failed to release the HM watchdog lock.\n");
            }
        }
        else {
            LOG_ERROR("Failed to acquire watchdogs lock.\n");
            error = E_Return::ERR_HM_TIMEOUT;
        }
    }
    else {
        LOG_ERROR("Failed to add HM watchdog. Invalid parameter.\n");
        error = E_Return::ERR_INVALID_PARAM;
    }

    return error;
}

E_Return HealthMonitor::RemoveWatchdog(const uint32_t kId) noexcept {
    E_Return                                         error;
    std::unordered_map<uint32_t, Timeout*>::iterator it;

    LOG_DEBUG("Removing HM watchdog %d.\n", kId);

    if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
        it = this->_watchdogs.find(kId);
        if (this->_watchdogs.end() != it) {
            try {
                this->_watchdogs.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                LOG_ERROR(
                    "Failed to remove HM watchdog. Error %s.",
                    rExc.what()
                );
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            LOG_ERROR("Failed to remove HM watchdog. No such ID");
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_wdLock)) {
            PANIC("Failed to release the HM watchdog lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire watchdogs lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::AddReporter(HMReporter* pReporter,
                                    uint32_t&   rId) noexcept {
    E_Return error;

    LOG_DEBUG("Adding HM reporter.\n");

    /* Check settings */
    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        if (this->_lastReporterId < UINT32_MAX) {
            try {
                this->_reporters.emplace(
                    std::make_pair(this->_lastReporterId, pReporter)
                );
                rId = this->_lastReporterId++;
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                LOG_ERROR(
                    "Failed to add HM reporter. Error %s.",
                    rExc.what()
                );
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            LOG_ERROR("Failed to add HM reporter. No more memory.\n");
            error = E_Return::ERR_MEMORY;
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            PANIC("Failed to release the HM reporter lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire reporters lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::RemoveReporter(const uint32_t kId) noexcept {
    E_Return                                            error;
    std::unordered_map<uint32_t, HMReporter*>::iterator it;

    LOG_DEBUG("Removing HM reporter %d.\n", kId);

    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        it = this->_reporters.find(kId);
        if (this->_reporters.end() != it) {
            try {
                this->_reporters.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                LOG_ERROR(
                    "Failed to remove HM reporter. Error %s.",
                    rExc.what()
                );
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            LOG_ERROR(
                "Failed to remove HM reporter. No such ID.\n");
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            PANIC("Failed to release the HM reporter lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire reporters lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::AddHMAction(HMReporter* pReporter) noexcept {
    BaseType_t result;
    E_Return   retVal;

    LOG_DEBUG("Adding HM Action.\n");

    /* Place the action in the actions queue */
    result = xQueueSend(this->_actionsQueue, (void*)&pReporter, 0);
    if (pdPASS != result) {
        LOG_ERROR("Failed to add HM action. Action queue is full.\n");
        /* We should never have too many actions */
        retVal = E_Return::ERR_HM_FULL;
    }
    else {
        retVal = E_Return::NO_ERROR;
    }

    return retVal;
}

void HealthMonitor::RealTimeTaskRoutine(void* pHealthMonitor) noexcept {
    BaseType_t     result;
    HealthMonitor* pHM;
    TickType_t     lastWakeTime;

    /* Get HM instance */
    pHM = (HealthMonitor*)pHealthMonitor;

    /* First tick */
    pHM->_pTimeout->Notify();
    lastWakeTime = xTaskGetTickCount();

    while (true) {
        /* Manage deadline miss */
        if (pHM->_pTimeout->HasTimedOut()) {
            PANIC("HM RT task deadline miss.\n");
        }
        pHM->_pTimeout->Notify();

        /* Perform HM checks */
        pHM->CheckWatchdogs();
        pHM->CheckReporters();

        /* Wait for period */
        result = xTaskDelayUntil(
            &lastWakeTime,
            HW_RT_TASK_PERIOD_NS / 1000000 / portTICK_PERIOD_MS
        );
        if (pdPASS != result) {
            PANIC("HM RT task periodic wait failed. Error %d\n", result);
        }
    }
}

void HealthMonitor::HMActionTaskRoutine(void* pHealthMonitor) noexcept {
    HealthMonitor* pHM;
    HMReporter*    pReporter;
    BaseType_t     result;

    pHM = (HealthMonitor*)pHealthMonitor;

    while (true) {
        /* Get the next job */
        result = xQueueReceive(
            pHM->_actionsQueue,
            (void*)&pReporter,
            portMAX_DELAY
        );

        if (pdPASS != result) {
            PANIC("Failed to retrieve HM Action from queue.\n");
        }

        /* Execute the action */
        pReporter->ExecuteAction();
    }
}

void HealthMonitor::CheckWatchdogs(void) const noexcept {
    uint64_t                                               currentTime;
    std::unordered_map<uint32_t, Timeout*>::const_iterator it;

    /* Check for watchdogs */
    currentTime = HWManager::GetTime();
    if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
        for (it = this->_watchdogs.begin();
             this->_watchdogs.end() != it;
             ++it) {
            /* Check for dealine miss */
            if (it->second->GetNextWatchdogEvent() < currentTime) {
                it->second->ExecuteHandler();
            }
        }

        if (pdPASS != xSemaphoreGive(this->_wdLock)) {
            PANIC("Failed to release HM watchdog lock.\n");
        }
    }
    else {
        PANIC("Failed to take HM watchdog lock.\n");
    }
}

void HealthMonitor::CheckReporters(void) const noexcept {
    uint64_t                                                  currentTime;
    std::unordered_map<uint32_t, HMReporter*>::const_iterator it;

    /* Check for HM reporters */
    currentTime = HWManager::GetTime();
    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        for (it = this->_reporters.begin();
             this->_reporters.end() != it;
             ++it) {
            it->second->HealthCheck(currentTime);
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            PANIC("Failed to release HM reporter lock.\n");
        }
    }
    else {
        PANIC("Failed to take HM reporter lock.\n");
    }
}

void HealthMonitor::RealTimeTaskInit(void) noexcept {
    BaseType_t result;

    LOG_DEBUG("Initializing HM RT task.\n");

    /* Create the timeout */
    this->_pTimeout = new Timeout(
        HW_RT_TASK_PERIOD_NS + HW_RT_TASK_PERIOD_TOLERANCE_NS,
        HW_RT_TASK_WD_TIMEOUT_NS,
        HealthMonitor::DeadlineMissHandler
    );
    if (nullptr == this->_pTimeout) {
        PANIC("Failed to create the HM RT task deadline manager.\n");
    }

    /* Create the real-time high-priority task */
    result = xTaskCreatePinnedToCore(
        HealthMonitor::RealTimeTaskRoutine,
        HW_RT_TASK_NAME,
        HW_RT_TASK_STACK,
        this,
        HW_RT_TASK_PRIO,
        &this->_RTTaskHandle,
        HW_RT_TASK_CORE
    );
    if (pdPASS != result) {
        PANIC("Failed to create the HM RT task.\n");
    }
}

void HealthMonitor::ActionsTaskInit(void) noexcept {
    BaseType_t result;

    /* Create the actions queue */
    this->_actionsQueue = xQueueCreate(
        HM_ACTIONS_TASK_QUEUE_LENGTH,
        sizeof(HMReporter*)
    );
    if (nullptr == this->_actionsQueue) {
        PANIC("Failed to create the HM action task queue.\n");
    }

    /* Create the real-time high-priority task */
    result = xTaskCreatePinnedToCore(
        HMActionTaskRoutine,
        HM_ACTIONS_TASK_NAME,
        HM_ACTIONS_TASK_STACK,
        this,
        HM_ACTIONS_TASK_PRIO,
        &this->_actionsTaskHandle,
        HM_ACTIONS_TASK_CORE
    );
    if (pdPASS != result) {
        PANIC("Failed to create the HM action task.\n");
    }
}

void HealthMonitor::DeadlineMissHandler(void) noexcept {
    PANIC("HM RT Task watchdog trigerred.\n");
}