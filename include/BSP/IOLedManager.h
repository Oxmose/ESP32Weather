/*******************************************************************************
 * @file IOLedManager.h
 *
 * @see IOLedManager.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 04/01/2026
 *
 * @version 1.0
 *
 * @brief This file contains the IO LED manager.
 *
 * @details This file contains the IO LED manager. The file provides the
 * services to manage the leds available on the station.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __BSP_IOLED_MANAGER_H_
#define __BSP_IOLED_MANAGER_H_

/****************************** OUTER NAMESPACE *******************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <BSP.h>               /* BSP definitions */
#include <cstdint>             /* Generic Types */
#include <Errors.h>            /* Errors definitions */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/****************************** INNER NAMESPACE *******************************/

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/

/** @brief Defines the LED functionalities. */
typedef enum {
    /** @brief Information LED */
    LED_INFO,
    /** @brief Max ID */
    LED_MAX_ID
} E_LedID;

/** @brief Defines the LED state. */
typedef struct {
    /** @brief Defines the power state. */
    bool enabled;

    /** @brief Defines the red color for RGB leds. */
    uint8_t red;
    /** @brief Defines the green color for RGB leds. */
    uint8_t green;
    /** @brief Defines the blue color for RGB leds. */
    uint8_t blue;

    /** @brief Defines the blink period in nanoseconds (0 for no blink). */
    uint64_t blinkPeriodNs;

    /** @brief Defines the current state. */
    bool isOn;
} S_LedState;

/** @brief Defines the led device. */
typedef struct {
    /** @brief Pins definition. */
    E_GPIORouting pin;
    /** @brief Tells if the LED is RGB. */
    bool isRgb;
} S_LedDevice;

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
 * @brief LED manager class.
 *
 * @details LED manager class. This class provides the services to manage the
 * available LEDs.
 */
class IOLedManager {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Constructs a new IOLedManager object.
         *
         * @details Constructs a new IOLedManager object.
         * The constructor initializes the LEDs and their GPIOs.
         */
        IOLedManager(void) noexcept;

        /**
         * @brief Updates the LED states.
         *
         * @details Updates the LED states. This function also calculate the
         * next LED state when changing states are set for the LEDs.
         */
        void Update(void) noexcept;

        /**
         * @brief Updates the LEDs power state.
         *
         * @details Updates the LEDs power state. This enables or disables the
         * led based on the parameters. Using this function with an invalid
         * led identifer has no efect.
         *
         * @param[in] kLedId The identifier of the led to enable or disable.
         * @param[in] kEnable Tells if the LED should be enabled (true) or
         * disabled (false).
         */
        void Enable(const E_LedID kLedId, const bool kEnable) noexcept;

        /**
         * @brief Sets the led state.
         *
         * @details Sets the led state. This will directly affect the LED state
         * after the call. Using this function with an invalid led identifer has
         * no efect.
         *
         * @param[in] kLedId The identifier of the led to set the state of.
         * @param[in] krState The state to set.
         */
        void SetState(const E_LedID kLedId, const S_LedState& krState) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief Stores the LED GPIO pins */
        S_LedDevice _pLedDev[E_LedID::LED_MAX_ID];
        /** @brief Stores the LEDs state */
        S_LedState _pLedStates[E_LedID::LED_MAX_ID];
        /** @brief Stores the next periods */
        uint64_t _pNextPeriods[E_LedID::LED_MAX_ID];

};

#endif /* #ifndef __BSP_IOLED_MANAGER_H_ */