/*******************************************************************************
 * @file MonitorPageHandler.h
 *
 * @see MonitorPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Monitor page handler.
 *
 * @details Monitor page handler. This file defines the monitor page handler
 * used to generate the monitor page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <PageHandler.h> /* Page Handler interface */

/* Header file */
#include <MonitorPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the monitor page title */
#define MONITOR_PAGE_TITLE "Monitor"

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
MonitorPageHandler::~MonitorPageHandler(void) noexcept {
    /* Nothing to do */
}

void MonitorPageHandler::Generate(std::string& rPageTitle,
                                  std::string& rPageBody) noexcept {
    rPageTitle = MONITOR_PAGE_TITLE;
    rPageBody =
        "<div>"
        "   <h1>Monitor</h1>"
        "</div";
}