/*******************************************************************************
 * @file WiFiSettingAPIHandler.cpp
 *
 * @see WiFiSettingAPIHandler.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>        /* Standard string */
#include <Logger.h>      /* Logger services */
#include <Errors.h>      /* Errors definitions */
#include <WebServer.h>   /* Web Server services */
#include <WiFiModule.h>  /* WiFi module configuration */
#include <APIHandler.h>  /* API Handler interface */
#include <SystemState.h> /* System state object */

/* Header file */
#include <WiFiSettingAPIHandler.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the argument string for the AP mode setting. */
#define API_ARG_AP_MODE "ap_mode"
/** @brief Defines the argument string for the SSID setting. */
#define API_ARG_SSID "ssid"
/** @brief Defines the argument string for the password setting. */
#define API_ARG_PASSWORD "netpass"
/** @brief Defines the argument string for the static setting. */
#define API_ARG_STATIC "static"
/** @brief Defines the argument string for the ip setting. */
#define API_ARG_IP "ip"
/** @brief Defines the argument string for the gateway setting. */
#define API_ARG_GATEWY "gateway"
/** @brief Defines the argument string for the subnet setting. */
#define API_ARG_SUBNET "subnet"
/** @brief Defines the argument string for the primary DNS setting. */
#define API_ARG_PRIMARY_DNS "pdns"
/** @brief Defines the argument string for the secondary DNS setting. */
#define API_ARG_SECONDARY_DNS "sdns"
/** @brief Defines the argument string for the web port setting. */
#define API_ARG_WEB_PORT "webp"
/** @brief Defines the argument string for the API port setting. */
#define API_ARG_API_PORT "apip"

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/**
 * @brief Checks and retrives a bool argument from the API call.
 *
 * @details Checks and retrives a bool argument from the API call. The check
 * increases the number of set arguments, set the current argument as set and
 * retrieve the value in the WiFi configuration structure.
 *
 * @param[in] I The index of the argument to retrieve.
 * @param[in] NAME The name of the argument to check against.
 * @param[out] PARAM The parameter to set with the value of the argument.
 */
#define CHECK_BOOL_ARG(I, NAME, PARAM)                                      \
    (pServer->argName(I).equals(NAME) && !PARAM.second) {                   \
        currentArg = pServer->arg(I);                                       \
        if (currentArg.equals("0")) {                                       \
            PARAM.first = false;                                            \
        }                                                                   \
        else {                                                              \
            PARAM.first = true;                                             \
        }                                                                   \
        ++argsSet;                                                          \
        PARAM.second = true;                                                \
    }

/**
 * @brief Checks and retrives a string argument from the API call.
 *
 * @details Checks and retrives a string argument from the API call. The check
 * increases the number of set arguments, set the current argument as set and
 * retrieve the value in the WiFi configuration structure.
 *
 * @param[in] I The index of the argument to retrieve.
 * @param[in] NAME The name of the argument to check against.
 * @param[out] PARAM The parameter to set with the value of the argument.
 */
#define CHECK_STR_ARG(I, NAME, PARAM)                                       \
    (pServer->argName(I).equals(NAME) && !PARAM.second) {                   \
        PARAM.first = std::string(pServer->arg(I).c_str());                 \
        ++argsSet;                                                          \
        PARAM.second = true;                                                \
    }

/**
 * @brief Checks and retrives a uint16 argument from the API call.
 *
 * @details Checks and retrives a uint16 argument from the API call. The check
 * increases the number of set arguments, set the current argument as set and
 * retrieve the value in the WiFi configuration structure.
 *
 * @param[in] I The index of the argument to retrieve.
 * @param[in] NAME The name of the argument to check against.
 * @param[out] PARAM The parameter to set with the value of the argument.
 */
#define CHECK_UINT16_ARG(I, NAME, PARAM)                                    \
    (pServer->argName(I).equals(NAME) && !PARAM.second) {                   \
        try {                                                               \
            PARAM.first = (uint16_t)std::stoi(pServer->arg(I).c_str());     \
            ++argsSet;                                                      \
            PARAM.second = true;                                            \
        }                                                                   \
        catch (std::exception& rExc) {                                      \
            rResponse = "{\"result\": " +                                   \
                std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) +     \
                ", \"msg\": \"Invalid parameter " NAME " value.\"}";        \
            break;                                                          \
        }                                                                   \
    }
/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
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
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
WiFiSettingAPIHandler::~WiFiSettingAPIHandler(void) noexcept {
    PANIC("Tried to destroy the WiFi settings API handler.\n");
}

void WiFiSettingAPIHandler::Handle(std::string& rResponse, WebServer* pServer)
noexcept {
    uint32_t args;

    LOG_DEBUG("Handling WiFi setting API.\n");

    /* Check the number of arguments */
    args = pServer->args();

    /* Check if the user just wants to get the current settings */
    if (1 == args && pServer->arg("mode").equals("getsettings")) {
        GetWiFiSettings(rResponse);
    }
    else if (12 == args && pServer->arg("mode").equals("setsettings")) {
        SetWiFiSettings(pServer, rResponse);
    }
    else {
        rResponse = "{\"result\": " +
            std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) +
            ", \"msg\": \"Unknown parameters.\"}";

        LOG_ERROR("Invalid WiFi setting API parameters. Count: %d\n", args);
    }
}

void WiFiSettingAPIHandler::GetWiFiSettings(std::string& rResponse)
const noexcept {
    WiFiModule*  pWiFiModule;
    S_WiFiConfig config;

    LOG_DEBUG("Handling WiFi settings Get API.\n");

    pWiFiModule = SystemState::GetInstance()->GetWiFiModule();
    pWiFiModule->GetConfiguration(&config);

    rResponse = "{\"result\": ";
    rResponse += std::to_string(E_APIResult::API_RES_NO_ERROR) + ", ";
    rResponse += "\"" API_ARG_AP_MODE "\": \"" + std::to_string(config.isAP) +
        "\", ";
    rResponse += "\"" API_ARG_SSID "\": \"" + std::string(config.ssid) + "\", ";
    rResponse += "\"" API_ARG_PASSWORD "\": \"" + std::string(config.password) +
        "\", ";
    rResponse += "\"" API_ARG_STATIC "\": \"" + std::to_string(config.isStatic)+
        "\", ";
    rResponse += "\"" API_ARG_IP "\": \"" + std::string(config.ip) + "\", ";
    rResponse += "\"" API_ARG_GATEWY "\": \"" + std::string(config.gateway) +
        "\", ";
    rResponse += "\"" API_ARG_SUBNET "\": \"" + std::string(config.subnet) +
        "\", ";
    rResponse += "\"" API_ARG_PRIMARY_DNS "\": \"" +
        std::string(config.primaryDNS) + "\", ";
    rResponse += "\"" API_ARG_SECONDARY_DNS "\": \"" +
        std::string(config.secondaryDNS) + "\", ";
    rResponse += "\"" API_ARG_WEB_PORT "\": \"" +
        std::to_string(config.webPort) + "\", ";
    rResponse += "\"" API_ARG_API_PORT "\": \"" +
        std::to_string(config.apiPort) + "\"} ";

}

void WiFiSettingAPIHandler::SetWiFiSettings(WebServer* pServer,
                                            std::string& rResponse) const
noexcept {
    uint32_t            args;
    uint32_t            i;
    uint32_t            argsSet;
    S_WiFiConfigRequest config;
    E_Return            result;
    WiFiModule*         pWiFiModule;
    String              currentArg;

    LOG_DEBUG("Handling WiFi settings Set API.\n");

    /* Check the number of arguments and their value */
    rResponse.clear();
    config.isAP.second = false;
    config.isStatic.second = false;
    config.ssid.second = false;
    config.password.second = false;
    config.ip.second = false;
    config.gateway.second = false;
    config.subnet.second = false;
    config.primaryDNS.second = false;
    config.secondaryDNS.second = false;
    config.webPort.second = false;
    config.apiPort.second = false;
    argsSet = 0;
    args = pServer->args();
    for (i = 0; i < args; ++i) {
        if CHECK_BOOL_ARG(i, API_ARG_AP_MODE, config.isAP)
        else if CHECK_BOOL_ARG(i, API_ARG_STATIC, config.isStatic)
        else if CHECK_STR_ARG(i, API_ARG_SSID, config.ssid)
        else if CHECK_STR_ARG(i, API_ARG_PASSWORD, config.password)
        else if CHECK_STR_ARG(i, API_ARG_IP, config.ip)
        else if CHECK_STR_ARG(i, API_ARG_GATEWY, config.gateway)
        else if CHECK_STR_ARG(i, API_ARG_SUBNET, config.subnet)
        else if CHECK_STR_ARG(i, API_ARG_PRIMARY_DNS, config.primaryDNS)
        else if CHECK_STR_ARG(i, API_ARG_SECONDARY_DNS, config.secondaryDNS)
        else if CHECK_UINT16_ARG(i, API_ARG_WEB_PORT, config.webPort)
        else if CHECK_UINT16_ARG(i, API_ARG_API_PORT, config.apiPort)
        else if (!pServer->argName(i).equals("mode")) {
            rResponse = "{\"result\": " +
                std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) +
                ", \"msg\": \"Unknown parameters or duplicate parameter " +
                pServer->argName(i).c_str() +
                ".\"}";

            LOG_ERROR(
                "WiFi Setttings Set API invalid parameter: %s.",
                pServer->argName(i).c_str()
            );

            break;
        }
        else {
            continue;
        }
    }

    if (11 == argsSet) {
        /* Everything went fine, continue */
        pWiFiModule = SystemState::GetInstance()->GetWiFiModule();
        result = pWiFiModule->SetConfiguration(config);
        if (E_Return::NO_ERROR == result) {
            rResponse = "{\"result\": " +
                std::to_string(E_APIResult::API_RES_NO_ERROR) +
                ", \"msg\": \"Saved WiFi settings.\"}";

            LOG_DEBUG("WiFi Setting API Set success.\n");
        }
        else {
            rResponse = "{\"result\": " +
                std::to_string(E_APIResult::API_RES_WIFI_SET_ACTION_ERR) +
                ", \"msg\": \"Error while saving the WiFi settings: error " +
                std::to_string(result) + "\"}";

            LOG_ERROR("WiFi Setting API Set error. Error %d.", result);
        }
    }
    else if (0 == rResponse.size()) {
        rResponse = "{\"result\": " +
            std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) +
            ", \"msg\": \"Invalid parameters, expected 11, parsed " +
            std::to_string(argsSet) + ".\"}";

            LOG_ERROR(
                "WiFi Setting API Set error. Expected 11 arguments, parser %d.",
                argsSet
            );
    }
}