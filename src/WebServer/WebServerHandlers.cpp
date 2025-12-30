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
#include <Logger.h>        /* Logger services */
#include <Errors.h>        /* Errors definitions */
#include <Arduino.h>       /* Arduino Framework */
#include <Settings.h>      /* Settings services */
#include <WebServer.h>     /* Web server services */
#include <HealthMonitor.h> /* HM Services */

/* Handlers */
#include <PageHandler.h>         /* Page handler interface */
#include <IndexPageHandler.h>    /* Index page handler*/
#include <MonitorPageHandler.h>  /* Index page handler*/
#include <SettingsPageHandler.h> /* Index page handler*/
#include <SensorsPageHandler.h>  /* Index page handler*/
#include <AboutPageHandler.h>    /* Index page handler*/

/* Header file */
#include <WebServerHandlers.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the server lock timeout in nanoseconds. */
#define SERVER_LOCK_TIMEOUT_NS 1000000000ULL
/** @brief Defines the server lock timeout in ticks. */
#define SERVER_LOCK_TIMEOUT_TICKS \
    (pdMS_TO_TICKS(SERVER_LOCK_TIMEOUT_NS / 1000000ULL))

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
#define CREATE_NEW_HANDLER(URL, HANDLER_CLASS, HANDLER_OBJ) {   \
    HANDLER_OBJ = new HANDLER_CLASS();                          \
    if (nullptr == HANDLER_OBJ) {                               \
        HealthMonitor::GetInstance()->ReportHM(                 \
            E_HMEvent::HM_EVENT_WEB_SERVER_INIT_ERROR,          \
            (void*)2                                            \
        );                                                      \
    }                                                           \
    this->_pageHandlers.emplace(URL, HANDLER_OBJ);              \
    this->_pServer->on(URL, HandleKnownURL);                    \
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
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WEB_SERVER_INIT_ERROR,
            (void*)0
        );
    }

    this->_lock = xSemaphoreCreateMutex();
    if (nullptr == this->_lock) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WEB_SERVER_INIT_ERROR,
            (void*)1
        );
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
}

WebServerHandlers::~WebServerHandlers(void) {
    spInstance = nullptr;
}

void WebServerHandlers::HandleNotFound(void) noexcept {
    std::string header;
    std::string footer;
    std::string page;

    /* Generate the page */
    spInstance->GetPageHeader(header, "Not Found");
    spInstance->GetPageFooter(footer);
    page = header + "<h1>Not Found</h1>" + footer;

    /* Send */
    spInstance->GenericHandler(page, 404);
}

void WebServerHandlers::HandleKnownURL(void) noexcept {
    std::map<std::string, PageHandler*>::const_iterator it;
    S_HMParamWebServerError                             error;
    std::string                                         header;
    std::string                                         footer;
    std::string                                         page;
    std::string                                         title;
    const char*                                         pageUrl;
    String                                              pageURLStr;

    /* Get the handler */
    pageURLStr = spInstance->_pServer->uri();
    pageUrl    = pageURLStr.c_str();

    it = spInstance->_pageHandlers.find(pageUrl);
    if (spInstance->_pageHandlers.end() == it) {
        error.pPageURL = pageUrl;
        error.pPage = &page;
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WEB_SERVER_NOT_FOUND,
            (void*)&error
        );
        title = "HM Error";
    }
    else {
        it->second->Generate(title, page);
    }

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);

    /* Send */
    spInstance->GenericHandler(header + page + footer, 200);
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