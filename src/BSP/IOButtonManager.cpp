/*******************************************************************************
 * @file IOButtonManager.cpp
 *
 * @see IOButtonManager.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 03/01/2026
 *
 * @version 1.0
 *
 * @brief This file contains the IO buttons manager.
 *
 * @details This file contains the IO buttons manager. The file provides the
 * services read input buttons and associate interrupts to the desired pins.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <BSP.h>           /* BSP definitions */
#include <cstdint>         /* Generic Types */
#include <Logger.h>        /* Logger services */
#include <Errors.h>        /* Errors definitions */
#include <Arduino.h>       /* Arduino framework */
#include <unordered_map>   /* Unordered maps */
#include <HealthMonitor.h> /* HM services */

/* Header File */
#include <IOButtonManager.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/** @brief Time in microseconds after which we consider a button keeped. */
#define BTN_KEEP_WAIT_TIME 1000000

/** @brief Defines the lock timeout in nanoseconds. */
#define ACTION_LOCK_TIMEOUT_NS 1000000ULL

/** @brief Defines the lock timeout in ticks. */
#define ACTION_LOCK_TIMEOUT_TICKS \
    (pdMS_TO_TICKS(ACTION_LOCK_TIMEOUT_NS / 1000000ULL))

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

IOButtonManager::IOButtonManager(void) noexcept {
    uint8_t  i;

    /* Init pins and handlers */
    memset(
        this->_pBtnPins,
        -1,
        sizeof(E_GPIORouting) * E_ButtonID::BUTTON_MAX_ID
    );
    memset(
        this->_pBtnLastPress,
        0,
        sizeof(uint64_t) * E_ButtonID::BUTTON_MAX_ID
    );
    memset(
        this->_pBtnStates,
        0,
        sizeof(E_ButtonState) * E_ButtonID::BUTTON_MAX_ID
    );

    /* Init the GPIOs */
    this->_pBtnPins[E_ButtonID::BUTTON_RESET] = E_GPIORouting::GPIO_BTN_RESET;
    this->_pBtnPinsMux[E_ButtonID::BUTTON_RESET] =
        E_GPIOPull::GPIO_BTN_RESET_MUX;

    /* Setup pinmux */
    for (i = 0; E_ButtonID::BUTTON_MAX_ID > i; ++i) {
        pinMode(this->_pBtnPins[i], this->_pBtnPinsMux[i]);
    }

    /* Init actions */
    _lastActionId = 0;
    this->_lock = xSemaphoreCreateMutex();
    if (nullptr == this->_lock) {
        PANIC("Failed to initialize IO Button Manager actions lock.\n");
    }

    /* Add to system state */
    SystemState::GetInstance()->SetIOButtonManager(this);

    LOG_DEBUG("Initialized IO Button Manager.\n");
}

IOButtonManager::~IOButtonManager(void) noexcept {
    PANIC("Tried to destroy the IO Button Manager.\n");
}

void IOButtonManager::Update(void) noexcept {
    uint8_t  i;
    uint8_t  btnState;
    uint64_t currTime;

    std::unordered_map<uint32_t, IOButtonManagerAction*>::const_iterator it;

    /* Check all pins */
    for (i = 0; E_ButtonID::BUTTON_MAX_ID > i; ++i) {
        btnState = digitalRead(this->_pBtnPins[i]);

        /* Manage pinmux */
        if (INPUT_PULLUP == this->_pBtnPinsMux[i]) {
            btnState = !btnState;
        }

        if (0 != btnState) {
            currTime = HWManager::GetTime();
            /* If this is the first time the button is pressed */
            if (E_ButtonState::BTN_STATE_UP == this->_pBtnStates[i]) {
                this->_pBtnStates[i]    = E_ButtonState::BTN_STATE_DOWN;
                this->_pBtnLastPress[i] = currTime;
            }
            else if (BTN_KEEP_WAIT_TIME < currTime - this->_pBtnLastPress[i])
            {
                this->_pBtnStates[i] = E_ButtonState::BTN_STATE_KEEP;
            }
        }
        else {
            /* When the button is released, its state is allways UP */
            this->_pBtnStates[i] = E_ButtonState::BTN_STATE_UP;
        }
    }

    /* Execute actions */
    for (it = this->_actions.begin(); this->_actions.end() != it; ++it) {
        /* Execute */
        it->second->Execute(this->_pBtnLastPress, this->_pBtnStates);
    }
}

E_ButtonState IOButtonManager::GetButtonState(const E_ButtonID kBtnId)
const noexcept {
    if (E_ButtonID::BUTTON_MAX_ID > kBtnId) {
        return this->_pBtnStates[kBtnId];
    }

    return E_ButtonState::BTN_STATE_DOWN;
}

uint64_t IOButtonManager::GetButtonKeepTime(const E_ButtonID kBtnId)
const noexcept {
    if (E_ButtonID::BUTTON_MAX_ID > kBtnId &&
        E_ButtonState::BTN_STATE_KEEP == this->_pBtnStates[kBtnId]) {
        return HWManager::GetTime() - this->_pBtnLastPress[kBtnId];
    }

    return 0;
}

E_Return IOButtonManager::AddAction(IOButtonManagerAction* pAction,
                                    uint32_t&              rActionId) noexcept {
    E_Return error;

    LOG_DEBUG("Adding IO Button Manager action.\n");

    if (pdPASS == xSemaphoreTake(this->_lock, ACTION_LOCK_TIMEOUT_TICKS)) {
        if (this->_lastActionId < UINT32_MAX) {
            try {
                this->_actions.emplace(
                    std::make_pair(this->_lastActionId, pAction)
                );
                rActionId = this->_lastActionId++;
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                LOG_ERROR(
                    "Error while adding IO Button action. Error: %s.\n",
                    rExc.what()
                );
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            LOG_ERROR(
                "Error while adding IO Button action. Error: %s.\n",
                "Not enough memory"
            );
            error = E_Return::ERR_MEMORY;
        }

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            PANIC("Failed to release the IO Button Manager Action lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire actions lock.\n");
        error = E_Return::ERR_BTN_ACTION_TIMEOUT;
    }

    return error;
}

E_Return IOButtonManager::RemoveAction(const uint32_t krActionId) noexcept {
    E_Return                                                       error;
    std::unordered_map<uint32_t, IOButtonManagerAction*>::iterator it;

    LOG_DEBUG("Removing IO Button Manager action.\n");

    if (pdPASS == xSemaphoreTake(this->_lock, ACTION_LOCK_TIMEOUT_TICKS)) {
        it = this->_actions.find(krActionId);
        if (this->_actions.end() != it) {
            try {
                this->_actions.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                LOG_ERROR(
                    "Error while removing IO Button action. Error: %s.\n",
                    rExc.what()
                );
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            LOG_ERROR(
                "Error while removing IO Button action. Error: %s.\n",
                "No such ID"
            );
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            PANIC("Failed to release the IO Button Manager Action lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire actions lock.\n");
        error = E_Return::ERR_BTN_ACTION_TIMEOUT;
    }

    return error;
}