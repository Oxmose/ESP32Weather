/*******************************************************************************
 * @file IOLedManager.cpp
 *
 * @see IOLedManager.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <BSP.h>               /* BSP definitions */
#include <cstdint>             /* Generic Types */
#include <Errors.h>            /* Errors definitions */

/* Header File */
#include <IOLedManager.h>

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
 * CLASS METHODS
 ******************************************************************************/

IOLedManager::IOLedManager(void) noexcept {
    /* Init states. */
    memset(this->_pLedStates, 0, sizeof(S_LedState) * E_LedID::LED_MAX_ID);

    /* Init the GPIOs */
    this->_pLedDev[E_LedID::LED_INFO].pin = E_GPIORouting::GPIO_LED_INFO;
    this->_pLedDev[E_LedID::LED_INFO].isRgb = true;
    pinMode(this->_pLedDev[E_LedID::LED_INFO].pin, OUTPUT);
    digitalWrite(this->_pLedDev[E_LedID::LED_INFO].pin, LOW);
}

void IOLedManager::Update(void) noexcept {
    uint8_t i;

    /* Iterate over all leds */
    for (i = 0; i < E_LedID::LED_MAX_ID; ++i) {
        /* Check if enabled */
        if (this->_pLedStates[i].enabled) {
            /* Check the blink period */
            if (0 != this->_pLedStates[i].blinkPeriodNs) {
                if (HWManager::GetTime() < this->_pLedStates[i].blinkPeriodNs) {
                    /* Update time */
                    this->_pLedStates[i].blinkPeriodNs +=
                        this->_pLedStates[i].blinkPeriodNs;

                    /* Update state */
                    this->_pLedStates[i].isOn = !this->_pLedStates[i].isOn;
                }
            }

            /* Set state */
            if (this->_pLedDev[i].isRgb) {
                neopixelWrite(
                    this->_pLedDev[i].pin,
                    this->_pLedStates[i].red * this->_pLedStates[i].isOn,
                    this->_pLedStates[i].green * this->_pLedStates[i].isOn,
                    this->_pLedStates[i].blue * this->_pLedStates[i].isOn
                );
            }
            else {
                digitalWrite(
                    this->_pLedDev[i].pin,
                    this->_pLedStates[i].isOn
                );
            }
        }
    }
}

void IOLedManager::Enable(const E_LedID kLedId, const bool kEnable) noexcept {
    if (E_LedID::LED_MAX_ID < kLedId) {
        this->_pLedStates[kLedId].enabled = kEnable;
    }
}

void IOLedManager::SetState(const E_LedID kLedId,
                             const S_LedState& krState) noexcept {
    if (E_LedID::LED_MAX_ID < kLedId) {
        this->_pLedStates[kLedId] = krState;
    }
}