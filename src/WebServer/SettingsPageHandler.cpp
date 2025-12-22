/*******************************************************************************
 * @file SettingsPageHandler.h
 *
 * @see SettingsPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Settings page handler.
 *
 * @details Settings page handler. This file defines the settings page handler
 * used to generate the settings page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <PageHandler.h> /* Page Handler interface */
#include <SystemState.h> /* System state provider */

/* Header file */
#include <SettingsPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the settings page title */
#define SETTINGS_PAGE_TITLE "Settings"

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
SettingsPageHandler::~SettingsPageHandler(void) noexcept {
    /* Nothing to do */
}

void SettingsPageHandler::Generate(std::string& rPageTitle,
                                   std::string& rPageBody) noexcept {
    rPageTitle = SETTINGS_PAGE_TITLE;
    rPageBody =
        "<div>"
        "   <h1>Settings</h1>"
        "</div>";
    GenerateNetworkSettings(rPageBody);
}

void SettingsPageHandler::GenerateNetworkSettings(std::string& rPageBuffer)
const noexcept {
    SystemState* pSysState;
    bool         isAp;
    std::string  buffer;
    uint16_t     port;

    pSysState = SystemState::GetInstance();

    rPageBuffer += "<h2>==== Access Point Settings ====</h2>";
    rPageBuffer += "<div>";
    /* State of AP mode */
    rPageBuffer += "<table>";
    rPageBuffer += "<tr><td>";
    rPageBuffer += "Access Point Enabled";
    rPageBuffer += "</td><td>";
    rPageBuffer += "<input type=\"checkbox\" id=\"ap_enable\" "
                   "name=\"ap_enable\" disabled ";

    pSysState->GetNetworkAPMode(isAp);
    if (isAp) {
        rPageBuffer += "checked";
    }
    rPageBuffer += "/></td></tr></table>";
    rPageBuffer += "</div>";

    rPageBuffer += "<h2>==== Node Settings ====</h2>";
    rPageBuffer += "<div>";
    rPageBuffer += "<table>";

    /* Network SSID */
    pSysState->GetNetworkSSID(buffer);
    rPageBuffer += "<tr><td>Network SSID</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Password */
    pSysState->GetNetworkPassword(buffer);
    rPageBuffer += "<tr><td>Network Password</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Static / Dynamic mode */
    rPageBuffer += "<tr><td>Static Configuration";
    rPageBuffer += "</td><td>";
    rPageBuffer += "<input type=\"checkbox\" id=\"net_stat_en\" "
                   "name=\"net_stat_en\" disabled ";

    pSysState->GetNetworkConfigMode(isAp);
    if (isAp) {
        rPageBuffer += "checked";
    }
    rPageBuffer += "/></td></tr>";

    /* Network Static IP */
    pSysState->GetNetworkIP(buffer);
    rPageBuffer += "<tr><td>Node IP</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Static Gateway */
    pSysState->GetNetworkGatewayIP(buffer);
    rPageBuffer += "<tr><td>Gateway IP</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Subnet */
    pSysState->GetNetworkSubnet(buffer);
    rPageBuffer += "<tr><td>Subnet</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Primary DNS */
    pSysState->GetNetworkPrimaryDNS(buffer);
    rPageBuffer += "<tr><td>Primary DNS</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    /* Network Secondary DNS */
    pSysState->GetNetworkSecondaryDNS(buffer);
    rPageBuffer += "<tr><td>Secondary DNS</td><td>";
    rPageBuffer += buffer;
    rPageBuffer += "</td></tr>";

    rPageBuffer += "</table></div>";

    rPageBuffer += "<h2>==== Interfaces Settings ====</h2>";
    rPageBuffer += "<div>";
    rPageBuffer += "<table>";

    /* Web interface port */
    pSysState->GetWebInterfacePort(port);
    rPageBuffer += "<tr><td>Web Interface Port</td><td>";
    rPageBuffer += std::to_string(port);
    rPageBuffer += "</td></tr>";
    /* API interface port */
    pSysState->GetAPIInterfacePort(port);
    rPageBuffer += "<tr><td>API Interface Port</td><td>";
    rPageBuffer += std::to_string(port);
    rPageBuffer += "</td></tr>";

    rPageBuffer += "</table></div>";
}

