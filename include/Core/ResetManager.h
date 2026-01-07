/*******************************************************************************
 * @file ResetManager.h
 *
 * @see ResetManager.cpp
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

#ifndef __CORE_RESET_MANAGER_H__
#define __CORE_RESET_MANAGER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <cstdint>           /* Standard Integer Definitions */
#include <IOButtonManager.h> /* IO Button Action interface */

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
/** @brief Defines the allowed reset states. */
typedef enum {
    /** @brief No reset action in progress. */
    RESET_NONE,
    /** @brief Waiting reset timeout before performing reset. */
    RESET_WAIT,
    /** @brief Wait for reset button up confirmation before reset. */
    RESET_PERFORM_WAIT_UP,
    /** @brief Wait for reset button down confirmation before reset. */
    RESET_PERFORM_WAIT_DOWN,
    /** @brief Perform the reset. */
    RESET_PERFORM
} E_ResetState;

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
 * @brief Reset manager class.
 *
 * @details Reset manager class. This class provides the services
 * to handle the firmware reset and factory reset.
 */
class ResetManager : public IOButtonManagerAction {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief The Reset Manager constructor.
         *
         * @details The Reset Manager constructor. Allocates and initializes the
         * reset resources.
         */
        ResetManager(void) noexcept;

        /**
         * @brief Destroys a ResetManager.
         *
         * @details Destroys a ResetManager. Since only one object is allowed
         * in the firmware, the destructor will generate a critical error.
         */
        virtual ~ResetManager(void) noexcept;

        /**
         * @brief The execute function that checks the buttons states to define
         * the reset action.
         *
         * @details The execute function that checks the buttons states to
         * define the reset action. Based on the buttons states and the timings,
         * the function will perform reset actions.
         *
         * @param[in] kpBtnLastPress The time at which the buttons were last
         * detected as pressed.
         * @param[in] kpBtnStates The button states detected during the last
         * update.
         */
        virtual void Execute(
            const uint64_t kpBtnLastPress[E_ButtonID::BUTTON_MAX_ID],
            const E_ButtonState kpBtnStates[E_ButtonID::BUTTON_MAX_ID])
            noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Performs the firmware reset.
         *
         * @details Performs the firmware reset. This will format the persistent
         * storage and reset the board.
         */
        void PerformReset(void) const noexcept;

        /** @brief Current reset state */
        E_ResetState _state;
        /** @brief Time at the entry of the reset state. */
        uint64_t _resetStartTime;
};

#endif /* #ifndef __CORE_RESET_MANAGER_H__ */