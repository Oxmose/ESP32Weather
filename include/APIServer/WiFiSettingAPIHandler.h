/*******************************************************************************
 * @file WiFiSettingAPIHandler.h
 * 
 * @see WiFiSettingAPIHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 24/12/2025
 *
 * @version 1.0
 *
 * @brief WiFi Settings API handler.
 *
 * @details WiFi Settings API handler. This file defines the WiFi Settings API 
 * handler used to handle all calls related to WiFi settings.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __WIFI_SETTINGS_API_HANDLER_H__
#define __WIFI_SETTINGS_API_HANDLER_H__

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
 * @brief The WiFiSettingAPIHandler class.
 *
 * @details The WiFiSettingAPIHandler class provides the necessary functions to handle
 * a WiFi settings call through the API.
 */
class WiFiSettingAPIHandler : public APIHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief APIHandler destructor.
         *
         * @details APIHandler destructor. The API handler should implement
         * this method.
         */
        virtual ~WiFiSettingAPIHandler(void) noexcept;

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
        /** 
         * @brief Retrieves the WiFi settings.
         * 
         * @details Retrieves the WiFi settings and fills the API response with 
         * the registered settings.
         * 
         * @param[out] rResponse The JSON buffer to fill with the WiFi settings.
         */
        void GetWiFiSettings(std::string& rResponse) const noexcept;

        /** 
         * @brief Updates the WiFi settings.
         * 
         * @details Updates the WiFi settings and fills the API response with 
         * the update status settings.
         * 
         * @param[in] pServer The server that received the request. Used to 
         * retrieve the settings values.
         * @param[out] rResponse The JSON buffer to fill with the WiFi settings.
         */
        void SetWiFiSettings(WebServer* pServer, std::string& rResponse) const 
        noexcept;
};

#endif /* #ifndef __WIFI_SETTINGS_API_HANDLER_H__ */