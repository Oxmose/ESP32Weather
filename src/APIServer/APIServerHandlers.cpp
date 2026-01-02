/*******************************************************************************
 * @file APIServerHandlers.cpp
 *
 * @see APIServerHandlers.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 15/12/2025
 *
 * @version 1.0
 *
 * @brief API Server URL handlers.
 *
 * @details API Server URL handlers. This file defines all the handlers used
 * for the API server.
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
#include <APIHandler.h>            /* API handler interface */
#include <PingAPIHandler.h>        /* Ping handler */
#include <WiFiSettingAPIHandler.h> /* WiFi Settings handler */

/* Header file */
#include <APIServerHandlers.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the ping URL */
#define API_URL_PING "/ping"
/** @brief Defines the ping URL */
#define API_URL_WIFI "/wifi"

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/**
 * @brief Creates a new API handler.
 *
 * @details  Creates a new API handler. This will add the handler to the
 * handlers table and generate a HM event in case of error.
 *
 * @param[in] URL The URL to handle.
 * @param[in] HANDLER_CLASS The class of the object used to handle the URL.
 * @param[out] HANDLER_OBJ The object that gets assigned the new handler.
 */
#define CREATE_NEW_HANDLER(URL, HANDLER_CLASS, HANDLER_OBJ) {           \
    HANDLER_OBJ = new HANDLER_CLASS();                                  \
    if (nullptr == HANDLER_OBJ) {                                       \
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_API_SERVER_INIT_ERROR, 2);  \
    }                                                                   \
    this->_apiHandlers.emplace(URL, HANDLER_OBJ);                       \
    this->_pServer->on(URL, HTTP_POST, HandleKnownURL);                 \
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
/** @brief Stores the current API server instance. */
static APIServerHandlers* spInstance = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
APIServerHandlers::APIServerHandlers(WebServer* pServer) noexcept {
    APIHandler* pNewHandler;

    if (nullptr != spInstance) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_API_SERVER_INIT_ERROR, 0);
    }

    this->_pServer = pServer;

    /* Create the handlers */
    CREATE_NEW_HANDLER(API_URL_PING, PingAPIHandler, pNewHandler);
    CREATE_NEW_HANDLER(API_URL_WIFI, WiFiSettingAPIHandler, pNewHandler);

    /* Configure the not found handler */
    this->_pServer->onNotFound(HandleNotFound);

    /* Set the instance */
    spInstance = this;
}

APIServerHandlers::~APIServerHandlers(void) {
    spInstance = nullptr;
}

void APIServerHandlers::HandleNotFound(void) noexcept {
    std::string response;

    response = std::string("{\"result\": ") +
        std::to_string(E_APIResult::API_RES_UNKNOWN) +
        std::string(", \"msg\": \"Unknown API\"}");

    /* Send */
    spInstance->GenericHandler(response, 200);
}

void APIServerHandlers::HandleKnownURL(void) noexcept {
    std::map<std::string, APIHandler*>::const_iterator it;
    S_HMParamAPIServerError                            error;
    std::string                                        response;
    const char*                                        pageUrl;
    String                                             pageURLStr;

    /* Get the handler */
    pageURLStr = spInstance->_pServer->uri();
    pageUrl    = pageURLStr.c_str();

    it = spInstance->_apiHandlers.find(pageUrl);
    if (spInstance->_apiHandlers.end() == it) {
        error.pAPIURL = pageUrl;
        error.pResponse = &response;
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_API_SERVER_NOT_FOUND, &error);
    }
    else {
        /* Get the potential GET and POST parameters */
        it->second->Handle(response, spInstance->_pServer);
    }

    /* Send */
    spInstance->GenericHandler(response, 200);
}

void APIServerHandlers::GenericHandler(const std::string& krResponse,
                                       const int32_t      kCode) noexcept {
    /* Update page length and send */
    this->_pServer->setContentLength(krResponse.size());
    this->_pServer->send(kCode, "application/json", krResponse.c_str());
}