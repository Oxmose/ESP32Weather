/*******************************************************************************
 * @file SensorsPageHandler.h
 *
 * @see SensorsPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Sensors page handler.
 *
 * @details Sensors page handler. This file defines the sensors page handler
 * used to generate the sensors page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <Logger.h>      /* Logger services */
#include <PageHandler.h> /* Page Handler interface */

/* Header file */
#include <SensorsPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the sensors page title */
#define SENSORS_PAGE_TITLE "Sensors"

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
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
SensorsPageHandler::~SensorsPageHandler(void) noexcept {
    PANIC("Tried to destroy the Sensors page handler.\n");
}

void SensorsPageHandler::Generate(std::string& rPageTitle,
                                  std::string& rPageBody) noexcept {
    rPageTitle = SENSORS_PAGE_TITLE;
    rPageBody =
        "<div>"
        "   <h1>Sensors</h1>"
        "</div";
}