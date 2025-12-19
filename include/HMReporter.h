/*******************************************************************************
 * @file HMReporter.h
 *
 * @see HMReporter.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 16/12/2025
 *
 * @version 1.0
 *
 * @brief Health Monitor Reporter abstract.
 *
 * @details Health Monitor Reporter abstract. Defines what the health monitor
 * reporters should implement to be used as a periodic health status checker.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __HM_REPORTER_H__
#define __HM_REPORTER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>  /* Standard string */
#include <cstdint> /* Standard types */

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
/** @brief Defines the possible health states. */
typedef enum {
    /** @brief Healthy */
    HM_HEALTHY,
    /** @brief Degraded */
    HM_DEGRADED,
    /** @brief Unhealthy */
    HM_UNHEALTHY,
    /** @brief Disabled */
    HM_DISABLED
} E_HMStatus;

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
 * @brief Health Monitor Reporter class.
 *
 * @details Health Monitor Reporter class. Defines what the HM reporters
 * should implement.
 */
class HMReporter
{
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
        HMReporter(const uint64_t     kCheckPeriodNs,
                   const uint32_t     kFailToDegrade,
                   const uint32_t     kFailToUnhealthy,
                   const std::string& krName) noexcept;
        /**
         * @brief HMReporter Interface Destructor.
         *
         * @details HMReporter Interface Destructor. Release the used memory.
         */
        virtual ~HMReporter(void) noexcept;

        /**
         * @brief Performs a health check.
         *
         * @details Performs a health check. This function is called by the
         * health monitor at each HM period.
         *
         * @param[in] kTime The current time for the check.
         */
        void HealthCheck(const uint64_t kTime) noexcept;

        /**
         * @brief Execute the current action.
         *
         * @details Execute the current action. If no action is defined, this
         * function has no effect.
         */
        void ExecuteAction(void) noexcept;

        /**
         * @brief Returns the current health status of the monitored item.
         *
         * @details Returns the current health status of the monitored item.
         *
         * @return Returns the current health status of the monitored item.
         */
        E_HMStatus GetStatus(void) const noexcept;

        /**
         * @brief Returns the current number of failures of the monitored item.
         *
         * @details Returns the current number of failures of the monitored
         * item.
         *
         * @return Returns the current number of failures of the monitored item.
         */
        uint32_t GetFailureCount(void) const noexcept;

        /**
         * @brief Returns the name of the monitored item.
         *
         * @details Returns the name of the monitored item.
         *
         * @return Returns the name of the monitored item.
         */
        const std::string& GetName(void) const noexcept;


    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /**
         * @brief Action executed on degraded health.
         *
         * @details Action executed on degraded health. Must be implemented by
         * the inherited class.
         */
        virtual void OnDegraded(void) noexcept = 0;
        /**
         * @brief Action executed on unhealthy health.
         *
         * @details Action executed on unhealthy health. Must be implemented by
         * the inherited class.
         */
        virtual void OnUnhealthy(void) noexcept = 0;
        /**
         * @brief Checks the current health for the report.
         *
         * @details  Checks the current health for the report. Must be
         * implemented by the inherited class.
         *
         * @return The function returns the check status: passed is True,
         * otherwise False.
         */
        virtual bool PerformCheck(void) noexcept = 0;

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief The time in nanoseconds at which the next check shall be
         * performed.
         */
        uint64_t _nextCheckNs;
        /** @brief The period in nanoseconds between two checks. */
        uint64_t _checkPeriodNs;
        /**
         * @brief The number of failures before switching health to degraded.
         */
        uint32_t _failBeforeDegraded;
        /**
         * @brief The number of failures before switching health to unhealthy.
         */
        uint32_t _failBeforeUnhealthy;
        /**
         * @brief The number of failures since the last healthy state.
         */
        uint32_t _failCount;
        /** @brief The name of the reporter. */
        std::string _name;
        /** @brief The current health status. */
        E_HMStatus _status;
        /** @brief Tells if an action is running to manage the status. */
        bool _hasRunningAction;
};

#endif /* #ifndef __HM_REPORTER_H__ */