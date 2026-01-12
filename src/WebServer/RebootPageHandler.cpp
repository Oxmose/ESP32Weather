/*******************************************************************************
 * @file RebootHandler.cpp
 *
 * @see RebootHandler.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Reboot page handler.
 *
 * @details Reboot page handler. This file defines the reboot page handler
 * used to generate the reboot page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>              /* Standard string */
#include <Errors.h>            /* Errors definitions */
#include <Logger.h>            /* Logger services */
#include <PageHandler.h>       /* Page Handler interface */
#include <SystemState.h>       /* System state services */
#include <ModeManager.h>       /* Mode management */
#include <WebServerHandlers.h> /* Handlers */

/* Header file */
#include <RebootPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the rebot page title */
#define REBOOT_PAGE_TITLE "Reboot"

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
RebootPageHandler::RebootPageHandler(WebServerHandlers* pHandlers) noexcept :
PageHandler(pHandlers) {
}

RebootPageHandler::~RebootPageHandler(void) noexcept {
    PANIC("Tried to destroy the Reboot page handler.\n");
}

void RebootPageHandler::Generate(std::string& rPageTitle,
                                 std::string& rPageBody) noexcept {
    WebServer*         pServer;
    WebServerHandlers* pHandlers;
    E_Return           error;
    String             arg;
    std::string        header;
    std::string        footer;

    rPageTitle = REBOOT_PAGE_TITLE;

    pHandlers = GetHandlers();
    pServer = pHandlers->GetServer();

    if (pServer->hasArg("mode")) {
        arg = pServer->arg("mode");

        /* Get the reboot mode */
        if (arg.equals("0")) {
            /* Send */
            pHandlers->GetPageHeader(header, rPageTitle);
            pHandlers->GetPageFooter(footer);

            rPageBody = "<div><h1>Rebooting in nominal mode.</h1></div>";
            pHandlers->GenericHandler(header + rPageBody + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_NOMINAL
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set nominal mode.\n");
            }
        }
        else if (arg.equals("1")) {
            /* Send */
            pHandlers->GetPageHeader(header, rPageTitle);
            pHandlers->GetPageFooter(footer);

            rPageBody = "<div><h1>Rebooting in maintenance mode.</h1></div>";
            pHandlers->GenericHandler(header + rPageBody + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_MAINTENANCE
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set maintenance mode.\n");
            }
        }
        else {
            rPageBody = "<div><h1>Unknown reboot mode.</h1></div>";
        }
    }
    else {
        rPageBody = "<div><h1>Unknown reboot mode.</h1></div>";
    }
}