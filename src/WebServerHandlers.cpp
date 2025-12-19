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
#include <WebServer.h>     /* Web server services */
#include <HealthMonitor.h> /* HM Services */
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
#define WEB_SERVER_URL_INDEX "/"

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
/** @brief See WebServerHandlers.h */
WebServer* WebServerHandlers::_SPSERVER = nullptr;
/** @brief See WebServerHandlers.h */
SemaphoreHandle_t WebServerHandlers::_SSERVER_LOCK = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
E_Return WebServerHandlers::Init(WebServer* pServer) noexcept {
    E_Return error;

    /* Create the server lock */
    if (nullptr == _SSERVER_LOCK) {
        _SSERVER_LOCK = xSemaphoreCreateMutex();
        if (nullptr == _SSERVER_LOCK) {
            LOG_ERROR("Failed to initialize the server lock.\n");
            error = E_Return::ERR_WEB_SERVER_LOCK;
        }
        else {
            _SPSERVER = pServer;
            error = E_Return::NO_ERROR;
        }
    }
    else {
        _SPSERVER = pServer;
        error = E_Return::NO_ERROR;
    }

    if (E_Return::NO_ERROR == error) {
        /* Configure the handlers */
        pServer->onNotFound(HandleNotFound);
        pServer->on(WEB_SERVER_URL_INDEX, HandleIndex);
    }

    return error;
}

void WebServerHandlers::HandleNotFound(void) noexcept {
    std::string header;
    std::string footer;
    std::string page;

    /* Generate the page */
    GetPageHeader(header, "Not Found");
    GetPageFooter(footer);
    page = header + "<h1>Not Found</h1>" + footer;

    /* Send */
    GenericHandler(page, 404);
}

void WebServerHandlers::HandleIndex(void) noexcept {
    std::string header;
    std::string footer;
    std::string page;

    /* Generate the page */
    GetPageHeader(header, "Index");
    GetPageFooter(footer);
    page = header + "<h1>Index</h1>" + footer;

    /* Send */
    GenericHandler(page, 200);
}

void WebServerHandlers::GetPageHeader(std::string& rHeaderStr,
                                      const std::string& krTitle) noexcept {
    rHeaderStr.clear();
    rHeaderStr += "<!DOCTYPE html>\n"
                  "<html lang='en'>\n"
                  "<head>\n"
                  "<meta http-equiv='refresh' content='60' name='viewport' "
                  "content='width=device-width, initial-scale=1' "
                  "charset='UTF-8'/>\n"
                  "<title>\n";
    rHeaderStr += krTitle;
    rHeaderStr += "</title>\n"
                  "</head>\n"
                  "<body>";
}

void WebServerHandlers::GetPageFooter(std::string& rFooterStr) noexcept {
    rFooterStr.clear();
    rFooterStr += "</body>\n</html>";
}

void WebServerHandlers::GenericHandler(const std::string& krPage,
                                       const int32_t      kCode) noexcept {
    if (nullptr != _SPSERVER) {
        /* Update page length and send */
        if (pdPASS == xSemaphoreTake(
            _SSERVER_LOCK,
            SERVER_LOCK_TIMEOUT_TICKS
        )) {
            _SPSERVER->setContentLength(krPage.size());
            _SPSERVER->send(kCode, "text/html", krPage.c_str());

            if (pdPASS != xSemaphoreGive(_SSERVER_LOCK)) {
                HealthMonitor::GetInstance()->ReportHM(
                    E_HMEvent::HM_EVENT_WEB_SERVER_LOCK,
                    (void*)1
                );
            }
        }
        else {
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_WEB_SERVER_LOCK,
                (void*)0
            );
        }
    }
    else {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WEB_SERVER_NO_SERVER
        );
    }
}