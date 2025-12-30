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
#include <string>              /* Standard string */
#include <cstdint>             /* Standard integer definitions */
#include <Errors.h>            /* Error definitions */
#include <WebServer.h>         /* Web server services */
#include <HMReporter.h>        /* HM Reporter abstraction */
#include <SystemState.h>       /* System State services */
#include <WebServerHandlers.h> /* WebServer handlers */
#include <APIServerHandlers.h> /* APIServer handlers */

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
/** @brief Defines the structure containing the settable WiFi setings. */
typedef struct {
    /** @brief WiFI AP mode. */
    bool isAP;
    /** @brief WiFi Static ocnfiguration status. */
    bool isStatic;
    /** @brief WiFi network SSID. */
    std::string ssid;
    /** @brief WiFi network password. */
    std::string password;
    /** @brief WiFi IP address. */
    std::string ip;
    /** @brief WiFi static gateway IP adress. */
    std::string gateway;
    /** @brief WiFi static subnet. */
    std::string subnet;
    /** @brief WiFi primary DNS IP address. */
    std::string primaryDNS;
    /** @brief WiFi secondary DNS IP address. */
    std::string secondaryDNS;
} S_WiFiConfig;

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
/* Forward declaration */
class WiFiModuleHealthReporter;

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
        WiFiModule(void) noexcept;

        /**
         * @brief WiFiModule destructor.
         *
         * @details WiFiModule destructor. Release the used memory.
         */
        ~WiFiModule(void) noexcept;

        /**
         * @brief Starts the WiFiModule.
         *
         * @details Starts the WiFiModule. When the module is started as AP, it
         * uses the SSID and Password to create the network. When not an AP, the
         * module will connect to the network with the given SSID and password.
         *
         * @return The functions returns the success or error status.
         */
        E_Return Start(void) noexcept;

        /**
         * @brief Starts the Web Servers.
         *
         * @details Starts the Web Servers. Two servers are started: the web
         * interface and the API interface. Both use the ports defined in
         * parameters.
         *
         * @return The functions returns the success or error status.
         */
        E_Return StartWebServers(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:

        /**
         * @brief Starts the WiFi module in AP mode.
         *
         * @details Starts the WiFi module in AP mode. The configuration is read
         * from the persistent storage.
         *
         * @return The functions returns the success or error status.
         */
        E_Return StartAP(void) noexcept;

        /**
         * @brief Starts the WiFi module in node mode.
         *
         * @details Starts the WiFi module in node mode. The configuration is
         * read from the persistent storage.
         *
         * @return The functions returns the success or error status.
         */
        E_Return StartNode(void) noexcept;

        /**
         * @brief Configures the different servers.
         *
         * @details Configures the different servers. This function will setup
         * the web and api servers.
         *
         * @return The functions returns the success or error status.
         */
        E_Return ConfigureServers(void) noexcept;

        /**
         * @brief Configures the different server tasks.
         *
         * @details Configures the different server tasks. This function will
         * setup the web and api server tasks.
         *
         * @return The functions returns the success or error status.
         */
        E_Return ConfigureServerTasks(void) noexcept;

        /** @brief Tells if the module is set as AP or Node */
        bool _isAP;
        /** @brief Stores the AP SSID */
        std::string _ssid;
        /** @brief Stores the AP password */
        std::string _password;
        /** @brief Stores the WiFi module IP address */
        std::string _ipAddress;
        /** @brief Stores the static configuration state */
        bool _isStatic;
        /** @brief Stores the static configuration gateway */
        std::string _staticGatewayIP;
        /** @brief Stores the static configuration subnet */
        std::string _staticSubnet;
        /** @brief Stores the static configuration primary DNS */
        std::string _staticPrimaryDNSIP;
        /** @brief Stores the static configuration secondary DNS */
        std::string _staticSecondaryDNSIP;

        /** @brief Stores the current state of the module */
        bool _isStarted;

        /** @brief Stores the Web Interface server instance. */
        WebServer* _pWebServer;
        /** @brief Stores the API Interface server instance. */
        WebServer* _pAPIServer;
        /** @brief Stores the web interface port. */
        uint16_t _webPort;
        /** @brief Stores the API interface port. */
        uint16_t _apiPort;

        /** @brief Stores the Web Interface server handlers instance. */
        WebServerHandlers* _pWebServerHandler;
        /** @brief Stores the API Interface server handlers instance. */
        APIServerHandlers* _pAPIServerHandler;

        /** @brief Web handler task handle. */
        TaskHandle_t _pWebServerTask;
        /** @brief API handler task handle. */
        TaskHandle_t _pAPIServerTask;

        /** @brief Stores the Health Reporter for the WiFiModule */
        WiFiModuleHealthReporter* _pReporter;
        /** @brief The health report ID. */
        uint32_t _reporterId;

        /** @brief Stores the system state object. */
        SystemState* _pSysState;

    /********************* FRIEND DECLARATIONS *********************/
    friend WiFiModuleHealthReporter;
};

/**
 * @brief WiFiModule Health Reporter class.
 *
 * @details WiFiModule Health Reporter class. This class is used to check the
 * WiFiModule continuous health and report the health status of the WiFiModule.
 */
class WiFiModuleHealthReporter : public HMReporter {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Initializes the Health Reporter.
         *
         * @details Initializes the Health Reporter. The next check period
         * will be started after the object initialization.
         *
         * @param[in] krParam The health reporter parameters.
         * @param[in] pModule The WiFiModule being monitored.
         *
         */
        WiFiModuleHealthReporter(const S_HMReporterParam& krParam,
                                 WiFiModule*              pModule) noexcept;

        /**
         * @brief WiFiModuleHealthReporter Interface Destructor.
         *
         * @details WiFiModuleHealthReporter Interface Destructor. Release the
         * used memory.
         */
        virtual ~WiFiModuleHealthReporter(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /**
         * @brief Action executed on degraded health.
         *
         * @details Action executed on degraded health. Must be implemented by
         * the inherited class.
         */
        virtual void OnDegraded(void) noexcept;
        /**
         * @brief Action executed on unhealthy health.
         *
         * @details Action executed on unhealthy health. Must be implemented by
         * the inherited class.
         */
        virtual void OnUnhealthy(void) noexcept;
        /**
         * @brief Checks the current health for the report.
         *
         * @details  Checks the current health for the report. Must be
         * implemented by the inherited class.
         *
         * @return The function returns the check status: passed is True,
         * otherwise False.
         */
        virtual bool PerformCheck(void) noexcept;

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief The module monitored by the reporter. */
        WiFiModule* _pModule;
};

#endif /* #ifndef __WIFI_MODULE_H__ */