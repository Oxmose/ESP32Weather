/*******************************************************************************
 * @file WebServerHandlers.h
 *
 * @see WebServerHandlers.cpp
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

#ifndef __WEB_SERVER_HANDLERS_H__
#define __WEB_SERVER_HANDLERS_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <map>           /* Standard maps */
#include <Errors.h>      /* Errors definitions */
#include <Arduino.h>     /* Arduino Framework */
#include <WebServer.h>   /* Web server services */
#include <PageHandler.h> /* Page Handlers */

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
 * @brief The WebServerHandlers class.
 *
 * @details The WebServerHandlers class provides the necessary functions to
 * handle URL requests and respond through the web servers.
 */
class WebServerHandlers {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief WebServerHandlers constructor.
         *
         * @details WebServerHandlers constructor. This sets the webserver used
         * by the handlers to handle requests.
         *
         * @param[in] pServer The server to use.
         */
        WebServerHandlers(WebServer* pServer) noexcept;

        /**
         * @brief WebServerHandlers destructor.
         *
         * @details WebServerHandlers destructor. Releases the used resources.
         */
        ~WebServerHandlers(void) noexcept;


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
         * @brief Creates the page header.
         *
         * @details Creates the page header. This will fill the string buffer
         * and use the appropriate page title.
         *
         * @param[out] rHeaderStr The string buffer that receives the header.
         * @param[in] krTitle The page title to set.
         */
        void GetPageHeader(std::string&       rHeaderStr,
                           const std::string& krTitle) const noexcept;
        /**
         * @brief Creates the page footer.
         *
         * @details Creates the page footer. This will fill the string buffer.
         *
         * @param[out] rFooterStr The string buffer that receives the footer.
         */
        void GetPageFooter(std::string& rFooterStr) const noexcept;

        /**
         * @brief Creates the page CSS section.
         *
         * @details Creates the page CSS section. This will fill the string
         * buffer.
         *
         * @param[out] rHeaderStr The string buffer that receives the CSS.
         */
        void GeneratePageCSS(std::string& rHeaderStr) const noexcept;

        /**
         * @brief Generic page handler.
         *
         * @details Generic page handler. The handler will send the page
         * to the requesting server.
         *
         * @param[in] kpPage The page to send.
         * @param[in] kCode The code to respond.
         */
        void GenericHandler(const std::string& krPage,
                            const int32_t      kCode) noexcept;

        /** @brief Stores the WebServer used by the handlers. */
        WebServer* _pServer;

        /** @brief Stores the WebServer lock.  */
        SemaphoreHandle_t _lock;

        /** @brief Stores the registered handlers for the pages. */
        std::map<std::string, PageHandler*> _pageHandlers;
};

#endif /* #ifndef __WEB_SERVER_HANDLERS_H__ */