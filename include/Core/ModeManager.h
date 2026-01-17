/*******************************************************************************
 * @file ModeManager.h
 *
 * @see ModeManager.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Firmware mode manager.
 *
 * @details Firmware mode manager. The firmware has two execution mode: nominal
 * and maintenance. The maintenance mode is activated when maintenance is
 * required on the firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __CORE_MODE_MANAGER_H__
#define __CORE_MODE_MANAGER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <Errors.h>    /* Errors definitions */
#include <WebServer.h> /* Web server services */
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

/** @brief Defines the allowed execution modes for the firmware. */
typedef enum {
    /** @brief Nominal execution mode. */
    MODE_NOMINAL,
    /** @brief Maintenance execution mode. */
    MODE_MAINTENANCE,
    /** @brief Faulted execution mode. */
    MODE_FAULTED
} E_Mode;

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
 * @brief Mode manager class.
 *
 * @details Mode manager class. Provides the functionalities to switch between
 * nominal and maintenance mode and perform maintenance operations.
 */
class ModeManager {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief The Mode Manager constructor.
         *
         * @details The Mode Manager constructor. Allocates and initializes the
         * manager resources.
         */
        ModeManager(void) noexcept;

        /**
         * @brief Destroys a ModeManager.
         *
         * @details Destroys a ModeManager. Since only one object is allowed
         * in the firmware, the destructor will generate a critical error.
         */
        ~ModeManager(void) noexcept;

        /**
         * @brief Sets the execution mode and reboot the firmware.
         *
         * @details Sets the execution mode and reboot the firmware. This will
         * save the execution mode in persistent storage and restart the
         * firmware.
         *
         * @param[in] kMode The execution mode to set.
         *
         * @return The function returns the success or error status.
         */
        E_Return SetMode(const E_Mode kMode) noexcept;

        /**
         * @brief Returns the current execution mode.
         *
         * @details Returns the current execution mode.
         *
         * @return Returns the current execution mode.
         */
        E_Mode GetMode(void) const noexcept;

        /**
         * @brief Starts the firmware.
         *
         * @details Starts the firmware. This function will retrieve the
         * execution mode and start the firmware in the requested mode.
         */
        void StartFirmware(void) noexcept;

        /**
         * @brief Updates the mode manager.
         *
         * @details Updates the mode mamanger. Depending on the current mode,
         * the update will perform various tasks such as handling the
         * maintenance actions.
         */
        void PeriodicUpdate(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:

        /**
         * @brief Starts the firmware in nominal execution mode.
         *
         * @details Starts the firmware in nominal execution mode. This is the
         * regular execution mode.
         */
        void StartNominal(void) const noexcept;

        /**
         * @brief Starts the firmware in maintenance execution mode.
         *
         * @details Starts the firmware in maintenance execution mode. This is
         * the execution mode used to perform maintenance actions.
         */
        void StartMaintenance(void) noexcept;

        /**
         * @brief Starts the maintenance web server.
         *
         * @details Starts the maintenance web server. The web server will be
         * the only way of interaction with the firmware in maintenance mode.
         */
        void StartMaintenanceServer(void) noexcept;

        /**
         * @brief Retrieves the last reset reason and perform actions
         * accordingly.
         *
         * @brief Retrieves the last reset reason and perform actions
         * accordingly. Actions are based on the type of reset. The maintenance
         * mode might be forced.
         */
        void GetLastReset(void) noexcept;

        /** @brief Stores the current mode of execution. */
        E_Mode _currentMode;

        /** @brief Stores the maintenance web server. */
        WebServer* _pMaintServer;

        /** @brief Force maintenance mode. */
        bool _forceMaintenance;
};

#endif /* #ifndef __CORE_MODE_MANAGER_H__ */