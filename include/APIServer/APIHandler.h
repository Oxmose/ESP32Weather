/*******************************************************************************
 * @file APIHandler.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 22/12/2025
 *
 * @version 1.0
 *
 * @brief API handler interface.
 *
 * @details API handler interface. The interface defines what a API handler
 * should be able to provide.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __API_HANDLER_H__
#define __API_HANDLER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>       /* Standard strings */
#include <WebServer.h>  /* Web Server services */

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
/** @brief Defines the API result values. */
typedef enum {
    /** @brief No error. */
    API_RES_NO_ERROR = 0,
    /** @brief API not registered. */
    API_RES_NOT_REGISTERED = 1,
    /** @brief Unknown API. */
    API_RES_UNKNOWN = 2,
    /** @brief Unknown wifi settings parameters */
    API_RES_WIFI_SET_UNKNOWN = 3,
    /** @brief Error while setting the wifi settings  */
    API_RES_WIFI_SET_ACTION_ERR = 4,
} E_APIResult;

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
 * @brief The APIHandler interface.
 *
 * @details The APIHandler interface provides the contract that all API
 * handler implementation should provide.
 */
class APIHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief APIHandler destructor.
         *
         * @details APIHandler destructor. The API handler should implement
         * this method.
         */
        virtual ~APIHandler(void) noexcept {};

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
        virtual void Handle(std::string& rResponse, WebServer* pServer)
            noexcept = 0;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /* None */
};

#endif /* #ifndef __API_HANDLER_H__ */