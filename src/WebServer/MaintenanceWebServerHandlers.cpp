/*******************************************************************************
 * @file MaintenanceWebServerHandlers.cpp
 *
 * @see MaintenanceWebServerHandlers.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Maintenance Web Server URL handlers.
 *
 * @details Maintenance Web Server URL handlers. This file defines all the
 * handlers used for the maintenance web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>           /* Hardware layer */
#include <Errors.h>        /* Errors definitions */
#include <Logger.h>        /* Logger services */
#include <Arduino.h>       /* Arduino Framework */
#include <version.h>       /* Versioning  */
#include <Settings.h>      /* Settings services */
#include <WebServer.h>     /* Web server services */
#include <SystemState.h>   /* Get the system state */
#include <ModeManager.h>   /* Mode management */

/* Header file */
#include <MaintenanceWebServerHandlers.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the index URL */
#define PAGE_URL_INDEX "/"
/** @brief Defines the reboot URL */
#define PAGE_URL_MONITOR "/reboot"

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
/** @brief Stores the current web server instance. */
static MaintenanceWebServerHandlers* spInstance = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
MaintenanceWebServerHandlers::MaintenanceWebServerHandlers(WebServer* pServer)
noexcept {
    this->_pServer = pServer;

    /* Configure the handlers */
    this->_pServer->onNotFound(HandleNotFound);
    this->_pServer->on(PAGE_URL_INDEX, HandleIndex);
    this->_pServer->on(PAGE_URL_MONITOR, HandleReboot);

    /* Set the instance */
    spInstance = this;

    LOG_DEBUG("Maintenance Web Server Page Handlers manager initialized.\n");
}

MaintenanceWebServerHandlers::~MaintenanceWebServerHandlers(void) {
    PANIC(
        "Tried to destroy the Maintenance Web Server page handlers manager.\n"
    );
}

void MaintenanceWebServerHandlers::HandleNotFound(void) noexcept {
    std::string header;
    std::string footer;
    std::string page;

    LOG_DEBUG(
        "Handling Web page not found: %s\n",
        spInstance->_pServer->uri().c_str()
    );

    /* Generate the page */
    spInstance->GetPageHeader(header, "Not Found");
    spInstance->GetPageFooter(footer);
    page = header + "<h1>Not Found</h1>" + footer;

    /* Send */
    spInstance->GenericHandler(page, 404);
}

void MaintenanceWebServerHandlers::HandleIndex(void) noexcept {
    std::string title;
    std::string page;
    std::string footer;
    std::string header;

    page =
        "<div>"
        "   <h1>Real-Time High-Reliability Weather Station</h1>"
        "   <h2>HWUID: ";
    page += HWManager::GetHWUID();
    page +=
        "  | " VERSION "</h2>"
        "</div>";

    page += "<div>";
    page += "<h3>==== Maintenance Mode ====</h3>";
    page += "<table>";
    page += "<tr>";
    page += "<td><a href=\"/reboot?mode=0\">Reboot in nominal mode</a></td>";
    page += "<td><a href=\"/reboot?mode=1\">Reboot in maintenance mode</a></td>";
    page += "</tr>";
    page += "</table>";
    page += "</div>";

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);


    /* Send */
    spInstance->GenericHandler(header + page + footer, 200);
}

void MaintenanceWebServerHandlers::HandleReboot(void) noexcept {
    std::string title;
    std::string page;
    std::string footer;
    std::string header;
    String      arg;
    E_Return    error;

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);


    if (spInstance->_pServer->hasArg("mode")) {
        arg = spInstance->_pServer->arg("mode");

        /* Get the reboot mode */
        if (arg.equals("0")) {
            /* Send */
            page = "<div><h1>Rebooting in nominal mode.</h1></div>";
            spInstance->GenericHandler(header + page + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_NOMINAL
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set nominal mode.\n");
            }
        }
        else if (arg.equals("1")) {
            /* Send */
            page = "<div><h1>Rebooting in maintenance mode.</h1></div>";
            spInstance->GenericHandler(header + page + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_MAINTENANCE
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set maintenance mode.\n");
            }
        }
        else {
            page = "<div><h1>Unknown reboot mode.</h1></div>";
        }
    }
    else {
        page = "<div><h1>Unknown reboot mode.</h1></div>";
    }

    /* Send */
    spInstance->GenericHandler(header + page + footer, 200);
}

void MaintenanceWebServerHandlers::GetPageHeader(std::string&       rHeaderStr,
                                                 const std::string& krTitle)
const noexcept {
    rHeaderStr.clear();
    rHeaderStr += "<!DOCTYPE html>\n"
                  "<html lang='en'>\n"
                  "<head>\n";
    rHeaderStr += "<meta name='viewport' ";
    rHeaderStr += "content='width=device-width, initial-scale=1' "
                  "charset='UTF-8'/>\n"
                  "<title>\n";
    rHeaderStr += krTitle;
    rHeaderStr += "</title>\n";
    GeneratePageCSS(rHeaderStr);
    rHeaderStr += "</head>\n"
                  "<body>";
}

void MaintenanceWebServerHandlers::GetPageFooter(std::string& rFooterStr)
const noexcept {
    rFooterStr.clear();
    rFooterStr += "</div></body>\n</html>";
}

void MaintenanceWebServerHandlers::GeneratePageCSS(std::string& rHeaderStr)
const noexcept {
    rHeaderStr +=
        "<style>"
        "   body {"
        "       font-family: monospace;"
        "   }"
        "   table, th, td {"
        "       border: 1px dashed gray;"
        "       border-collapse: collapse;"
        "   }"
        "   td, th {"
        "       padding: 5px;"
        "   }"
        "</style>";
}

void MaintenanceWebServerHandlers::GenericHandler(const std::string& krPage,
                                                  const int32_t      kCode)
noexcept {
    /* Update page length and send */
    this->_pServer->setContentLength(krPage.size());
    this->_pServer->send(kCode, "text/html", krPage.c_str());
}