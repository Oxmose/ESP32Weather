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
#include <Errors.h>    /* Errors definitions */
#include <Arduino.h>   /* Arduino Framework */
#include <WebServer.h> /* Web server services */

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
         * @brief Initializes the handlers.
         *
         * @details Initializes the handlers. This sets the webserver used by
         * the handlers to handle requests.
         *
         * @param[in] pServer The server to use.
         *
         * @return The function returns the success or error status.
         */
        static E_Return Init(WebServer* pServer) noexcept;

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
         * @brief Handles the index URL.
         *
         * @details Handles the index URL.
         */
        static void HandleIndex(void) noexcept;

        /**
         * @brief Creates the page header.
         *
         * @details Creates the page header. This will fill the string buffer
         * and use the appropriate page title.
         *
         * @param[out] rHeaderStr The string buffer that receives the header.
         * @param[in] krTitle The page title to set.
         */
        static void GetPageHeader(std::string&       rHeaderStr,
                                  const std::string& krTitle) noexcept;
        /**
         * @brief Creates the page footer.
         *
         * @details Creates the page footer. This will fill the string buffer.
         *
         * @param[out] rFooterStr The string buffer that receives the footer.
         */
        static void GetPageFooter(std::string& rFooterStr);

        /**
         * @brief Generic page handler.
         *
         * @details Generic page handler. The handler will send the page
         * to the requesting server.
         *
         * @param[in] kpPage The page to send.
         * @param[in] kCode The code to respond.
         */
        static void GenericHandler(const std::string& krPage,
                                   const int32_t      kCode) noexcept;

        /** @brief Stores the WebServer used by the handlers. */
        static WebServer* _SPSERVER;

        /** @brief Stores the WebServer lock.  */
        static SemaphoreHandle_t _SSERVER_LOCK;
};

#endif /* #ifndef __API_SERVER_HANDLERS_H__ */