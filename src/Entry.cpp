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
#include <Arduino.h>    /* Arduino library */
#include <Logger.h>     /* Firmware logger */
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

void setup(void) {

    /* Initialize logger and wait */
    INIT_LOGGER(LOG_LEVEL_DEBUG);
    delayMicroseconds(500000); // TODO: Update with HAL when done

    LOG_INFO("RTHR Weather Station Booting...\n");

    /* Initialize the WiFi module*/
    spWifiModule = new WiFiModule("TestSSID", "12345678");
    if (nullptr == spWifiModule) {
        LOG_ERROR("Failed to instanciate the WiFi Module\n");
        idle();
    }

    if (E_Return::NO_ERROR != spWifiModule->Start()) {
        LOG_ERROR("Failed to start the WiFi Module\n");
        idle();
    }
}

void loop(void) {

}

void idle(void) {
    while(1);
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/