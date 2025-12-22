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
#include <Settings.h>          /* Settings manager */
#include <SystemState.h>       /* System state provider */
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

/**
 * @brief Gets the specified settings.
 *
 * @details Gets the specified settings. An error to the HM is trigered in case
 * of failure and the default value is provided.
 *
 * @param[in] NAME The name of the settings to get.
 * @param[out] BUFFER The buffer that receives the setting value.
 * @param[in] SIZE The size of the setting.
 * @param[out] ERROR The error buffer.
 * @param[in] SETTINGS The settings object to use to retrieve the setting.
 * @param[out] HM_PARAM The HM parameters used to retrieve the default value.
 */
#define GET_SETTING(NAME, BUFFER, SIZE, ERROR, SETTINGS, HM_PARAM) {    \
    ERROR = SETTINGS->GetSettings(NAME, (uint8_t*)BUFFER, SIZE);        \
    if (E_Return::NO_ERROR != ERROR) {                                  \
        HM_PARAM.kpSettingName = NAME;                                  \
        HM_PARAM.pSettingBuffer = (void*)BUFFER;                        \
        HM_PARAM.settingBufferSize = SIZE;                              \
        HealthMonitor::GetInstance()->ReportHM(                         \
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,                          \
            &HM_PARAM                                                   \
        );                                                              \
    }                                                                   \
}


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

WiFiModule::WiFiModule(void) noexcept
{
    HealthMonitor* pHM;
    E_Return       result;

    /* Setup the WiFi service as Access Point with the provided SSID and
    * password
    */
    this->_isAP = true;
    this->_isStatic = false;
    this->_ssid = "";
    this->_password = "";

    /* Set as not started */
    this->_isStarted = false;

    this->_pWebServer = nullptr;
    this->_pAPIServer = nullptr;

    this->_pSysState = SystemState::GetInstance();

    pHM = HealthMonitor::GetInstance();

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
        pHM->ReportHM(
            E_HMEvent::HM_EVENT_WIFI_CREATE,
            (void*)0
        );
    }
    result = pHM->AddReporter(this->_pReporter, this->_reporterId);
    if (E_Return::NO_ERROR != result) {
        pHM->ReportHM(
            E_HMEvent::HM_EVENT_HM_ADD_ACTION,
            (void*)result
        );
    }

    /* Disable persistence */
    WiFi.persistent(false);
}

WiFiModule::~WiFiModule(void) noexcept {
    HealthMonitor* pHM;
    E_Return       result;

    if (nullptr != this->_pReporter) {
        pHM = HealthMonitor::GetInstance();
        result = pHM->RemoveReporter(this->_reporterId);
        if (E_Return::NO_ERROR != result) {
            pHM->ReportHM(
                E_HMEvent::HM_EVENT_HM_REMOVE_ACTION,
                (void*)result
            );
        }

        delete this->_pReporter;
    }
}

E_Return WiFiModule::Start(void) noexcept {
    E_Return               error;
    Settings*              pSettings;
    S_HMParamSettingAccess hmAccessParam;
    char                   networkSSID[SETTING_NODE_SSID_LENGTH + 1];
    char                   networkPassword[SETTING_NODE_PASS_LENGTH + 1];
    char                   ipBuffer[SETTING_ADDR_LENGTH + 1];

    if (!this->_isStarted) {
        pSettings = Settings::GetInstance();
        /* Check if we should be AP */
        GET_SETTING(
            SETTING_IS_AP,
            &this->_isAP,
            sizeof(bool),
            error,
            pSettings,
            hmAccessParam
        );


        /* Check the AP Type */
        if (!this->_isAP) {
            memset(networkSSID, 0, SETTING_NODE_SSID_LENGTH + 1);
            memset(networkPassword, 0, SETTING_NODE_PASS_LENGTH + 1);
            memset(ipBuffer, 0, SETTING_ADDR_LENGTH + 1);

            /* Get the node SSID */
            GET_SETTING(SETTING_NODE_SSID, networkSSID,
                SETTING_NODE_SSID_LENGTH, error, pSettings, hmAccessParam);

            /* Get the node password */
            GET_SETTING(SETTING_NODE_PASS, networkPassword,
                SETTING_NODE_PASS_LENGTH, error, pSettings, hmAccessParam);

            /* Get the static configuration */
            GET_SETTING(SETTING_NODE_STATIC, &this->_isStatic, sizeof(bool),
                error, pSettings, hmAccessParam);
            GET_SETTING(SETTING_NODE_ST_IP, ipBuffer, SETTING_ADDR_LENGTH,
                error, pSettings, hmAccessParam);
            this->_ipAddress = ipBuffer;
            GET_SETTING(SETTING_NODE_ST_GATE, ipBuffer, SETTING_ADDR_LENGTH,
                error, pSettings, hmAccessParam);
            this->_staticGatewayIP = ipBuffer;
            GET_SETTING(SETTING_NODE_ST_SUBNET, ipBuffer, SETTING_ADDR_LENGTH,
                error, pSettings, hmAccessParam);
            this->_staticSubnet = ipBuffer;
            GET_SETTING(SETTING_NODE_ST_PDNS, ipBuffer, SETTING_ADDR_LENGTH,
                error, pSettings, hmAccessParam);
            this->_staticPrimaryDNSIP = ipBuffer;
            GET_SETTING(SETTING_NODE_ST_SDNS, ipBuffer, SETTING_ADDR_LENGTH,
                error, pSettings, hmAccessParam);
            this->_staticSecondaryDNSIP = ipBuffer;

            /* Start the node */
            error = StartNode();
        }
        else {
            this->_ssid = HWManager::GetHWUID();
            this->_password = HWManager::GetMacAddress();
            error = StartAP();
        }

        if (E_Return::NO_ERROR == error) {
            this->_isStarted = true;

            /* Update the system state */
            this->_pSysState->SetNetworkAPMode(this->_isAP);
            this->_pSysState->SetNetworkSSID(this->_ssid);
            this->_pSysState->SetNetworkPassword(this->_password);
            this->_pSysState->SetNetworkIP(this->_ipAddress);
            this->_pSysState->SetNetworkConfigMode(this->_isStatic);
            this->_pSysState->SetNetworkGatewayIP(this->_staticGatewayIP);
            this->_pSysState->SetNetworkSubnet(this->_staticSubnet);
            this->_pSysState->SetNetworkPrimaryDNS(this->_staticPrimaryDNSIP);
            this->_pSysState->SetNetworkSecondaryDNS(
                this->_staticSecondaryDNSIP
            );
        }
    }
    else {
        error = E_Return::NO_ERROR;
    }

    return error;
}

E_Return WiFiModule::StartWebServers(void) noexcept {
    Settings*              pSettings;
    E_Return               result;
    S_HMParamSettingAccess hmAccessParam;

    /* Get the ports from settings */
    pSettings = Settings::GetInstance();

    result = pSettings->GetSettings(
        SETTING_WEB_PORT,
        (uint8_t*)&this->_webPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        hmAccessParam.kpSettingName = SETTING_WEB_PORT;
        hmAccessParam.pSettingBuffer = (void*)&this->_webPort;
        hmAccessParam.settingBufferSize = sizeof(uint16_t);
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,
            &hmAccessParam
        );
    }
    result = pSettings->GetSettings(
        SETTING_API_PORT,
        (uint8_t*)&this->_apiPort,
        sizeof(uint16_t)
    );
    if (E_Return::NO_ERROR != result) {
        hmAccessParam.kpSettingName = SETTING_API_PORT;
        hmAccessParam.pSettingBuffer = (void*)&this->_apiPort;
        hmAccessParam.settingBufferSize = sizeof(uint16_t);
        HealthMonitor::GetInstance()->ReportHM(
            E_HMEvent::HM_EVENT_SETTINGS_LOAD,
            &hmAccessParam
        );
    }

    /* Create the web server */
    if (nullptr != this->_pWebServer) {
        this->_pWebServer->stop();
        delete this->_pWebServer;
        this->_pWebServer = nullptr;
    }
    this->_pWebServer = new WebServer(this->_webPort);

    if (nullptr != this->_pWebServer) {
        /* Create the API server */
        if (nullptr != this->_pAPIServer) {
            this->_pAPIServer->stop();
            delete this->_pAPIServer;
            this->_pAPIServer = nullptr;
        }
        this->_pAPIServer = new WebServer(this->_apiPort);

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
    else {
        this->_pSysState->SetWebInterfacePort(this->_webPort);
        this->_pSysState->SetAPIInterfacePort(this->_apiPort);
    }

    return result;
}

E_Return WiFiModule::StartAP(void) noexcept {
    bool     retVal;
    E_Return error;

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

        error = E_Return::NO_ERROR;
    }
    else {
        LOG_ERROR("Failed to create the Access Point\n");

        error = E_Return::ERR_WIFI_CONN;
    }

    return error;
}

E_Return WiFiModule::StartNode(void) noexcept {
    E_Return    error;
    Timeout     connTimeout(NODE_CONNECT_TIMEOUT_NS);
    bool        wifiStatus;
    IPAddress   ipAddr;
    IPAddress   gatewayIpAddr;
    IPAddress   subnetAddr;
    IPAddress   primaryDnsIpAddr;
    IPAddress   secondaryDnsIpAddr;

    LOG_INFO("Connecting to network: %s.\n", this->_ssid.c_str());

    error = E_Return::NO_ERROR;

    /* Check static configuration */
    if (this->_isStatic) {
        wifiStatus = WiFi.config(
            ipAddr.fromString(this->_ipAddress.c_str()),
            gatewayIpAddr.fromString(this->_staticGatewayIP.c_str()),
            subnetAddr.fromString(this->_staticSubnet.c_str()),
            primaryDnsIpAddr.fromString(this->_staticPrimaryDNSIP.c_str()),
            secondaryDnsIpAddr.fromString(this->_staticSecondaryDNSIP.c_str())
        );

        if (!wifiStatus) {
            LOG_ERROR("Failed to set static configuration.\n");
            error = E_Return::ERR_WIFI_CONN;
        }
    }
    if (E_Return::NO_ERROR == error) {
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

            error = E_Return::NO_ERROR;
        }
        else {
            LOG_ERROR(
                "Failed to connect to network %s.\n",
                this->_ssid.c_str()
            );
            error = E_Return::ERR_WIFI_CONN;
        }
    }

    return error;
}

E_Return WiFiModule::ConfigureAPIServer(void) noexcept {

    E_Return result;

    /* Initialize the handlers */
    result = APIServerHandlers::Init(this->_pAPIServer);

    return result;
}

E_Return WiFiModule::ConfigureServers(void) noexcept {
    E_Return result;

    result = E_Return::NO_ERROR;

    this->_pWebServerHandler = new WebServerHandlers(this->_pWebServer);
    if (nullptr == this->_pWebServerHandler) {
        result = E_Return::ERR_MEMORY;
    }

    if (E_Return::NO_ERROR == result) {
        result = ConfigureAPIServer();
        if (E_Return::NO_ERROR != result) {
            LOG_ERROR("Failed to configure the API server.\n");
        }
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
    result = this->_pModule->Start();
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