/*******************************************************************************
 * @file NTPManager.h
 *
 * @see NTPManager.cpp
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

#ifndef __CORE_NTP_MANAGER_H__
#define __CORE_NTP_MANAGER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <cstdint>      /* Standard Integer Definitions */
#include <WiFiUdp.h>    /* WiFi UDP manager */
#include <Timeout.h>    /* Timeout manager */
#include <NTPClient.h>  /* NTP client */
#include <HMReporter.h> /* HM Reporter interface */

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
/* Forward declaration */
class NTPManagerHealthReporter;

/**
 * @brief NTP client manager class.
 *
 * @details NTP client manager class. Provides the time through an NTP client
 * and maintains time between refreshes.
 */
class NTPManager {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief The NTP manager constructor.
         *
         * @details The NTP manager constructor. Allocates and initializes the
         * reset resources.
         */
        NTPManager(void) noexcept;

        /**
         * @brief Destroys a NTPManager.
         *
         * @details Destroys a NTPManager. Since only one object is allowed
         * in the firmware, the destructor will generate a critical error.
         */
        virtual ~NTPManager(void) noexcept;

        /**
         * @brief Updates the NTP states.
         *
         * @details Updates the NTP states. This function also is in charge of
         * retrieving the NTP time when needed.
         */
        void Update(void) noexcept;

        /**
         * @brief Returns the current day of the week starting by Sunday.
         *
         * @details Returns the current day of the week starting by Sunday.
         *
         * @return Returns the current day of the week starting by Sunday.
         */
        uint8_t GetDay(void) const noexcept;

        /**
         * @brief Returns the current hour of the day.
         *
         * @details Returns the current hour of the day.
         *
         * @return Returns the current hour of the day.
         */
        uint8_t GetHours(void) const noexcept;

        /**
         * @brief Returns the current minute of the hour.
         *
         * @details Returns the current minute of the hour.
         *
         * @return Returns the current minute of the hour.
         */
        uint8_t GetMinutes(void) const noexcept;

        /**
         * @brief Returns the current second of the minute.
         *
         * @details Returns the current second of the minute.
         *
         * @return Returns the current second of the minute.
         */
        uint8_t GetSeconds(void) const noexcept;

        /**
         * @brief Returns the current timestamp in epoch time starting Jan. 1,
         * 1970.
         *
         * @details Returns the current timestamp in epoch time starting Jan. 1,
         * 1970.
         *
         * @return Returns the current timestamp in epoch time starting Jan. 1,
         * 1970.
         */
        uint64_t GetTimestamp(void) const noexcept;

        /**
         * @brief Updates the time offset.
         *
         * @details Updates the time offset. This is to accomodate timezone
         * update.
         */
        void SetTimeOffset(const int32_t kOffset) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief NTP manager update task routine.
         *
         * @details NTP manager update task routine. The routine periodically
         * runs to update the NTP server. The period can be changed based on the
         * current NTP manager health.
         *
         * @param[in] pInstance The NTP manager instance to update.
         */
        static void NTPTaskRoutine(void* pInstance) noexcept;

        /**
         * @brief Handles the NTP task watchdog trigger.
         *
         * @details Handles the NTP task watchdog trigger. Health
         * monitor is used to correct the error.
         */
        static void DeadlineMissHandler(void) noexcept;

        /** @brief Stores the NTP client instance. */
        NTPClient* _pClient;
        /** @brief The NTP UDP manager. */
        WiFiUDP _ntpUDP;
        /** @brief Stores the number of update failures. */
        uint32_t _updateFailed;
        /** @brief Stores the health reporter. */
        NTPManagerHealthReporter* _pReporter;
        /** @brief Stores the refresh period in nanoseconds. */
        volatile uint64_t _periodNs;
        /** @brief Task deadline miss manager. */
        Timeout* _pTimeout;
        /** @brief Update task handle. */
        TaskHandle_t _taskHandle;

    /********************* FRIEND DECLARATIONS *********************/
    friend NTPManagerHealthReporter;
};


/**
 * @brief NTPManager Health Reporter class.
 *
 * @details NTPManager Health Reporter class. This class is used to check the
 * NTPManager continuous health and report the health status of the NTPManager.
 */
class NTPManagerHealthReporter : public HMReporter {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Initializes the Health Reporter.
         *
         * @details Initializes the Health Reporter. The next check period
         * will be started after the object initialization.
         *
         * @param[in] krParam The health reporter parameters.
         * @param[in] pModule The WiFiModule being monitored.
         *
         */
        NTPManagerHealthReporter(const S_HMReporterParam& krParam,
                                 NTPManager*              pModule) noexcept;

        /**
         * @brief NTPManagerHealthReporter Interface Destructor.
         *
         * @details NTPManagerHealthReporter Interface Destructor. Release the
         * used memory.
         */
        virtual ~NTPManagerHealthReporter(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /**
         * @brief Action executed on degraded health.
         *
         * @details Action executed on degraded health. Must be implemented by
         * the inherited class.
         */
        virtual void OnDegraded(void) noexcept;

        /**
         * @brief Action executed on unhealthy health.
         *
         * @details Action executed on unhealthy health. Must be implemented by
         * the inherited class.
         */
        virtual void OnUnhealthy(void) noexcept;

        /**
         * @brief Checks the current health for the report.
         *
         * @details  Checks the current health for the report. Must be
         * implemented by the inherited class.
         *
         * @return The function returns the check status: passed is True,
         * otherwise False.
         */
        virtual bool PerformCheck(void) noexcept;

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief The module monitored by the reporter. */
        NTPManager* _pModule;
};

#endif /* #ifndef __CORE_NTP_MANAGER_H__ */