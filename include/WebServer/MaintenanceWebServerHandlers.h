/*******************************************************************************
 * @file MaintenanceWebServerHandlers.h
 *
 * @see MaintenanceWebServerHandlers.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Maintenance Web Server URL handlers.
 *
 * @details Maintenance Web Server URL handlers. This file defines all the
 * handlers used for the maintenance web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __MAINTENANCE_WEB_SERVER_HANDLERS_H__
#define __MAINTENANCE_WEB_SERVER_HANDLERS_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>        /* Standard strings */
#include <cstdint>       /* Standard integer definitions */
#include <WebServer.h>   /* Web server services */

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
 * @brief The MaintenanceWebServerHandlers class.
 *
 * @details The MaintenanceWebServerHandlers class provides the necessary
 * functions to handle URL requests and respond through the maintenance web
 * servers.
 */
class MaintenanceWebServerHandlers {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Destroys a MaintenanceWebServerHandlers.
         *
         * @details Destroys a MaintenanceWebServerHandlers. Since only one
         * object is allowed in the firmware, the destructor will generate a
         * critical error.
         */
        MaintenanceWebServerHandlers(WebServer* pServer) noexcept;

        /**
         * @brief WebServerHandlers destructor.
         *
         * @details WebServerHandlers destructor. Releases the used resources.
         */
        ~MaintenanceWebServerHandlers(void) noexcept;


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
         * @details Handles the index URL. Provides the information in
         * maintenance mode.
         */
        static void HandleIndex(void) noexcept;

        /**
         * @brief Handles the reboot URL.
         *
         * @details Handles the reboot URL. Sets the new execution mode and
         * reboots.
         */
        static void HandleReboot(void) noexcept;

        /**
         * @brief Handles the RAM logs loading request URL.
         *
         * @details Handles the RAM logs loading request URL. Respond with new
         * logs when available.
         */
        static void HandleRamLoad(void) noexcept;

        /**
         * @brief Handles the Journal logs loading request URL.
         *
         * @details Handles the Journal logs loading request URL. Respond with
         * new logs when available.
         */
        static void HandleJournalLoad(void) noexcept;

        /**
         * @brief Handles the log clear request URL.
         *
         * @details Handles the log clear request URL. Respond with an empty
         * response.
         */
        static void HandleClearLogs(void) noexcept;

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
         * @brief Creates the page Script section.
         *
         * @details Creates the page Script section. This will fill the string
         * buffer.
         *
         * @param[out] rHeaderStr The string buffer that receives the Script.
         */
        void GeneratePageScripts(std::string& rHeaderStr) const noexcept;


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


        /**
         * @brief Formats the firmware logs.
         *
         * @details Formats the firmware logs to be displayed in an HLTM page.
         *
         * @param[out] rPage The page buffer to fill with the logs.
         */
        void GetFormatedLogs(std::string& rPage) const noexcept;

        /** @brief Stores the WebServer used by the handlers. */
        WebServer* _pServer;
};

#endif /* #ifndef __MAINTENANCE_WEB_SERVER_HANDLERS_H__ */