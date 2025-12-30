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
#include <Logger.h>      /* Logger service */
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
 * @param[out] SETFLAG The set flag to set to true.
 * @param[out] PARAM The parameter to set with the value of the argument.
 */
#define CHECK_BOOL_ARG(I, NAME, SETFLAG, PARAM)                             \
    (pServer->argName(I).equals(NAME) && !SETFLAG) {                        \
        currentArg = pServer->arg(I);                                       \
        if (currentArg.equals("0")) {                                       \
            PARAM = false;                                                  \
        }                                                                   \
        else {                                                              \
            PARAM = true;                                                   \
        }                                                                   \
        ++argsSet;                                                          \
        SETFLAG = true;                                                     \
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
 * @param[out] SETFLAG The set flag to set to true.
 * @param[out] PARAM The parameter to set with the value of the argument.
 */
#define CHECK_STR_ARG(I, NAME, SETFLAG, PARAM)                              \
    (pServer->argName(I).equals(NAME) && !SETFLAG) {                        \
        currentArg = pServer->arg(I);                                       \
        PARAM = currentArg.c_str();                                         \
        ++argsSet;                                                          \
        SETFLAG = true;                                                     \
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
    /* Nothing to do */
}

void WiFiSettingAPIHandler::Handle(std::string& rResponse, WebServer* pServer) 
noexcept {
    uint32_t args;

    /* Check the number of arguments */
    args = pServer->args();

    /* Check if the user just wants to get the current settings */
    if (1 == args && pServer->arg("mode").equals("getsettings")) {
        GetWiFiSettings(rResponse);
    }
    else if (10 == args && pServer->arg("mode").equals("setsettings")) {
        SetWiFiSettings(pServer, rResponse);
    }
    else {
        rResponse = "{\"result\": " + 
            std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) + 
            ", \"msg\": \"Unknown parameters.\"}";
    }
}

void WiFiSettingAPIHandler::GetWiFiSettings(std::string& rResponse) 
const noexcept {
    SystemState* pSysState;
    std::string  buffer;
    bool         boolBuffer;

    pSysState = SystemState::GetInstance();

    rResponse = "{\"result\": ";
    rResponse += std::to_string(E_APIResult::API_RES_NO_ERROR) + ", ";
    pSysState->GetNetworkAPMode(boolBuffer);
    rResponse += "\"" API_ARG_AP_MODE "\": \"" + std::to_string(boolBuffer) + 
        "\", ";
    pSysState->GetNetworkSSID(buffer);
    rResponse += "\"" API_ARG_SSID "\": \"" + buffer + "\", ";
    pSysState->GetNetworkPassword(buffer);
    rResponse += "\"" API_ARG_PASSWORD "\": \"" + buffer + "\", ";
    pSysState->GetNetworkConfigMode(boolBuffer);
    rResponse += "\"" API_ARG_STATIC "\": \"" + std::to_string(boolBuffer) + 
        "\", ";
    pSysState->GetNetworkIP(buffer);
    rResponse += "\"" API_ARG_IP "\": \"" + buffer + "\", ";
    pSysState->GetNetworkGatewayIP(buffer);
    rResponse += "\"" API_ARG_GATEWY "\": \"" + buffer + "\", ";
    pSysState->GetNetworkSubnet(buffer);
    rResponse += "\"" API_ARG_SUBNET "\": \"" + buffer + "\", ";
    pSysState->GetNetworkPrimaryDNS(buffer);
    rResponse += "\"" API_ARG_PRIMARY_DNS "\": \"" + buffer + "\", ";
    pSysState->GetNetworkSecondaryDNS(buffer);
    rResponse += "\"" API_ARG_SECONDARY_DNS "\": \"" + buffer + "\"} ";
}

void WiFiSettingAPIHandler::SetWiFiSettings(WebServer* pServer, 
                                            std::string& rResponse) const 
noexcept {
    uint32_t     args;
    uint32_t     i;
    uint32_t     argsSet;
    bool         apModeSet;
    bool         ssidSet;
    bool         netpassSet;
    bool         staticSet;
    bool         ipSet;
    bool         gatewaySet;
    bool         subnetSet;
    bool         pDNSSet;
    bool         sDNSSet;
    String       currentArg;
    S_WiFiConfig config;
    E_Return     result;

    /* Check the number of arguments and their value */
    rResponse.clear();
    apModeSet = false;
    ssidSet = false;
    netpassSet = false;
    staticSet = false;
    ipSet = false;
    gatewaySet = false;
    subnetSet = false;
    pDNSSet = false;
    sDNSSet = false;
    argsSet = 0;
    args = pServer->args();
    for (i = 0; i < args; ++i) {
        if CHECK_BOOL_ARG(i, API_ARG_AP_MODE, apModeSet, config.isAP)
        else if CHECK_BOOL_ARG(i, API_ARG_STATIC, staticSet, config.isStatic)
        else if CHECK_STR_ARG(i, API_ARG_SSID, ssidSet, config.ssid)
        else if CHECK_STR_ARG(i, API_ARG_PASSWORD, netpassSet, config.password)
        else if CHECK_STR_ARG(i, API_ARG_IP, ipSet, config.ip)
        else if CHECK_STR_ARG(i, API_ARG_GATEWY, gatewaySet, config.gateway)
        else if CHECK_STR_ARG(i, API_ARG_SUBNET, subnetSet, config.subnet)
        else if CHECK_STR_ARG(
            i, 
            API_ARG_PRIMARY_DNS, 
            pDNSSet, 
            config.primaryDNS
        )
        else if CHECK_STR_ARG(
            i, 
            API_ARG_SECONDARY_DNS, 
            sDNSSet, 
            config.secondaryDNS
        )
        else if (!pServer->argName(i).equals("mode")) {
            rResponse = "{\"result\": " + 
                std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) + 
                ", \"msg\": \"Unknown parameters or duplicate parameter " + 
                pServer->argName(i).c_str() + 
                ".\"}";
            
            break;
        }
        else {
            continue;
        }
    }

    if (9 == argsSet) {
        /* Everything went fine, continue */
        result = E_Return::NO_ERROR; /* TODO */
        if (E_Return::NO_ERROR == result) {
            rResponse = "{\"result\": " + 
                std::to_string(E_APIResult::API_RES_NO_ERROR) + 
                ", \"msg\": \"Saved WiFi settings.\"}";
        }
        else {
            rResponse = "{\"result\": " + 
                std::to_string(E_APIResult::API_RES_WIFI_SET_ACTION_ERR) + 
                ", \"msg\": \"Error while saving the WiFi settings: error " + 
                std::to_string(result) + "\"}";
        }
    }
    else if (0 == rResponse.size()) {
        rResponse = "{\"result\": " + 
            std::to_string(E_APIResult::API_RES_WIFI_SET_UNKNOWN) + 
            ", \"msg\": \"Invalid parameters.\"}";
    }
}