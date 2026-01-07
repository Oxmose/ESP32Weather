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
#include <Logger.h>          /* Logger service */
#include <nvs_flash.h>       /* NVS formating */
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
#define RESET_PERFORM_TIMEOUT_NS 5000000000ULL

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
    .green = 0,
    .blue = 0,
    .blinkPeriodNs = 0,
    .isOn = false
};
/** @brief Waiting up led state information. */
static const S_LedState sWaitUpLedState = {
    .enabled = true,
    .red = 0,
    .green = 10,
    .blue = 50,
    .blinkPeriodNs = 250000000ULL,
    .isOn = true
};
/** @brief Waiting down led state information. */
static const S_LedState sWaitDownLedState = {
    .enabled = true,
    .red = 0,
    .green = 25,
    .blue = 0,
    .blinkPeriodNs = 100000000ULL,
    .isOn = true
};
/** @brief Waiting none led state information. */
static const S_LedState sNoneLedState = {
    .enabled = true,
    .red = 0,
    .green = 0,
    .blue = 0,
    .blinkPeriodNs = 0,
    .isOn = false
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

    LOG_DEBUG("Reset Manager initialized.\n");
}

ResetManager::~ResetManager(void) noexcept {
    PANIC("Tried to destroy the Reset Manager.\n");
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
                this->_resetStartTime = HWManager::GetTime();
                this->_state = E_ResetState::RESET_WAIT;
                pLed->SetState(E_LedID::LED_INFO, sWaitLedState);

                LOG_DEBUG("Reset manager transitioning: NONE -> WAIT.\n");
            }

            break;

        case E_ResetState::RESET_WAIT:
            if (E_ButtonState::BTN_STATE_KEEP ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {

                /* Wait for timeout */
                if (RESET_PERFORM_WAIT_NS <=
                    HWManager::GetTime() - this->_resetStartTime) {
                    /* Perform reset */
                    this->_resetStartTime = HWManager::GetTime();
                    this->_state = E_ResetState::RESET_PERFORM_WAIT_UP;
                    pLed->SetState(E_LedID::LED_INFO, sWaitUpLedState);

                    LOG_DEBUG(
                        "Reset manager transitioning: WAIT -> WAIT_UP.\n"
                    );
                }
            }
            else {
                /* Go back to none */
                this->_state = E_ResetState::RESET_NONE;
                pLed->SetState(E_LedID::LED_INFO, sNoneLedState);
            }

            break;

        case E_ResetState::RESET_PERFORM_WAIT_UP:
            /* Check that the button is up again */
            if (E_ButtonState::BTN_STATE_UP ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {
                this->_resetStartTime = HWManager::GetTime();
                this->_state = E_ResetState::RESET_PERFORM_WAIT_DOWN;
                pLed->SetState(E_LedID::LED_INFO, sWaitDownLedState);

                LOG_DEBUG(
                    "Reset manager transitioning: WAIT_UP -> WAIT_DOWN.\n"
                );
            }

            break;

        case E_ResetState::RESET_PERFORM_WAIT_DOWN:
            /* Wait for confirmation timeout check */
            if (RESET_PERFORM_TIMEOUT_NS <=
                HWManager::GetTime() - this->_resetStartTime) {
                this->_state = E_ResetState::RESET_NONE;
                pLed->SetState(E_LedID::LED_INFO, sNoneLedState);
                break;

                LOG_DEBUG(
                    "Reset manager transitioning: WAIT_UP -> NONE.\n"
                );
            }

            /* Check that the button is down again */
            if (E_ButtonState::BTN_STATE_DOWN ==
                kpBtnStates[E_ButtonID::BUTTON_RESET]) {
                this->_state = E_ResetState::RESET_PERFORM;

                LOG_DEBUG(
                    "Reset manager transitioning: WAIT_UP -> PERFORM.\n"
                );
            }
            break;

        case E_ResetState::RESET_PERFORM:
            PerformReset();
            this->_state = E_ResetState::RESET_NONE;
            pLed->SetState(E_LedID::LED_INFO, sNoneLedState);

            LOG_DEBUG(
                "Reset manager transitioning: PERFORM -> NONE.\n"
            );
            break;

        default:
            LOG_ERROR("Unknown reset state: %d.\n", this->_state);
            break;
    }
}

void ResetManager::PerformReset(void) const noexcept {
    nvs_flash_erase();
    LOG_INFO("Formated persistent memory.\n");
    HWManager::Reboot();
}