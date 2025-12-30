/*******************************************************************************
 * @file APIServerHandlers.h
 *
 * @see APIServerHandlers.cpp
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

#ifndef __API_SERVER_HANDLERS_H__
#define __API_SERVER_HANDLERS_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <map>           /* Standard maps */
#include <Errors.h>      /* Errors definitions */
#include <Arduino.h>     /* Arduino Framework */
#include <WebServer.h>   /* Web server services */
#include <APIHandler.h>  /* API Handlers */

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
 * @brief The APIServerHandlers class.
 *
 * @details The APIServerHandlers class provides the necessary functions to
 * handle URL requests and respond through the apiweb servers.
 */
class APIServerHandlers {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:

        /**
         * @brief APIServerHandlers constructor.
         *
         * @details APIServerHandlers constructor. This sets the webserver used
         * by the handlers to handle requests.
         *
         * @param[in] pServer The server to use.
         */
        APIServerHandlers(WebServer* pServer) noexcept;

        /**
         * @brief APIServerHandlers destructor.
         *
         * @details APIServerHandlers destructor. Releases the used resources.
         */
        ~APIServerHandlers(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Handles the invalid URLs.
         *
         * @details Handles the invalid URLs.
         */
        static void HandleNotFound(void) noexcept;

        /**
         * @brief Handles the registered URLs.
         *
         * @details Handles the registered URLs. If the URL is not found in the
         * handlers table, the HM is notified.
         */
        static void HandleKnownURL(void) noexcept;

        /**
         * @brief Generic API handler.
         *
         * @details Generic API handler. The handler will send the reponse
         * to the requesting server.
         *
         * @param[in] krResponse The reponse to send.
         * @param[in] kCode The code to respond.
         */
        void GenericHandler(const std::string& krResponse,
                            const int32_t      kCode) noexcept;

        /** @brief Stores the WebServer used by the handlers. */
        WebServer* _pServer;

        /** @brief Stores the WebServer lock.  */
        SemaphoreHandle_t _lock;

        /** @brief Stores the registered handlers for the API. */
        std::map<std::string, APIHandler*> _apiHandlers;
};

#endif /* #ifndef __API_SERVER_HANDLERS_H__ */