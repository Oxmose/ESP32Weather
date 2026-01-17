/*******************************************************************************
 * @file Entry.cpp
 *
 * @see Entry.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Main Module entry point.
 *
 * @details Weather Station Main Module entry point. This file contains the main
 * entry point and loop for the weather station firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>         /* Hardware services*/
#include <Logger.h>      /* Firmware logger */
#include <version.h>     /* Versionning information */
#include <Storage.h>     /* Storage manager */
#include <ModeManager.h> /* Mode manager */
#include <SystemState.h> /* System state services */

/* Header file */
#include <Entry.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

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
/** @brief Stores the current mode manager. */
static ModeManager* spModeManager;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

#ifndef UNIT_TEST

void setup(void) {
    SystemState* pSystemState;
    Storage*     pStorage;

    /* Create the system state */
    pSystemState = SystemState::GetInstance();
    if (nullptr == pSystemState) {
        PANIC("Failed to initialize system state manager.\n");
    }

    /* Create the storage manager */
    pStorage = new Storage();
    if (nullptr == pStorage) {
        PANIC("Failed to initialize system storage manager.\n");
    }

    /* Welcome output*/
    LOG_INFO("RTHR Weather Station Booting...\n");
    LOG_INFO("#==============================#\n");
    LOG_INFO("| HWUID: %s   |\n", HWManager::GetHWUID());
    LOG_INFO("| " VERSION " |\n");
    LOG_INFO("#==============================#\n");

    /* Create the mode manager */
    spModeManager = new ModeManager();
    if (nullptr == spModeManager) {
        PANIC("Failed to initialize mode manager.\n");
    }

    /* Start the firmware */
    spModeManager->StartFirmware();
}

void loop(void) {
    /* Update mode manager */
    spModeManager->PeriodicUpdate();
}

#endif /* #ifndef UNIT_TEST */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */