/*******************************************************************************
 * @file HealthMonitor.h
 *
 * @see HealthMonitor.cpp
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

#ifndef __HEALTH_MONITOR_H__
#define __HEALTH_MONITOR_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <Errors.h>          /* Errors definitions */
#include <Arduino.h>         /* Arduino framework */
#include <Timeout.h>         /* Timeout services */
#include <HMReporter.h>      /* HM Reporters */
#include <unordered_map>     /* Standard unordered map */
#include <esp32-hal-timer.h> /* ESP32 Timers */
#include <HMConfiguration.h> /* Health Monitor Configuration */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/**
 * @brief Defines the real-time task period in nanoseconds.
 */
#define HW_RT_TASK_PERIOD_NS 100000000ULL

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/** @brief Defines the system states. */
typedef enum {
    /** @brief System is starting */
    STARTING,
    /** @brief System is executing */
    EXECUTING,
    /** @brief System is faulted */
    FAULTED
} E_SystemState;

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
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASSES
 ******************************************************************************/

/**
 * @brief Health Monitor class.
 *
 * @details Health Monitor class. This class provides the services
 * to monitor and handle system's health issues. It also provides watchdogs
 * features.
 */
class HealthMonitor
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Get the instance of the Health Monitor object.
         *
         * @details Get the instance of the Health monotor object.
         *
         * @return The instance of the Health monotor object is returned.
         * nullptr is returned on error.
         */
        static HealthMonitor* GetInstance(void) noexcept;

        /**
         * @brief Initializes the health monitor.
         *
         * @details Initializes the health monitor. This function setups the
         * health monitor and its related tasks.
         */
        void Init(void) noexcept;

        /**
         * @brief Adds a watchdog to the watchdogs check list.
         *
         * @details Adds a watchdog to the watchdogs check list. The watchdog
         * will start being monitored after this call.
         *
         * @param[in] pTimeout The Timeout that contains the watchdog to add.
         * @param[out] rId The unique identifier that will be associated to the
         * watchdog.
         *
         * @return The function returns the success or error status.
         */
        E_Return AddWatchdog(Timeout* pTimeout, uint32_t& rId) noexcept;

        /**
         * @brief Removes a watchdog from the watchdogs check list.
         *
         * @details Removes a watchdog from the watchdogs check list. If the
         * watchfdog does not exist, no action is taken.
         *
         * @param[in] kId The unique identifier of the watchdog to remove.
         *
         * @return The function returns the success or error status.
         */
        E_Return RemoveWatchdog(const uint32_t kId) noexcept;

        /**
         * @brief Sets the HM system state.
         *
         * @details Sets the HM system state. Based on the system state, the HM
         * will provide different services.
         *
         * @param[in] kState The state to set.
         */
        void SetSystemState(const E_SystemState kState) noexcept;

        /**
         * @brief Adds a HM reporter.
         *
         * @details Adds a HM repoter. The reporter will be added to the
         * reporters list and checked when needed.
         *
         * @param[in] pReporter The reporter to add.
         * @param[out] rId The unique identifier that will be associated to the
         * reporter.
         *
         * @return The function returns the success or error status.
         */
        E_Return AddReporter(HMReporter* pReporter, uint32_t& rId) noexcept;

        /**
         * @brief Removes a HM reporter.
         *
         * @details Removes a HM repoter. The reporter will be removed to the
         * reporters list and will no longer be checked.
         *
         * @param[in] pReporter The id of the reporter to remove.
         *
         * @return The function returns the success or error status.
         */
        E_Return RemoveReporter(const uint32_t kId) noexcept;

        /**
         * @brief Adds an HM action to execute.
         *
         * @details Adds an HM action to execute. The action is provided by the
         * HM Reporter and is executed in a concurent thread.
         *
         * @param[in] pReporter The reporter embedding the action to execute.
         *
         * @return The function returns the success or error status.
         */
        E_Return AddHMAction(HMReporter* pReporter) noexcept;

        /**
         * @brief Reports an event to the Health Monitor.
         *
         * @details Reports an event to the Health Monitor. Based on the
         * configuration of the HM, the event will be handled and actions
         * will be taken.
         *
         * @param[in] kEvent The event to report.
         * @param[in] pParam An optional parameter for the eventhandler. Can
         * be NULL.
         */
        void ReportHM(const E_HMEvent kEvent,
                      void*           pParam = nullptr) const noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Health Monitor constructor.
         *
         * @details Health Monitor constructor. Allocates the resources prior to
         * initialization.
         */
        HealthMonitor(void) noexcept;

        /**
         * @brief Hardware Real time task.
         *
         * @details Hardware Real time task. Executes periodically at high
         * priority and frequency. This task monitors health related features.
         *
         * @param[in] pHealthMonitor The health monitor instance to be used by
         * the task.
         */
        static void RealTimeTaskRoutine(void* pHealthMonitor) noexcept;

        /**
         * @brief HM Actions task.
         *
         * @details HM Actions task. Executes the HM actions in a queued
         * fashion.
         *
         * @param[in] pHealthMonitor The health monitor instance to be used by
         * the task.
         */
        static void HMActionTaskRoutine(void* pHealthMonitor) noexcept;

        /**
         * @brief Checks each watchdogs registered.
         *
         * @details Checks each watchdogs registered. If a watchdog is trigerred
         * the handling function is called.
         */
        void CheckWatchdogs(void) const noexcept;

        /**
         * @brief Checks each health reporter registered.
         *
         * @details Checks each health reporter registered. This will update
         * their check time and status in case of failure.
         */
        void CheckReporters(void) const noexcept;

        /**
         * @brief Initializes the hardware real-time task.
         *
         * @details  Initializes the hardware real-time task. This starts the
         * task timer and the task thread. On error, a reboot is issued.
         */
        void RealTimeTaskInit(void) noexcept;

        /**
         * @brief Initializes the actions task.
         *
         * @details  Initializes the actions task. On error, a reboot is issued.
         */
        void ActionsTaskInit(void) noexcept;

        /** @brief Real-time task timer. */
        hw_timer_t* _RTTaskTimer;
        /** @brief Real-time task handle. */
        TaskHandle_t _RTTaskHandle;
        /** @brief Actions task handle. */
        TaskHandle_t _actionsTaskHandle;
        /** @brief Map that contains the watchdogs */
        std::unordered_map<uint32_t, Timeout*> _watchdogs;
        /** @brief Map that contains the HM reporters */
        std::unordered_map<uint32_t, HMReporter*> _reporters;
        /** @brief Last watchdog ID provided. */
        uint32_t _lastWDId;
        /** @brief Last reporter ID provided. */
        uint32_t _lastReporterId;
        /** @brief Stores the watchdogs mutex. */
        SemaphoreHandle_t _wdLock;
        /** @brief Stores the reporters mutex. */
        SemaphoreHandle_t _reportersLock;
        /** @brief Stores the current system state. */
        E_SystemState _systemState;
        /** @brief The HM actions queue */
        QueueHandle_t _actionsQueue;


        /** @brief Health Monitor Singleton instance. */
        static HealthMonitor* _SPINSTANCE;
};


#endif /* #ifndef __HEALTH_MONITOR_H__ */