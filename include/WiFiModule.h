/*******************************************************************************
 * @file WifiModule.h
 *
 * @see WifiModule.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief WiFi module driver.
 *
 * @details WiFi module driver. This is a wrapper for the Arduino WiFi module,
 * allowing centralized and uniform configuration and usage of the WiFi module.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __WIFI_MODULE_H__
#define __WIFI_MODULE_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>      /* Standard string */
#include <cstdint>     /* Standard integer definitions */
#include <Errors.h>    /* Error definitions */
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
 * @brief The WiFiModule class.
 *
 * @details The WiFiModule class provides the necessary functions to interract
 * with the WiFi module, create an access point, connect to a network and
 * start a server.
 */
class WiFiModule {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief WiFiModule Constructor to connect to a network.
         *
         * @details WiFiModule Constructor to connect to a network. The
         * constructor will store the SSID and the password of the network to
         * connect to and the actual connection is done using the Start
         * function.
         */
        WiFiModule(const std::string& krSSID, const std::string& krPassword);

        /**
         * @brief Starts the WiFiModule.
         *
         * @details Starts the WiFiModule. When the module is started as AP, it
         * uses the SSID and Password to create the network. When not an AP, the
         * module will connect to the network with the given SSID and password.
         *
         * @param[in] kStartAP Tells if the module should be started as access
         * point or connect to an existing network.
         *
         * @return The functions returns the success or error status.
         */
        E_Return Start(const bool kStartAP) noexcept;

        /**
         * @brief Starts the Web Servers.
         *
         * @details Starts the Web Servers. Two servers are started: the web
         * interface and the API interface. Both use the ports defined in
         * parameters.
         *
         * @param[in] kWebIFacePort The port to use for the web interface.
         * @param[in] kAPIIfacePort The port to use for the API interface.
         *
         * @return The functions returns the success or error status.
         */
        E_Return StartWebServers(const uint16_t kWebIFacePort,
                                 const uint16_t kAPIIfacePort) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Configures the web server.
         *
         * @details Configures the web server. This function will setup the
         * handlers for the webserver various settings.
         *
         * @return The functions returns the success or error status.
         */
        E_Return ConfigureWebServer(void) noexcept;

        /**
         * @brief Configures the API server.
         *
         * @details Configures the API server. This function will setup the
         * handlers for the api server various settings.
         *
         * @return The functions returns the success or error status.
         */
        E_Return ConfigureAPIServer(void) noexcept;

        /** @brief Tells if the module is set as AP or Node */
        bool _isAP;
        /** @brief Stores the AP SSID */
        std::string _ssid;
        /** @brief Stores the AP password */
        std::string _password;
        /** @brief Stores the WiFi module IP address */
        std::string _ipAddress;
        /** @brief Stores the current state of the module */
        bool _isStarted;
        /** @brief Stores the Web Interface server instance. */
        WebServer* _pWebServer;
        /** @brief Stores the API Interface server instance. */
        WebServer* _pAPIServer;
        /** @brief Web handler task handle. */
        TaskHandle_t _pWebServerTask;
        /** @brief API handler task handle. */
        TaskHandle_t _pAPIServerTask;
};

#endif /* #ifndef __WIFI_MODULE_H__ */