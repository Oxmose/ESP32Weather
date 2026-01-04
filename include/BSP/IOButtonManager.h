/*******************************************************************************
 * @file IOButtonManager.h
 *
 * @see IOButtonManager.cpp
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

#ifndef __BSP_IOBUTTON_MANAGER_H_
#define __BSP_IOBUTTON_MANAGER_H_

/****************************** OUTER NAMESPACE *******************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <map>       /* std::map */
#include <BSP.h>     /* BSP definitions */
#include <cstdint>   /* Generic Types */
#include <Errors.h>  /* Errors definitions */
#include <Arduino.h> /* Arduino framework */

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

/** @brief Defines the button states. */
typedef enum {
    /** @brief Button is released */
    BTN_STATE_UP,
    /** @brief Button is pressed */
    BTN_STATE_DOWN,
    /** @brief Button has been pressed for more than a specified time */
    BTN_STATE_KEEP
} E_ButtonState;

/** @brief Defines the button functionalities. */
typedef enum {
    /** @brief Button function: reset */
    BUTTON_RESET,
    /** @brief Max ID */
    BUTTON_MAX_ID
} E_ButtonID;

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
 * @brief Button action interface.
 *
 * @details Button actiopn interface. The interface defines the methods the
 * button action classes should implement.
 */
class IOButtonManagerAction {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief The button action interface destructor.
         *
         * @details The button action interface destructor. The inherited
         * classes should implement their own destructor.
         */
        virtual ~IOButtonManagerAction(void) noexcept {};

        /**
         * @brief The execute function that is called everythime the button
         * manager is updated.
         *
         * @details The execute function that is called everythime the button
         * manager is updated. This function performs the action provided by
         * the button action class and should be implemented by the inherited
         * class.
         *
         * @param[in] kpBtnLastPress The time at which the buttons were last
         * detected as pressed.
         * @param[in] kpBtnStates The button states detected during the last
         * update.
         */
        virtual void Execute(
            const uint64_t kpBtnLastPress[E_ButtonID::BUTTON_MAX_ID],
            const E_ButtonState kpBtnStates[E_ButtonID::BUTTON_MAX_ID])
            noexcept = 0;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /* None */
};

/**
 * @brief Button manager class.
 *
 * @details Button manager class. This class provides the services
 * to retrieve the button states.
 */
class IOButtonManager {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Constructs a new IOButtonManager object.
         *
         * @details Constructs a new IOButtonManager object.
         * The constructor initializes the buttons and their GPIOs.
         */
        IOButtonManager(void) noexcept;

        /**
         * @brief Updates the button states.
         *
         * @details Updates the button states. This function also calculate the
         * time a button has been pressed to setup the KEEP state.
         */
        void Update(void) noexcept;

        /**
         * @brief Get the Button State.
         *
         * @param[in] kBtnId The button for which the state shall be returned.
         *
         * @return The function returns the button state.
         */
        E_ButtonState GetButtonState(const E_ButtonID kBtnId) const noexcept;

        /**
         * @brief Get the Button Keep Time.
         *
         * @param[in] kBtnId The button for which the keep state shall be
         * returned.
         *
         * @return The function returns the time the button has been in the
         * KEPP state.
         */
        uint64_t GetButtonKeepTime(const E_ButtonID kBtnId) const noexcept;

        /**
         * @brief Adds an action to execute at each button update.
         *
         * @details Adds an action to execute at each button update. The action
         * identifier is returned in the ID buffer.
         *
         * @param[in] pAction The action to add.
         * @param[out] rActionId The action identifier buffer.
         *
         * @return The function returns the success or error status.
         */
        E_Return AddAction(IOButtonManagerAction* pAction, uint32_t& rActionId)
        noexcept;

        /**
         * @brief Removes an action to execute at each button update.
         *
         * @details Removes an action to execute at each button update. The
         * identifier is used to remove the action.
         *
         * @param[in] krActionId The action identifier.
         *
         * @return The function returns the success or error status.
         */
        E_Return RemoveAction(const uint32_t krActionId) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief Stores the buttons GPIO pins */
        E_GPIORouting _pBtnPins[E_ButtonID::BUTTON_MAX_ID];
        /** @brief Stores the button GPIO pin mux */
        E_GPIOPull _pBtnPinsMux[E_ButtonID::BUTTON_MAX_ID];
        /** @brief Stores the time as which the buttons were pressed last. */
        uint64_t _pBtnLastPress[E_ButtonID::BUTTON_MAX_ID];
        /** @brief Stores the buttons state. */
        E_ButtonState _pBtnStates[E_ButtonID::BUTTON_MAX_ID];
        /** @brief Stores the button actions. */
        std::map<uint32_t, IOButtonManagerAction*> _actions;
        /** @brief Stores the last given action identifier. */
        uint32_t _lastActionId;
        /** @brief Stores the actions mutex. */
        SemaphoreHandle_t _lock;

};

#endif /* #ifndef __BSP_IOBUTTON_MANAGER_H_ */