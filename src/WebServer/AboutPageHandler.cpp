/*******************************************************************************
 * @file AboutPageHandler.h
 *
 * @see AboutPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief About page handler.
 *
 * @details About page handler. This file defines the about page handler
 * used to generate the about page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>         /* BSP Services */
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <version.h>     /* Versioning */
#include <PageHandler.h> /* Page Handler interface */

/* Header file */
#include <AboutPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the about page title */
#define ABOUT_PAGE_TITLE "About"

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
AboutPageHandler::~AboutPageHandler(void) noexcept {
    /* Nothing to do */
}

void AboutPageHandler::Generate(std::string& rPageTitle,
                                std::string& rPageBody) noexcept {
    rPageTitle = ABOUT_PAGE_TITLE;

    /* Title + Header information */
    rPageBody =
        "<div>"
        "   <h2>Real-Time High-Reliability Weather Station</h2>"
        "   <h3>HWUID: ";
    rPageBody += HWManager::GetHWUID();
    rPageBody +=
        "  | " VERSION " </h3>"
        "<p>Real-Time High-Reliability Weather Station is designed by "
        "<a href=\"https://me.olsontek.dev\">Alexy Torres</a>. It is made to be"
        " highly reliable and interface with various sensors to provide "
        "extended information about the environment.</p>"
        "<p>Version / Build | " VERSION "</p>"
        "<p>GitHub Project: <a "
        "href=\"https://github.com/Oxmose/ESP32Weather\">"
        "ESP32Weather</a></p>";
    rPageBody += "</div>";
}