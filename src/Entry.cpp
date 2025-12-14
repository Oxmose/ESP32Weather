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
#include <BSP.h>           /* Hardware services*/
#include <Logger.h>        /* Firmware logger */
#include <Errors.h>        /* Error codes */
#include <Arduino.h>       /* Arduino library */
#include <Settings.h>      /* Settings manager */
#include <HealthMonitor.h> /* Health Monitoring */

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
/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

#ifndef UNIT_TEST

void setup(void) {
    E_Return       result;
    HealthMonitor* pHealthMon;

    /* Initialize logger */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    HWManager::DelayExecNs(500000000);

    LOG_INFO("RTHR Weather Station Booting...\n");

    /* Initialize the Hardware Layer */
    HWManager::Init();
    LOG_INFO("Hardware Layer Initialized.\n");

    /* Initialize the Health Monitor */
    pHealthMon = HealthMonitor::GetInstance();
    pHealthMon->Init();
    LOG_INFO("Health Monitor Initialized.\n");

    /* Initialize the setting manager */
    result = Settings::InitInstance();
    if (E_Return::NO_ERROR != result) {
        /* TODO: Health Monitor Notify */
    }
}

void loop(void) {

}

#endif /* #ifndef UNIT_TEST */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
/* None */