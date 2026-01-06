/*******************************************************************************
 * @file IOTask.cpp
 *
 * @see IOTask.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 06/01/2026
 *
 * @version 1.0
 *
 * @brief IO Task class.
 *
 * @details IO Task class. Provides the functionalities to create and execute
 * the IO task responsible of inputs and outputs management.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* Hardware layer services */
#include <Logger.h>          /* Logger service */
#include <Timeout.h>         /* Timeout manager */
#include <SystemState.h>     /* System State services. */
#include <IOLedManager.h>    /* IO Led manager */
#include <HealthMonitor.h>   /* HM Services */
#include <IOButtonManager.h> /* IO Button manager */

/* Header file */
#include <IOTask.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Main loop period in nanoseconds. */
#define HW_IO_TASK_PERIOD_NS 25000000ULL
/** @brief Main loop period tolerance in nanoseconds. */
#define HW_IO_TASK_PERIOD_TOLERANCE_NS 1250000ULL
/** @brief Main loop watchdog timeout in nanoseconds. */
#define HW_IO_TASK_WD_TIMEOUT_NS (2 * HW_IO_TASK_PERIOD_NS)
/** @brief Hardware Real-Time Task name. */
#define HW_IO_TASK_NAME "HW-IO_TASK"
/** @brief Hardware Real-Time Task stack size in bytes. */
#define HW_IO_TASK_STACK 4096
/** @brief Hardware Real-Time Task priority. */
#define HW_IO_TASK_PRIO (configMAX_PRIORITIES - 1)
/** @brief Hardware Real-Time Task mapped core ID. */
#define HW_IO_TASK_CORE 0

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
/** @brief Stores the current IO task instance for external routines. */
static IOTask* spIOTask = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
IOTask::IOTask(void) noexcept {
    BaseType_t result;

    /* Check instance */
    if (nullptr != spIOTask) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_CREATE, 0);
    }

    /* Init system state */
    this->_pSysState = SystemState::GetInstance();

    /* Create the timeout */
    this->_pTimeout = new Timeout(
        HW_IO_TASK_PERIOD_NS + HW_IO_TASK_PERIOD_TOLERANCE_NS,
        HW_IO_TASK_WD_TIMEOUT_NS,
        IOTask::DeadlineMissHandler
    );
    if (nullptr == this->_pTimeout) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_CREATE, 2);
    }

    /* Create the task */
    result = xTaskCreatePinnedToCore(
        IOTask::IOTaskRoutine,
        HW_IO_TASK_NAME,
        HW_IO_TASK_STACK,
        this,
        HW_IO_TASK_PRIO,
        &this->_IOTaskHandle,
        HW_IO_TASK_CORE
    );
    if (pdPASS != result) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_CREATE, 3);
    }

    /* Set instance and set as started */
    spIOTask = this;
}

void IOTask::DeadlineMissHandler(void) noexcept {
    HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_DEADLINE_MISS, 2);
}

void IOTask::IOTaskRoutine(void* pParam) noexcept {
    IOLedManager*    pLed;
    IOButtonManager* pBtn;
    IOTask*          pIOTask;
    BaseType_t       result;
    TickType_t       lastWakeTime;

    /* Get the task */
    pIOTask = (IOTask*)pParam;

    /* First tick */
    pIOTask->_pTimeout->Tick();
    lastWakeTime = xTaskGetTickCount();

    while (true) {
        /* Manage deadline miss */
        if (pIOTask->_pTimeout->Check()) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_DEADLINE_MISS, 0);
        }
        pIOTask->_pTimeout->Tick();

        /* Get the instances */
        pLed = pIOTask->_pSysState->GetIOLedManager();
        pBtn = pIOTask->_pSysState->GetIOButtonManager();

        /* Update the IO buttons */
        pBtn->Update();

        /* Update the LED */
        pLed->Update();

        /* Wait for period */
        result = xTaskDelayUntil(
            &lastWakeTime,
            HW_IO_TASK_PERIOD_NS / 1000000 / portTICK_PERIOD_MS
        );
        if (pdPASS != result) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_IO_TASK_DEADLINE_MISS, 1);
        }
    }
}