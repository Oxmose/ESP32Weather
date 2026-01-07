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
#include <WiFiValidator.h>     /* WiFi Settings validator */
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
 */
#define GET_SETTING(NAME, BUFFER, SIZE, ERROR, SETTINGS) {                   \
    ERROR = SETTINGS->GetSettings(NAME, (uint8_t*)BUFFER, SIZE);             \
    if (E_Return::NO_ERROR != ERROR) {                                       \
        if (E_Return::ERR_SETTING_NOT_FOUND == ERROR) {                      \
            LOG_ERROR("Failed to get setting %s. Trying to get default.\n"); \
            ERROR = SETTINGS->GetDefault(NAME, (uint8_t*)BUFFER, SIZE);      \
            if (E_Return::NO_ERROR != ERROR) {                               \
                PANIC("Failed to get setting %s. Error: %d\n", NAME, ERROR); \
            }                                                                \
        }                                                                    \
        else {                                                               \
            PANIC("Failed to get setting %s. Error: %d\n", NAME, ERROR);     \
        }                                                                    \
    }                                                                        \
}

/**
 * @brief Sets the specified settings.
 *
 * @details Sets the specified settings. An error to the HM is trigered in case
 * of failure.
 *
 * @param[in] NAME The name of the settings to set.
 * @param[out] BUFFER The buffer that contains the setting value.
 * @param[in] SIZE The size of the setting.
 * @param[out] ERROR The error buffer.
 * @param[in] SETTINGS The settings object to use to retrieve the setting.
 */
#define SET_SETTING(NAME, BUFFER, SIZE, ERROR, SETTINGS) {              \
    ERROR = SETTINGS->SetSettings(NAME, (uint8_t*)BUFFER, SIZE);        \
    if (E_Return::NO_ERROR != ERROR) {                                  \
        PANIC("Failed to set setting %s. Error: %d\n", NAME, ERROR);    \
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
    this->_config.isAP = true;
    this->_config.isStatic = false;
    memset(this->_config.ssid, 0, SSID_SIZE_BYTES + 1);
    memset(this->_config.password, 0, PASS_SIZE_BYTES + 1);
    memset(this->_config.ip, 0, IP_ADDR_SIZE_BYTES + 1);
    memset(this->_config.gateway, 0, IP_ADDR_SIZE_BYTES + 1);
    memset(this->_config.subnet, 0, IP_ADDR_SIZE_BYTES + 1);
    memset(this->_config.primaryDNS, 0, IP_ADDR_SIZE_BYTES + 1);
    memset(this->_config.secondaryDNS, 0, IP_ADDR_SIZE_BYTES + 1);

    /* Set as not started */
    this->_isStarted = false;

    this->_pWebServer = nullptr;
    this->_pAPIServer = nullptr;

    pHM = SystemState::GetInstance()->GetHealthMonitor();

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
        PANIC("Failed to create WiFi module HM reporter.\n");
    }
    result = pHM->AddReporter(this->_pReporter, this->_reporterId);
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to add WiFi module HM reporter. Error %d\n", result);
    }

    /* Disable persistence */
    WiFi.persistent(false);

    /* Add to system state */
    SystemState::GetInstance()->SetWiFiModule(this);

    LOG_DEBUG("Initializes WiFi Module.\n");
}

WiFiModule::~WiFiModule(void) noexcept {
    PANIC("Tried to destroy the WiFi module.\n");
}

E_Return WiFiModule::Start(void) noexcept {
    E_Return  error;
    Settings* pSettings;

    LOG_DEBUG("Starting WiFi module.\n");

    if (!this->_isStarted) {
        pSettings = SystemState::GetInstance()->GetSettings();
        /* Check if we should be AP */
        GET_SETTING(
            SETTING_IS_AP,
            &this->_config.isAP,
            sizeof(bool),
            error,
            pSettings
        );

        /* Check the AP Type */
        memset(this->_config.ssid, 0, SSID_SIZE_BYTES + 1);
        memset(this->_config.password, 0, PASS_SIZE_BYTES + 1);

        if (!this->_config.isAP) {
            LOG_INFO("Starting WiFi as node.\n");

            memset(this->_config.ip, 0, IP_ADDR_SIZE_BYTES + 1);
            memset(this->_config.gateway, 0, IP_ADDR_SIZE_BYTES + 1);
            memset(this->_config.subnet, 0, IP_ADDR_SIZE_BYTES + 1);
            memset(this->_config.primaryDNS, 0, IP_ADDR_SIZE_BYTES + 1);
            memset(this->_config.secondaryDNS, 0, IP_ADDR_SIZE_BYTES + 1);

            /* Get the node SSID */
            GET_SETTING(
                SETTING_NODE_SSID,
                this->_config.ssid,
                SSID_SIZE_BYTES,
                error,
                pSettings
            );

            /* Get the node password */
            GET_SETTING(
                SETTING_NODE_PASS,
                this->_config.password,
                PASS_SIZE_BYTES,
                error,
                pSettings
            );

            /* Get the static configuration */
            GET_SETTING(
                SETTING_NODE_STATIC,
                &this->_config.isStatic,
                sizeof(bool),
                error,
                pSettings
            );
            GET_SETTING(
                SETTING_NODE_ST_IP,
                this->_config.ip,
                IP_ADDR_SIZE_BYTES,
                error,
                pSettings
            );
            GET_SETTING(
                SETTING_NODE_ST_GATE,
                this->_config.gateway,
                IP_ADDR_SIZE_BYTES,
                error,
                pSettings
            );
            GET_SETTING(
                SETTING_NODE_ST_SUBNET,
                this->_config.subnet,
                IP_ADDR_SIZE_BYTES,
                error,
                pSettings
            );
            GET_SETTING(
                SETTING_NODE_ST_PDNS,
                this->_config.primaryDNS,
                IP_ADDR_SIZE_BYTES,
                error,
                pSettings
            );
            GET_SETTING(
                SETTING_NODE_ST_SDNS,
                this->_config.secondaryDNS,
                IP_ADDR_SIZE_BYTES,
                error,
                pSettings
            );

            /* Start the node */
            error = StartNode();
        }
        else {
            LOG_INFO("Starting WiFi as AP.\n");

            memcpy(
                this->_config.ssid,
                HWManager::GetHWUID(),
                strnlen(HWManager::GetHWUID(), SSID_SIZE_BYTES)
            );
            memcpy(
                this->_config.password,
                HWManager::GetMacAddress(),
                strnlen(HWManager::GetMacAddress(), PASS_SIZE_BYTES)
            );
            error = StartAP();
        }

        if (E_Return::NO_ERROR == error) {
            this->_isStarted = true;
        }
        else {
            LOG_ERROR("Failed to initialize WiFi module. Error %d\n", error);
        }
    }
    else {
        error = E_Return::NO_ERROR;
    }

    return error;
}

E_Return WiFiModule::StartWebServers(void) noexcept {
    Settings* pSettings;
    E_Return  result;

    LOG_DEBUG("Starting Web Servers.\n");

    /* Get the ports from settings */
    pSettings = SystemState::GetInstance()->GetSettings();

    GET_SETTING(
        SETTING_WEB_PORT,
        &this->_config.webPort,
        sizeof(uint16_t),
        result,
        pSettings
    );

    GET_SETTING(
        SETTING_API_PORT,
        &this->_config.apiPort,
        sizeof(uint16_t),
        result,
        pSettings
    );

    /* Create the web server */
    LOG_DEBUG("Creating Web Server with port %d.\n", this->_config.webPort);
    if (nullptr != this->_pWebServer) {
        this->_pWebServer->stop();
        delete this->_pWebServer;
        this->_pWebServer = nullptr;
    }
    this->_pWebServer = new WebServer(this->_config.webPort);

    if (nullptr != this->_pWebServer) {
        /* Create the API server */
        LOG_DEBUG("Creating API Server with port %d.\n", this->_config.apiPort);
        if (nullptr != this->_pAPIServer) {
            this->_pAPIServer->stop();
            delete this->_pAPIServer;
            this->_pAPIServer = nullptr;
        }
        this->_pAPIServer = new WebServer(this->_config.apiPort);

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
        LOG_DEBUG("Starting Web and API servers.\n");
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

void WiFiModule::GetConfiguration(S_WiFiConfig* pConfig) const noexcept {
    memcpy(pConfig, &this->_config, sizeof(S_WiFiConfig));
}

E_Return WiFiModule::SetConfiguration(const S_WiFiConfigRequest& krConfig)
noexcept {
    E_Return  result;
    Settings* pSettings;

    LOG_DEBUG("Setting new WiFi configuration.\n");
    LOG_DEBUG(
        "New WiFi Configuration: \n"
        "\tIs AP: %d\n"
        "\tSSID: %s\n"
        "\tPassword: %s\n"
        "\tIs Static: %d\n"
        "\tIP: %s\n"
        "\tGateway: %s\n"
        "\tSubnet: %s\n"
        "\tPrimary DNS: %s\n"
        "\tSecondary DNS: %s\n"
        "\tWeb Port: %d\n"
        "\tAPI Port: %d\n",
        krConfig.isAP.first,
        krConfig.ssid.first.c_str(),
        krConfig.password.first.c_str(),
        krConfig.isStatic.first,
        krConfig.ip.first.c_str(),
        krConfig.gateway.first.c_str(),
        krConfig.subnet.first.c_str(),
        krConfig.primaryDNS.first.c_str(),
        krConfig.secondaryDNS.first.c_str(),
        krConfig.webPort.first,
        krConfig.apiPort.first
    );

    result = ValidateConfiguration(krConfig);

    /* Once all is valid, stop the servers */
    if (E_Return::NO_ERROR == result) {
        pSettings = SystemState::GetInstance()->GetSettings();

        LOG_DEBUG("Applying new WiFi configuration.\n");

        /* Apply the configuration */
        SET_SETTING(
            SETTING_IS_AP,
            &krConfig.isAP.first,
            sizeof(bool),
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_SSID,
            krConfig.ssid.first.c_str(),
            SSID_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_PASS,
            krConfig.password.first.c_str(),
            PASS_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_STATIC,
            &krConfig.isStatic.first,
            sizeof(bool),
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_ST_IP,
            krConfig.ip.first.c_str(),
            IP_ADDR_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_ST_GATE,
            krConfig.gateway.first.c_str(),
            IP_ADDR_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_ST_SUBNET,
            krConfig.subnet.first.c_str(),
            IP_ADDR_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_ST_PDNS,
            krConfig.primaryDNS.first.c_str(),
            IP_ADDR_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_NODE_ST_SDNS,
            krConfig.secondaryDNS.first.c_str(),
            IP_ADDR_SIZE_BYTES,
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_WEB_PORT,
            &krConfig.webPort.first,
            sizeof(uint16_t),
            result,
            pSettings
        );
        SET_SETTING(
            SETTING_API_PORT,
            &krConfig.apiPort.first,
            sizeof(uint16_t),
            result,
            pSettings
        );
        result = pSettings->Commit();

        if (E_Return::NO_ERROR == result) {
            LOG_INFO("WiFi settings updated, rebooting...\n");
            HWManager::Reboot();
        }
        else {
            PANIC("Error while commiting WiFi settings. Error %d\n", result);
        }
    }

    return result;
}

E_Return WiFiModule::StartAP(void) noexcept {
    bool        retVal;
    E_Return    error;
    String      ip;

    LOG_DEBUG("Starting WiFi module as AP.\n");

    /* Set AP */
    retVal = WiFi.softAP(
        this->_config.ssid,
        this->_config.password,
        1,
        0,
        WIFI_MODULE_MAX_CONN
    );

    /* On Success, get the IP address */
    if (retVal) {
        LOG_INFO("Started the WiFi Module in AP mode\n");
        LOG_INFO("    SSID: %s\n", this->_config.ssid);
        LOG_INFO("    Password: %s\n", this->_config.password);

        /* Wait to get IP */
        ip = WiFi.softAPIP().toString();
        memset(this->_config.ip, 0, IP_ADDR_SIZE_BYTES + 1);
        memcpy(this->_config.ip, ip.c_str(), ip.length());

        LOG_INFO("    IP Address: %s\n", this->_config.ip);

        error = E_Return::NO_ERROR;
    }
    else {
        LOG_ERROR("Failed to create the Access Point.\n");
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
    String      ip;

    LOG_DEBUG("Starting WiFi module as node.\n");

    LOG_INFO("Connecting to network: %s.\n", this->_config.ssid);

    error = E_Return::NO_ERROR;

    /* Check static configuration */
    if (this->_config.isStatic) {
        ipAddr.fromString(this->_config.ip);
        gatewayIpAddr.fromString(this->_config.gateway);
        subnetAddr.fromString(this->_config.subnet);
        primaryDnsIpAddr.fromString(this->_config.primaryDNS);
        secondaryDnsIpAddr.fromString(this->_config.secondaryDNS);
        wifiStatus = WiFi.config(
            ipAddr,
            gatewayIpAddr,
            subnetAddr,
            primaryDnsIpAddr,
            secondaryDnsIpAddr
        );

        if (!wifiStatus) {
            LOG_ERROR("Failed to set static configuration.\n");
            error = E_Return::ERR_WIFI_CONN;
        }
    }
    if (E_Return::NO_ERROR == error) {
        /* Connect to existing network */
        WiFi.begin(this->_config.ssid, this->_config.password);
        connTimeout.Notify();
        while (WL_CONNECTED != WiFi.status() && !connTimeout.HasTimedOut()) {
            HWManager::DelayExecNs(500000000);
        }

        if (WL_CONNECTED == WiFi.status()) {
            ip = WiFi.localIP().toString();
            memset(this->_config.ip, 0, IP_ADDR_SIZE_BYTES + 1);
            memcpy(this->_config.ip, ip.c_str(), ip.length());

            LOG_INFO("Connected the WiFi Module to network\n");
            LOG_INFO("    SSID: %s\n", this->_config.ssid);
            LOG_INFO("    IP Address: %s\n", this->_config.ip);

            error = E_Return::NO_ERROR;
        }
        else {
            LOG_ERROR("Failed to connect to network %s.\n", this->_config.ssid);
            error = E_Return::ERR_WIFI_CONN;
        }
    }

    return error;
}

E_Return WiFiModule::ConfigureServers(void) noexcept {
    E_Return result;

    LOG_DEBUG("Creating Web and API server handlers.\n");

    this->_pWebServerHandler = new WebServerHandlers(this->_pWebServer);
    if (nullptr != this->_pWebServerHandler) {
        this->_pAPIServerHandler = new APIServerHandlers(this->_pAPIServer);
        if (nullptr == this->_pAPIServerHandler) {
            LOG_ERROR("Failed to create the API server hander.\n");
            result = E_Return::ERR_MEMORY;
        }
        else {
            result = E_Return::NO_ERROR;
        }
    }
    else {
        LOG_ERROR("Failed to create the Web server handler.\n");
        result = E_Return::ERR_MEMORY;
    }

    return result;
}

E_Return WiFiModule::ConfigureServerTasks(void) noexcept {
    E_Return  result;
    BaseType_t createRes;

    LOG_DEBUG("Creating Web and API server handlers tasks.\n");

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

            LOG_ERROR("Failed to create the API handler task.\n");
            result = E_Return::ERR_API_SERVER_TASK;
        }
        else {
            result = E_Return::NO_ERROR;
        }
    }
    else {
        LOG_ERROR("Failed to create the web handler task.\n");
        result = E_Return::ERR_WEB_SERVER_TASK;
    }

    return result;
}

E_Return WiFiModule::ValidateConfiguration(const S_WiFiConfigRequest& krConfig)
const noexcept {

    E_Return result;

    /* Validate the new switches */
    if (!WiFiValidator::ValidateSwitches(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_SWITCHES;
    }
    /* Validate the new SSID */
    else if (!WiFiValidator::ValidateSSID(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_SSID;
    }
    /* Validate the new password */
    else if (!WiFiValidator::ValidatePassword(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_PASSWORD;
    }
    /* Validate the new IP */
    else if (!WiFiValidator::ValidateIP(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_IP;
    }
    /* Validate the new gateway */
    else if (!WiFiValidator::ValidateGateway(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_GATEWAY;
    }
    /* Validate the new subnet */
    else if (!WiFiValidator::ValidateSubnet(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_SUBNET;
    }
    /* Validate the new primary and secondary dns */
    else if (!WiFiValidator::ValidateDNS(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_DNS;
    }
    /* Validate the new web and api ports */
    else if (!WiFiValidator::ValidatePorts(krConfig)) {
        result = E_Return::ERR_WIFI_INVALID_PORTS;
    }
    else {
        result = E_Return::NO_ERROR;
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

    LOG_ERROR("WiFi module is unhealthy, restarting.\n")

    /* Disable and restart */
    if (this->_pModule->_config.isAP) {
        WiFi.softAPdisconnect();
    }
    else {
        WiFi.disconnect();
    }
    this->_pModule->_isStarted = false;

    /* Restart */
    result = this->_pModule->Start();
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to restart WiFi module. Error %d\n", result);
    }
}

bool WiFiModuleHealthReporter::PerformCheck(void) noexcept {
    bool checkResult;

    checkResult = true;

    if (this->_pModule->_isStarted && !this->_pModule->_config.isAP) {

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