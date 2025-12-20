/*******************************************************************************
 * @file WifiModule.cpp
 *
 * @see WifiModule.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>               /* BSP Layer */
#include <string>              /* Standard string */
#include <WiFi.h>              /* WiFi services */
#include <cstdint>             /* Standard integer definitions */
#include <Errors.h>            /* Error definitions */
#include <Logger.h>            /* Logger services */
#include <Timeout.h>           /* Timeout manager */
#include <HealthMonitor.h>     /* HM services */
#include <WebServerHandlers.h> /* WebServer URL handlers */
#include <APIServerHandlers.h> /* APIServer URL handlers */

/* Header file */
#include <WiFiModule.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/**
 * @brief Defines the maximum number of parallel connections that can be
 * used.
 */
#define WIFI_MODULE_MAX_CONN 10

/** @brief Connection timeout in nanoseconds. */
#define NODE_CONNECT_TIMEOUT_NS 15000000000 // 15 seconds

/** @brief Defines the Web Server Task name. */
#define WEB_SERVER_TASK_NAME "WEB-SRV_TASK"
/** @brief Defines the Web Server Task stack size in bytes. */
#define WEB_SERVER_TASK_STACK 4096
/** @brief Defines the Web Server Task priority. */
#define WEB_SERVER_TASK_PRIO (configMAX_PRIORITIES - 2)
/** @brief Defines the Web Server Task mapped core. */
#define WEB_SERVER_TASK_CORE 1
/** @brief Defines the API Server Task name. */
#define API_SERVER_TASK_NAME "API-SRV_TASK"
/** @brief Defines the API Server Task stack size in bytes. */
#define API_SERVER_TASK_STACK 4096
/** @brief Defines the API Server Task priority. */
#define API_SERVER_TASK_PRIO (configMAX_PRIORITIES - 2)
/** @brief Defines the API Server Task mapped core. */
#define API_SERVER_TASK_CORE 1

/** @brief Defines the WiFiModule Health Report period in nanoseconds. */
#define WIFI_MODULE_HM_REPORT_PERIOD_NS 1000000000ULL
/** @brief Defines the WiFiModule Health Report degraded threshold. */
#define WIFI_MODULE_HM_REPORT_FAIL_TO_DEGRADE 5
/** @brief Defines the WiFiModule Health Report unhealthy threshold. */
#define WIFI_MODULE_HM_REPORT_FAIL_TO_UNHEALTHY 10
/** @brief Defines the WiFiModule Health Report name. */
#define WIFI_MODULE_HM_REPORT_NAME "HM_WIFIMODULE"

/** @brief Mininal accepted RSSI */
#define WIFI_MIN_RSSI 10

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
/**
 * @brief Server handle client routine.
 *
 * @details Server handle client routine. This routine will handle new clients
 * to the server.
 *
 * @param[in] pServer The server to use.
 */
static void WebServerHandleRoutine(void* pServer);

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
static void WebServerHandleRoutine(void* pServer) {
    while (true) {
        ((WebServer*)pServer)->handleClient();
    }
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/

WiFiModule::WiFiModule(const std::string& krSSID,
                       const std::string& krPassword) noexcept
{
    HealthMonitor* pHM;
    E_Return       result;

    /* Setup the WiFi service as Access Point with the provided SSID and
    * password
    */
    this->_isAP = true;
    this->_ssid = krSSID;
    this->_password = krPassword;

    /* Set as not started */
    this->_isStarted = false;

    this->_pWebServer = nullptr;
    this->_pAPIServer = nullptr;

    /* Add the health reporter */
    this->_pReporter = new WiFiModuleHealthReporter(
        S_HMReporterParam {
            WIFI_MODULE_HM_REPORT_PERIOD_NS,
            WIFI_MODULE_HM_REPORT_FAIL_TO_DEGRADE,
            WIFI_MODULE_HM_REPORT_FAIL_TO_UNHEALTHY,
            WIFI_MODULE_HM_REPORT_NAME
        },
        this
    );
    if (nullptr == this->_pReporter) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WIFI_CREATE,
            (void*)0
        );
    }
    pHM = HealthMonitor::GetInstance();
    result = pHM->AddReporter(this->_pReporter, this->_reporterId);
    if (E_Return::NO_ERROR != result) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_HM_ADD_ACTION,
            (void*)result
        );
    }
}


WiFiModule::~WiFiModule(void) noexcept {
    if (nullptr != this->_pReporter) {
        delete this->_pReporter;
    }
}

E_Return WiFiModule::Start(const bool kStartAP) noexcept {
    bool        retVal;
    E_Return    error;
    Timeout     connTimeout(NODE_CONNECT_TIMEOUT_NS);

    error = E_Return::NO_ERROR;

    if (!this->_isStarted) {

        this->_isAP = kStartAP;

        /* Disable persistence */
        WiFi.persistent(false);

        /* Check the AP Type */
        if (this->_isAP) {
            /* Set AP */
            retVal = WiFi.softAP(
                this->_ssid.c_str(),
                this->_password.c_str(),
                1,
                0,
                WIFI_MODULE_MAX_CONN
            );

            /* On Success, get the IP address */
            if (retVal) {
                this->_ipAddress = WiFi.softAPIP().toString().c_str();

                LOG_INFO("Started the WiFi Module in AP mode\n");
                LOG_INFO("    SSID: %s\n", this->_ssid.c_str());
                LOG_INFO("    Password: %s\n", this->_password.c_str());
                LOG_INFO("    IP Address: %s\n", this->_ipAddress.c_str());

                /* Success */
                this->_isStarted = true;
            }
            else {
                LOG_ERROR("Failed to create the Access Point\n");

                error = E_Return::ERR_WIFI_CONN;
            }
        }
        else {
            LOG_INFO("Connecting to network: %s.\n", this->_ssid.c_str());

            /* Connect to existing network */
            WiFi.begin(this->_ssid.c_str(), this->_password.c_str());
            connTimeout.Tick();
            while (WL_CONNECTED != WiFi.status() && !connTimeout.Check()) {
                HWManager::DelayExecNs(500000000);
            }

            if (WL_CONNECTED == WiFi.status()) {
                this->_ipAddress = WiFi.localIP().toString().c_str();

                LOG_INFO("Connected the WiFi Module to network\n");
                LOG_INFO("    SSID: %s\n", this->_ssid.c_str());
                LOG_INFO("    IP Address: %s\n", this->_ipAddress.c_str());

                /* Success */
                this->_isStarted = true;
            }
            else {
                LOG_ERROR(
                    "Failed to connect to network %s.\n",
                    this->_ssid.c_str()
                );
                error = E_Return::ERR_WIFI_CONN;
            }
        }
    }

    return error;
}

E_Return WiFiModule::StartWebServers(const uint16_t kWebIFacePort,
                                     const uint16_t kAPIIfacePort) noexcept {
    E_Return   result;

    /* Create the web server */
    if (nullptr != this->_pWebServer) {
        this->_pWebServer->stop();
        delete this->_pWebServer;
        this->_pWebServer = nullptr;
    }
    this->_pWebServer = new WebServer(kWebIFacePort);

    if (nullptr != this->_pWebServer) {
        /* Create the API server */
        if (nullptr != this->_pAPIServer) {
            this->_pAPIServer->stop();
            delete this->_pAPIServer;
            this->_pAPIServer = nullptr;
        }
        this->_pAPIServer = new WebServer(kAPIIfacePort);

        if (nullptr == this->_pAPIServer) {
            LOG_ERROR("Failed to instanciate the API server.\n");
            result = E_Return::ERR_MEMORY;
        }
        else {
            result = E_Return::NO_ERROR;
        }
    }
    else {
        LOG_ERROR("Failed to instanciate the web server.\n");
        result = E_Return::ERR_MEMORY;
    }

    /* If everything went well, configure the servers */
    if (E_Return::NO_ERROR == result) {
        result = ConfigureServers();
    }

    /* If everything went well, start the servers */
    if (E_Return::NO_ERROR == result) {
        this->_pWebServer->begin();
        this->_pAPIServer->begin();

        result = ConfigureServerTasks();
    }

    if (E_Return::NO_ERROR != result) {
        delete this->_pAPIServer;
        delete this->_pWebServer;
    }

    return result;
}

E_Return WiFiModule::ConfigureWebServer(void) noexcept {
    E_Return result;

    /* Initialize the handlers */
    result = WebServerHandlers::Init(this->_pWebServer);

    return result;
}

E_Return WiFiModule::ConfigureAPIServer(void) noexcept {

    E_Return result;

    /* Initialize the handlers */
    result = APIServerHandlers::Init(this->_pAPIServer);

    return result;
}

E_Return WiFiModule::ConfigureServers(void) noexcept {
    E_Return result;

    result = ConfigureWebServer();
    if (E_Return::NO_ERROR == result) {
        result = ConfigureAPIServer();
        if (E_Return::NO_ERROR != result) {
            LOG_ERROR("Failed to configure the API server.\n");
        }
    }
    else {
        LOG_ERROR("Failed to configure the web server.\n");
    }

    return result;
}

E_Return WiFiModule::ConfigureServerTasks(void) noexcept {
    E_Return  result;
    BaseType_t createRes;

    /* Start the web server task */
    createRes = xTaskCreatePinnedToCore(
        WebServerHandleRoutine,
        WEB_SERVER_TASK_NAME,
        WEB_SERVER_TASK_STACK,
        this->_pWebServer,
        WEB_SERVER_TASK_PRIO,
        &this->_pWebServerTask,
        WEB_SERVER_TASK_CORE
    );
    if (pdPASS == createRes) {
        /* Start the API server task */
        createRes = xTaskCreatePinnedToCore(
            WebServerHandleRoutine,
            API_SERVER_TASK_NAME,
            API_SERVER_TASK_STACK,
            this->_pAPIServer,
            API_SERVER_TASK_PRIO,
            &this->_pAPIServerTask,
            API_SERVER_TASK_CORE
        );
        if (pdPASS != createRes) {
            /* Delete the first task */
            vTaskDelete(this->_pWebServerTask);

            LOG_ERROR("Failed to initialize the API handler task.\n");
            result = E_Return::ERR_API_SERVER_TASK;
        }
        else {
            result = E_Return::NO_ERROR;
        }
    }
    else {
        LOG_ERROR("Failed to initialize the web handler task.\n");
        result = E_Return::ERR_WEB_SERVER_TASK;
    }

    return result;
}

WiFiModuleHealthReporter::WiFiModuleHealthReporter(
    const S_HMReporterParam& krParam,
    WiFiModule*              pModule) noexcept :
    HMReporter(krParam) {
    this->_pModule = pModule;
}

WiFiModuleHealthReporter::~WiFiModuleHealthReporter(void) noexcept {
    /* Nothing to do */
}

void WiFiModuleHealthReporter::OnDegraded(void) noexcept {
    LOG_ERROR(
        "WiFi module is degraded, expecting to recover before unhealthy"
        " status.\n"
    );
}

void WiFiModuleHealthReporter::OnUnhealthy(void) noexcept {
    E_Return result;

    LOG_ERROR(
        "WiFi module is unhealthy, restarting.\n"
    )

    /* Disable and restart */
    if (this->_pModule->_isAP) {
        WiFi.softAPdisconnect();
    }
    else {
        WiFi.disconnect();
    }
    this->_pModule->_isStarted = false;

    /* Restart */
    result = this->_pModule->Start(this->_pModule->_isAP);
    if (E_Return::NO_ERROR != result) {
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_WIFI_CREATE,
            (void*)result
        );
    }
}

bool WiFiModuleHealthReporter::PerformCheck(void) noexcept {
    bool checkResult;

    checkResult = true;

    if (this->_pModule->_isStarted && !this->_pModule->_isAP) {

        /* Check WiFi status */
        checkResult &= WiFi.isConnected();

        /* Check WiFi RSSI */
        if (checkResult) {
            checkResult &= WiFi.RSSI() >= WIFI_MIN_RSSI;
        }
    }
    else {
        checkResult &= this->_pModule->_isStarted;
    }

    return checkResult;
}