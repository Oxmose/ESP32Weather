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
#include <unordered_map>     /* Standard unordered map */
#include <esp32-hal-timer.h> /* ESP32 Timers */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/* None */

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
         * @param[out] pId The unique identifier that will be associated to the
         * watchdog.
         *
         * @return The function returns the success or error status.
         */
        E_Return AddWatchdog(Timeout* pTimeout, uint32_t& pId) noexcept;

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
         * @brief Initializes the hardware real-time task.
         *
         * @details  Initializes the hardware real-time task. This starts the
         * task timer and the task thread. On error, a reboot is issued.
         */
        void RealTimeTaskInit(void) noexcept;

        /** @brief Real-time task timer. */
        hw_timer_t* _RTTaskTimer;
        /** @brief Real-time task handle. */
        TaskHandle_t _RTTaskHandle;
        /** @brief Map that contains the watchdogs */
        std::unordered_map<uint32_t, Timeout*> _watchdogs;
        /** @brief Last watchdog ID provided. */
        uint32_t _lastWDId;
        /** @brief Stores the watchdogs mutex. */
        SemaphoreHandle_t _wdLock;
        /** @brief Stores the current system state. */
        E_SystemState _systemState;

        /** @brief Health Monitor Singleton instance. */
        static HealthMonitor* _SPINSTANCE;
};


#endif /* #ifndef __HEALTH_MONITOR_H__ */