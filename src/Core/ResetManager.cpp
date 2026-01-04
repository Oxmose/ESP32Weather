/*******************************************************************************
 * @file ResetManager.cpp
 *
 * @see ResetManager.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 03/01/2026
 *
 * @version 1.0
 *
 * @brief Reset manager class.
 *
 * @details Reset manager class. Provides the functionalities to handle the
 * firmware reset and factory reset.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* Hardware layer */
#include <cstdint>           /* Standard Integer Definitions */
#include <Logger.h>          /* Logging services */
#include <SystemState.h>     /* System state manager */
#include <IOLedManager.h>    /* LED Manager */
#include <IOButtonManager.h> /* IO Button Action interface */

/* Header file */
#include <ResetManager.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/**
 * @brief Time during which the reset button should be pressed before entering
 * the reset state.
 */
#define RESET_PERFORM_WAIT_NS 5000000000ULL
/**
 * @brief Time during which the reset button should be pressed again to perform
 * a reset.
 */
#define RESET_PERFORM_TIMEOUT_NS 3000000000ULL

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
/** @brief Waiting led state information. */
static const S_LedState sWaitLedState = {
    .enabled = true,
    .red = 0,
    .green = 255,
    .blue = 255,
    .blinkPeriodNs = 1000000000,
    .isOn = true
};
/** @brief Waiting up led state information. */
static const S_LedState sWaitUpLedState = {
    .enabled = true,
    .red = 255,
    .green = 0,
    .blue = 0,
    .blinkPeriodNs = 0,
    .isOn = true
};
/** @brief Waiting down led state information. */
static const S_LedState sWaitDownLedState = {
    .enabled = true,
    .red = 0,
    .green = 255,
    .blue = 0,
    .blinkPeriodNs = 500000000,
    .isOn = true
};

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
ResetManager::ResetManager(void) noexcept {
    this->_state = E_ResetState::RESET_NONE;
    this->_resetStartTime = 0;
}

ResetManager::~ResetManager(void) noexcept {

}

void ResetManager::Execute(
    const uint64_t kpBtnLastPress[E_ButtonID::BUTTON_MAX_ID],
    const E_ButtonState kpBtnStates[E_ButtonID::BUTTON_MAX_ID])
    noexcept {

    IOLedManager* pLed;

    /* Get the IO led manager */
    pLed = SystemState::GetInstance()->GetIOLedManager();

    switch (this->_state) {
        case E_ResetState::RESET_NONE:
            /* Check if the reset button is keeped */
            if (E_ButtonState::BTN_STATE_KEEP ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {
                /* Wait for timeout */
                this->_state = E_ResetState::RESET_WAIT;
                this->_resetStartTime = HWManager::GetTime();
            }
            break;

        case E_ResetState::RESET_WAIT:
            if (E_ButtonState::BTN_STATE_KEEP ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {

                /* Wait for timeout */
                if (RESET_PERFORM_WAIT_NS <=
                    kpBtnLastPress[BUTTON_RESET] - this->_resetStartTime) {
                    /* Perform reset */
                    this->_state = E_ResetState::RESET_PERFORM_WAIT_UP;
                    this->_resetStartTime = HWManager::GetTime();
                }
            }
            else {
                /* Go back to none */
                this->_state = E_ResetState::RESET_NONE;
            }

            pLed->SetState(E_LedID::LED_INFO, sWaitLedState);
            break;

        case E_ResetState::RESET_PERFORM_WAIT_UP:
            /* Wait for confirmation timeout check */
            if (RESET_PERFORM_TIMEOUT_NS <=
                HWManager::GetTime() - this->_resetStartTime) {
                this->_state = E_ResetState::RESET_NONE;
                break;
            }

            /* Check that the button is up again */
            if (E_ButtonState::BTN_STATE_UP ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {
                this->_state = E_ResetState::RESET_PERFORM_WAIT_DOWN;
            }

            pLed->SetState(E_LedID::LED_INFO, sWaitUpLedState);
            break;

        case E_ResetState::RESET_PERFORM_WAIT_DOWN:
            /* Wait for confirmation timeout check */
            if (RESET_PERFORM_TIMEOUT_NS <=
                HWManager::GetTime() - this->_resetStartTime) {
                this->_state = E_ResetState::RESET_NONE;
                break;
            }

            /* Check that the button is down again */
            if (E_ButtonState::BTN_STATE_DOWN ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {
                this->_state = E_ResetState::RESET_PERFORM;
            }

            pLed->SetState(E_LedID::LED_INFO, sWaitDownLedState);
            break;

        case E_ResetState::RESET_PERFORM:
            PerformReset();
            break;

        default:
            LOG_ERROR("Unknown reset state: %d.\n", this->_state);
            break;
    }
}

void ResetManager::PerformReset(void) const noexcept {

}