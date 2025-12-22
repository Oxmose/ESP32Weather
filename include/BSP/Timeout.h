/*******************************************************************************
 * @file Timeout.h
 *
 * @see Timeout.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the timeout features.
 *
 * @details This file defines the timeout features. The timeout and watchdog
 * functionalities are provided by the defined class.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <cstdint> /* Standard int types */

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
/**
 * @brief Timeout management class.
 *
 * @details Timeout management class. This class provides support to detect
 * timeout and apply eternal actions when the timeout check is not reached
 * (watchdog).
 */
class Timeout
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Timeout constructor.
         *
         * @details Timeout constructor. Creates a new timeout manager with
         * the given timeout in nanoseconds and an optional watchdog action.
         * If the action is NULL, the watchdog is disabled.
         *
         * @param[in] kTimeoutNs The timeout in nanoseconds.
         * @param[in] kWatchdogNs The optional watchdog timeout in nanoseconds.
         * @param[in] pHandler The handler action of the watchdog action.
         */
        Timeout(const uint64_t kTimeoutNs,
                const uint64_t kWatchdogNs = 0,
                void           (*pHandler)(void) = nullptr) noexcept;

        /**
         * @brief Timeout destuctor.
         *
         * @details Timeout destuctor. Release the watchdog action if existing.
         */
        ~Timeout(void) noexcept;

        /**
         * @brief Notifies the timeout and watchdog of a tick.
         *
         * @details Notifies the timeout and watchdog of a tick. This reset
         * their next trigger time.
         */
        void Tick(void) noexcept;

        /**
         * @brief Checks if the timeout was reached.
         *
         * @details Checks if the timeout was reached. If the timeout was
         * reached, true is returned, otherwise false.s
         *
         * @return Returns true if the timeout occured, false otherwise.
         */
        bool Check(void) const noexcept;

        /**
         * @brief Returns the time in nanoseconds when the timeout will be
         * reached.
         *
         * @details Returns the time in nanoseconds when the timeout will be
         * reached.
         *
         * @return Returns the time in nanoseconds when the timeout will be
         * reached.
         */
        uint64_t GetNextTimeEvent(void) const noexcept;

        /**
         * @brief Returns the time in nanoseconds when the timeout will be
         * triggered.
         *
         * @details Returns the time in nanoseconds when the watchdog will be
         * triggered.
         *
         * @return Returns the time in nanoseconds when the timeout will be
         * triggered.
         */
        uint64_t GetNextWatchdogEvent(void) const noexcept;

        /**
         * @brief Executes the watchdog handler.
         *
         * @details Executes the watchdog handler. This is called when the
         * watchdog triggers after reaching its deadline. If the handler is
         * not set, this function has no action.
         */
        void ExecuteHandler(void) const noexcept;


    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief Initial timout delay. */
        uint64_t _timeout;
        /** @brief Initial watchdog delay. */
        uint64_t _wdTimeout;
        /** @brief Stores the next timeout event. */
        uint64_t _nextTimeEvent;
        /** @brief Stores the next watchdog event. */
        uint64_t _nextWatchdogEvent;
        /** @brief Stores the watchdog Id when used. */
        uint32_t _watchdogId;
        /** @brief Stores the watchdog trigger handler. */
        void (*_pWDHandler) (void);
};

#endif /* #ifndef __TIMEOUT_H__ */