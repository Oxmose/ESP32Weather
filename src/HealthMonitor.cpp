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
#include <esp32-hal-timer.h> /* ESP32 Timers */

/* Header file */
#include <HealthMonitor.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/**
 * @brief Defines the real-time task period in nanoseconds.
 */
#define HW_RT_TASK_PERIOD_NS 10000000ULL

/**
 * @brief Defines the real-time task period tolerance in nanoseconds.
 */
#define HW_RT_TASK_PERIOD_TOLERANCE_NS 100000ULL

/**
 * @brief Defines the real-time task period deadline in nanoseconds.
 */
#define HW_RT_TASK_DEADLINE_MISS_DELAY \
    (HW_RT_TASK_PERIOD_NS + HW_RT_TASK_PERIOD_TOLERANCE_NS)

/** @brief Real-Time Task timer identifier. */
#define HW_RT_TASK_TIMER 0

/** @brief Real-Time Task timer prescaler. */
#define HW_RT_TASK_TIMER_PRESCALER 80ULL

/** @brief Real-Time Task timer input clock frequency. */
#define HW_RT_TASK_TIMER_FREQ 80000000ULL

/** @brief Decimator of the Real-Time Task timer interrupt. */
#define HW_RT_TASK_TIMER_DEC_INT                                \
    (                                                           \
        (HW_RT_TASK_TIMER_FREQ / HW_RT_TASK_TIMER_PRESCALER) /  \
        (1000000000ULL / HW_RT_TASK_PERIOD_NS)                  \
    )

/** @brief Hardware Real-Time Task name. */
#define HW_RT_TASK_NAME "HW-RT_Task"

/** @brief Hardware Real-Time Task stack size in bytes. */
#define HW_RT_TASK_STACK 4096

/** @brief Hardware Real-Time Task priority. */
#define HW_RT_TASK_PRIO (configMAX_PRIORITIES - 1)

/** @brief Hardware Real-Time Task mapped core ID. */
#define HW_RT_TASK_CORE 0

/** @brief Defines the watchdogs lock timeout in nanoseconds. */
#define WD_LOCK_TIMEOUT_NS 20000000ULL

/** @brief Defines the watchdogs lock timeout in ticks. */
#define WD_LOCK_TIMEOUT_TICKS (pdMS_TO_TICKS(WD_LOCK_TIMEOUT_NS / 1000000ULL))

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
/**
 * @brief Real-time task timer handler.
 *
 * @details Real-time task timer handler. This IRQ handler will release the real
 * time task for execution.
 */
static void IRAM_ATTR RealTimeTaskTimerHandler(void);

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/
/** @brief Static handle for the ISR */
static TaskHandle_t sRTTaskHandle;

/** @brief Singleton instance of the health monitor. */
HealthMonitor* HealthMonitor::_SPINSTANCE = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
static void IRAM_ATTR RealTimeTaskTimerHandler(void) {
    BaseType_t result;
    BaseType_t needSchedule;

    /* Notify the real-time task */
    needSchedule = pdFALSE;
    result = xTaskNotifyFromISR(
        sRTTaskHandle,
        0,
        eNoAction,
        &needSchedule
    );

    if (pdPASS != result) {
        LOG_ERROR("Failed to notify the Real-Time Task.\n");
        HWManager::Reboot();
    }

    /* Check if scheduling is needed */
    portYIELD_FROM_ISR(needSchedule);
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/

HealthMonitor* HealthMonitor::GetInstance(void) noexcept {
    if (nullptr == HealthMonitor::_SPINSTANCE) {
        HealthMonitor::_SPINSTANCE = new HealthMonitor();

        if (nullptr == HealthMonitor::_SPINSTANCE) {
            LOG_ERROR("Failed to insanciate the Health Monitor.\n");
            HWManager::Reboot();
        }
    }

    return HealthMonitor::_SPINSTANCE;
}

E_Return HealthMonitor::AddWatchdog(Timeout* pTimeout, uint32_t& rId) noexcept {
    E_Return error;

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
                    error = E_Return::ERR_UNKNOWN;
                }
            }
            else {
                error = E_Return::ERR_MEMORY;
            }

            if (pdPASS != xSemaphoreGive(this->_wdLock)) {
                /* TODO: Error Health Monitor */
            }
        }
        else {
            LOG_ERROR("Failed to acquire watchdogs lock.\n");
            error = E_Return::ERR_HM_TIMEOUT;
        }
    }
    else {
        error = E_Return::ERR_INVALID_PARAM;
    }


    return error;
}

E_Return HealthMonitor::RemoveWatchdog(const uint32_t kId) noexcept {
    E_Return                                         error;
    std::unordered_map<uint32_t, Timeout*>::iterator it;

    if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
        it = this->_watchdogs.find(kId);
        if (this->_watchdogs.end() != it) {
            try {
                this->_watchdogs.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_wdLock)) {
            /* TODO: Error Health Monitor */
        }
    }
    else {
        LOG_ERROR("Failed to acquire watchdogs lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

void HealthMonitor::SetSystemState(const E_SystemState kState) noexcept {
    this->_systemState = kState;
}

void HealthMonitor::Init(void) noexcept {
    /* Initialize the real-time task */
    RealTimeTaskInit();
}

HealthMonitor::HealthMonitor(void) noexcept {
    this->_lastWDId = 0;
    this->_systemState = E_SystemState::STARTING;

    this->_wdLock = xSemaphoreCreateMutex();
    if (nullptr == this->_wdLock) {
        LOG_ERROR("Failed to initialize Health Monitor Watchdogs lock.\n");
        HWManager::Reboot();
    }
}

void HealthMonitor::RealTimeTaskRoutine(void* pHealthMonitor) noexcept {
    BaseType_t                                       result;
    HealthMonitor*                                   pHM;
    uint64_t                                         currentTime;
    std::unordered_map<uint32_t, Timeout*>::iterator it;
    Timeout                                          hmDeadlineTimeout(
        HW_RT_TASK_DEADLINE_MISS_DELAY
    );
    pHM = (HealthMonitor*)pHealthMonitor;
    while (true) {
        /* Wait for trigger signal */
        result = xTaskNotifyWait(0xFFFFFFFFUL, 0x0UL, 0x0UL, portMAX_DELAY);

        /* Check for event error*/
        if (pdPASS != result) {
            LOG_ERROR("HW Real-Time Task event error.\n");
            HWManager::Reboot();
        }

        /* Check for deadline miss */
        if (E_SystemState::EXECUTING == pHM->_systemState &&
            hmDeadlineTimeout.Check()) {
            LOG_ERROR(
                "HW Real-Time Task deadline miss (expeected %llu).\n",
                hmDeadlineTimeout.GetNextTimeEvent()
            );
            HWManager::Reboot();
        }
        /* Tick the timeout */
        hmDeadlineTimeout.Tick();

        /* Check for watchdogs */
        currentTime = HWManager::GetTime();
        if (pdPASS == xSemaphoreTake(pHM->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
            for (it = pHM->_watchdogs.begin();
                 pHM->_watchdogs.end() != it;
                 ++it) {
                /* Check for dealine miss */
                if (it->second->GetNextWatchdogEvent() < currentTime) {
                    it->second->ExecuteHandler();
                }
            }

            if (pdPASS != xSemaphoreGive(pHM->_wdLock)) {
                /* TODO: Error Health Monitor */
            }
        }
        else {
            LOG_ERROR("Failed to acquire watchdogs lock.\n");
            HWManager::Reboot();
        }
    }
}

void HealthMonitor::RealTimeTaskInit(void) noexcept {
    BaseType_t result;

    /* Start the real time task timer */
    this->_RTTaskTimer = timerBegin(
        HW_RT_TASK_TIMER,
        HW_RT_TASK_TIMER_PRESCALER,
        true
    );
    if (nullptr == this->_RTTaskTimer) {
        LOG_ERROR("Failed to initialize the Real-Time Timer.\n");
        HWManager::Reboot();
    }
    timerAttachInterrupt(this->_RTTaskTimer, RealTimeTaskTimerHandler, true);
    timerAlarmWrite(this->_RTTaskTimer, HW_RT_TASK_TIMER_DEC_INT, true);
    timerAlarmEnable(this->_RTTaskTimer);


    /* Create the real-time high-priority task */
    result = xTaskCreatePinnedToCore(
        RealTimeTaskRoutine,
        HW_RT_TASK_NAME,
        HW_RT_TASK_STACK,
        this,
        HW_RT_TASK_PRIO,
        &this->_RTTaskHandle,
        HW_RT_TASK_CORE
    );
    if (pdPASS != result) {
        LOG_ERROR("Failed to initialize the Real-Time Task.\n");
        HWManager::Reboot();
    }

    sRTTaskHandle = this->_RTTaskHandle;
}
