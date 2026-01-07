/*******************************************************************************
 * @file PinAPIHandler.h
 *
 * @see PinAPIHandler.cpp
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

#ifndef __PING_API_HANDLER_H__
#define __PING_API_HANDLER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>       /* Standard strings */
#include <WebServer.h>  /* Web Server services */
#include <APIHandler.h> /* API Handler interface */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
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
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASSES
 ******************************************************************************/

 /**
 * @brief The PingAPIHandler class.
 *
 * @details The PingAPIHandler class provides the necessary functions to handle
 * a Ping call through the API.
 */
class PingAPIHandler : public APIHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Destroys a PingAPIHandler.
         *
         * @details Destroys a PingAPIHandler. Since only one object is allowed
         * in the firmware, the destructor will generate a critical error.
         */
        virtual ~PingAPIHandler(void) noexcept;

        /**
         * @brief Handle the API call.
         *
         * @details Generate the page. The function should generate API response
         * and process the API call.
         *
         * @param[out] rResponse The response to the API call.
         * @param[in] pServer The server that received the call. Used to
         * retrieve the call parameters.
         */
        virtual void Handle(std::string& rResponse, WebServer* pServer) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /* None */
};

#endif /* #ifndef __PING_API_HANDLER_H__ */