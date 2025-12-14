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
#include <Logger.h>     /* Firmware logger */
#include <Errors.h>     /* Error codes */
#include <Arduino.h>    /* Arduino library */
#include <Settings.h>   /* Settings manager */
#include <WiFiModule.h> /* WiFi services */

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
static WiFiModule* spWifiModule;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

#ifndef UNIT_TEST

void setup(void) {
    E_Return result;

    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    delayMicroseconds(500000); // TODO: Update with HAL when done


    LOG_INFO("RTHR Weather Station Booting...\n");

    /* Initialize the setting manager */
    result = Settings::InitInstance();
    if (E_Return::NO_ERROR != result) {
        /* TODO: Health Monitor Notify */
    }

    /* Initialize the WiFi module*/
    spWifiModule = new WiFiModule("TestSSID", "12345678");
    if (nullptr == spWifiModule) {
        LOG_ERROR("Failed to instanciate the WiFi Module\n");
        /* TODO: Health Monitor Notify */
    }

    if (E_Return::NO_ERROR != spWifiModule->Start()) {
        LOG_ERROR("Failed to start the WiFi Module\n");
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