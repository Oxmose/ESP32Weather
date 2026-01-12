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
#include <Errors.h>        /* Errors definitions */
#include <Logger.h>        /* Logger services */
#include <Arduino.h>       /* Arduino Framework */
#include <Settings.h>      /* Settings services */
#include <WebServer.h>     /* Web server services */

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
#define CREATE_NEW_HANDLER(URL, HANDLER_CLASS, HANDLER_OBJ) {               \
    HANDLER_OBJ = new HANDLER_CLASS();                                      \
    if (nullptr == HANDLER_OBJ) {                                           \
        PANIC("Failed to allocate a new handler for API %s.\n", URL)        \
    }                                                                       \
    this->_apiHandlers.emplace(URL, HANDLER_OBJ);                           \
    this->_pServer->on(URL, HTTP_POST, HandleKnownURL);                     \
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
        PANIC(
            "Tried to re-create a new API Server handlers manager instance.\n"
        );
    }

    this->_pServer = pServer;

    /* Create the handlers */
    CREATE_NEW_HANDLER(API_URL_PING, PingAPIHandler, pNewHandler);
    CREATE_NEW_HANDLER(API_URL_WIFI, WiFiSettingAPIHandler, pNewHandler);

    /* Configure the not found handler */
    this->_pServer->onNotFound(HandleNotFound);

    /* Set the instance */
    spInstance = this;

    LOG_DEBUG("Created the API Server handlers manager.\n");
}

APIServerHandlers::~APIServerHandlers(void) {
    PANIC("Tried to destroy the API Server handlers manager.\n");
}

void APIServerHandlers::HandleNotFound(void) noexcept {
    std::string response;

    LOG_DEBUG(
        "Handling API not found: %s\n",
        spInstance->_pServer->uri().c_str()
    );

    response = "{\"result\": ";
    response += std::to_string(E_APIResult::API_RES_UNKNOWN);
    response += ", \"msg\": \"Unknown API: ";
    response += spInstance->_pServer->uri().c_str();
    response += "\"}";

    /* Send */
    spInstance->GenericHandler(response, 404);
}

void APIServerHandlers::HandleKnownURL(void) noexcept {
    std::unordered_map<std::string, APIHandler*>::const_iterator it;
    std::string                                                  response;
    const char*                                                  pageUrl;
    String                                                       pageURLStr;
    int32_t                                                      code;

    LOG_DEBUG(
        "Handling API: %s\n",
        spInstance->_pServer->uri().c_str()
    );

    /* Get the handler */
    pageURLStr = spInstance->_pServer->uri();
    pageUrl = pageURLStr.c_str();

    it = spInstance->_apiHandlers.find(pageUrl);
    if (spInstance->_apiHandlers.end() == it) {
        response = "{\"result\": ";
        response += std::to_string(E_APIResult::API_RES_NOT_REGISTERED);
        response += ", \"msg\": \"Non-registered API: ";
        response += spInstance->_pServer->uri().c_str();
        response += "\"}";

        LOG_ERROR(
            "API URL not registered: %s\n",
            spInstance->_pServer->uri().c_str()
        );

        code = 500;
    }
    else {
        /* Get the potential GET and POST parameters */
        it->second->Handle(response, spInstance->_pServer);
        code = 200;
    }

    /* Send */
    spInstance->GenericHandler(response, code);
}

void APIServerHandlers::GenericHandler(const std::string& krResponse,
                                       const int32_t      kCode) noexcept {
    /* Update page length and send */
    this->_pServer->setContentLength(krResponse.size());
    this->_pServer->send(kCode, "application/json", krResponse.c_str());
}