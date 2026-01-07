/*******************************************************************************
 * @file WebServerHandlers.cpp
 *
 * @see WebServerHandlers.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 15/12/2025
 *
 * @version 1.0
 *
 * @brief Web Server URL handlers.
 *
 * @details Web Server URL handlers. This file defines all the handlers used
 * for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <Errors.h>        /* Errors definitions */
#include <Logger.h>        /* Logger services */
#include <Arduino.h>       /* Arduino Framework */
#include <Settings.h>      /* Settings services */
#include <WebServer.h>     /* Web server services */
#include <HealthMonitor.h> /* HM Services */

/* Handlers */
#include <PageHandler.h>         /* Page handler interface */
#include <IndexPageHandler.h>    /* Index page handler*/
#include <MonitorPageHandler.h>  /* Monitor page handler*/
#include <SettingsPageHandler.h> /* Settings page handler*/
#include <SensorsPageHandler.h>  /* Sensors page handler*/
#include <AboutPageHandler.h>    /* About page handler*/

/* Header file */
#include <WebServerHandlers.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the index URL */
#define PAGE_URL_INDEX "/"
/** @brief Defines the monitoring URL */
#define PAGE_URL_MONITOR "/monitor"
/** @brief Defines the settings URL */
#define PAGE_URL_SETTINGS "/settings"
/** @brief Defines the sensors URL */
#define PAGE_URL_SENSORS "/sensors"
/** @brief Defines the about URL */
#define PAGE_URL_ABOUT "/about"

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/**
 * @brief Creates a new web handler.
 *
 * @details  Creates a new web handler. This will add the handler to the
 * handlers table and generate a HM event in case of error.
 *
 * @param[in] URL The URL to handle.
 * @param[in] HANDLER_CLASS The class of the object used to handle the URL.
 * @param[out] HANDLER_OBJ The object that gets assigned the new handler.
 */
#define CREATE_NEW_HANDLER(URL, HANDLER_CLASS, HANDLER_OBJ) {               \
    HANDLER_OBJ = new HANDLER_CLASS();                                      \
    if (nullptr == HANDLER_OBJ) {                                           \
        PANIC("Failed to allocate a new handler for Web page %s.\n", URL);  \
    }                                                                       \
    this->_pageHandlers.emplace(URL, HANDLER_OBJ);                          \
    this->_pServer->on(URL, HandleKnownURL);                                \
}

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
static WebServerHandlers* spInstance = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
WebServerHandlers::WebServerHandlers(WebServer* pServer) noexcept {
    PageHandler* pNewHandler;

    if (nullptr != spInstance) {
        PANIC("Tried to re-create the Web Server handlers manager.\n");
    }

    this->_pServer = pServer;

    /* Create the handlers */
    CREATE_NEW_HANDLER(PAGE_URL_INDEX, IndexPageHandler, pNewHandler);
    CREATE_NEW_HANDLER(PAGE_URL_MONITOR, MonitorPageHandler, pNewHandler);
    CREATE_NEW_HANDLER(PAGE_URL_SETTINGS, SettingsPageHandler, pNewHandler);
    CREATE_NEW_HANDLER(PAGE_URL_SENSORS, SensorsPageHandler, pNewHandler);
    CREATE_NEW_HANDLER(PAGE_URL_ABOUT, AboutPageHandler, pNewHandler);

    /* Configure the not found handler */
    this->_pServer->onNotFound(HandleNotFound);

    /* Set the instance */
    spInstance = this;

    LOG_DEBUG("Web Server Page Handlers manager initialized.\n");
}

WebServerHandlers::~WebServerHandlers(void) {
    PANIC("Tried to destroy the Web Server page handlers manager.\n");
}

void WebServerHandlers::HandleNotFound(void) noexcept {
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

void WebServerHandlers::HandleKnownURL(void) noexcept {
    std::unordered_map<std::string, PageHandler*>::const_iterator it;
    std::string                                                   header;
    std::string                                                   footer;
    std::string                                                   page;
    std::string                                                   title;
    const char*                                                   pageUrl;
    String                                                        pageURLStr;
    int32_t                                                       code;

    LOG_DEBUG(
        "Handling Web page: %s\n",
        spInstance->_pServer->uri().c_str()
    );

    /* Get the handler */
    pageURLStr = spInstance->_pServer->uri();
    pageUrl    = pageURLStr.c_str();

    it = spInstance->_pageHandlers.find(pageUrl);
    if (spInstance->_pageHandlers.end() == it) {
        spInstance->GetPageHeader(header, "Not Found");
        spInstance->GetPageFooter(footer);
        page = "<h1>Not Registered</h1>";
        code = 500;
    }
    else {
        it->second->Generate(title, page);
        code = 200;
    }

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);

    /* Send */
    spInstance->GenericHandler(header + page + footer, code);
}

void WebServerHandlers::GetPageHeader(std::string&       rHeaderStr,
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

void WebServerHandlers::GetPageFooter(std::string& rFooterStr) const noexcept {
    rFooterStr.clear();
    rFooterStr += "<br /><div>";
    rFooterStr += "<h2>==== Navigation ====</h2>";
    rFooterStr += "<table><tr>";
    rFooterStr += "<td><a href=\"/\">Home</a></td>";
    rFooterStr += "<td><a href=\"/monitor\">Monitor</a></td>";
    rFooterStr += "<td><a href=\"/settings\">Settings</a></td>";
    rFooterStr += "<td><a href=\"/sensors\">Sensors</a></td>";
    rFooterStr += "<td><a href=\"/about\">About</a></td>";
    rFooterStr += "</tr></table>";
    rFooterStr += "</div></body>\n</html>";
}

void WebServerHandlers::GeneratePageCSS(std::string& rHeaderStr) const noexcept {
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

void WebServerHandlers::GenericHandler(const std::string& krPage,
                                       const int32_t      kCode) noexcept {
    /* Update page length and send */
    this->_pServer->setContentLength(krPage.size());
    this->_pServer->send(kCode, "text/html", krPage.c_str());
}