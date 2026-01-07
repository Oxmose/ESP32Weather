/*******************************************************************************
 * @file PinAPIHandler.cpp
 *
 * @see PinAPIHandler.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 23/12/2025
 *
 * @version 1.0
 *
 * @brief Ping API handler.
 *
 * @details Ping API handler. This file defines the Ping API handler
 * used to handle and generate the ping response.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>       /* Standard string */
#include <Logger.h>     /* Logger services */
#include <Errors.h>     /* Errors definitions */
#include <WebServer.h>  /* Web Server services */
#include <APIHandler.h> /* API Handler interface */

/* Header file */
#include <PingAPIHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

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
PingAPIHandler::~PingAPIHandler(void) noexcept {
    PANIC("Tried to destroy the Ping API handler.\n");
}

void PingAPIHandler::Handle(std::string& rResponse, WebServer* pServer)
noexcept {
    (void)pServer;

    LOG_DEBUG("Handling Ping API.\n");

    rResponse = "{\"result\": " +
        std::to_string(E_APIResult::API_RES_NO_ERROR) +
        ", \"msg\": \"Pong\"}";
}