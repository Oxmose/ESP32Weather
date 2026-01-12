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
#include <Logger.h>      /* Logger services */
#include <WiFiModule.h>  /* WiFi module services */
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
SettingsPageHandler::SettingsPageHandler(WebServerHandlers* pHandlers) noexcept :
PageHandler(pHandlers) {
}

SettingsPageHandler::~SettingsPageHandler(void) noexcept {
    PANIC("Tried to destroy the Settings page handler.\n");
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
    WiFiModule*  pWiFiModule;
    S_WiFiConfig config;

    pWiFiModule = SystemState::GetInstance()->GetWiFiModule();
    pWiFiModule->GetConfiguration(&config);

    rPageBuffer += "<h2>==== Access Point Settings ====</h2>";
    rPageBuffer += "<div>";
    /* State of AP mode */
    rPageBuffer += "<table>";
    rPageBuffer += "<tr><td>";
    rPageBuffer += "Access Point Enabled";
    rPageBuffer += "</td><td>";
    rPageBuffer += "<input type=\"checkbox\" id=\"ap_enable\" "
                   "name=\"ap_enable\" disabled ";

    if (config.isAP) {
        rPageBuffer += "checked";
    }
    rPageBuffer += "/></td></tr></table>";
    rPageBuffer += "</div>";

    rPageBuffer += "<h2>==== Node Settings ====</h2>";
    rPageBuffer += "<div>";
    rPageBuffer += "<table>";

    /* Network SSID */
    rPageBuffer += "<tr><td>Network SSID</td><td>";
    rPageBuffer += config.ssid;
    rPageBuffer += "</td></tr>";

    /* Network Password */
    rPageBuffer += "<tr><td>Network Password</td><td>";
    rPageBuffer += config.password;
    rPageBuffer += "</td></tr>";

    /* Network Static / Dynamic mode */
    rPageBuffer += "<tr><td>Static Configuration";
    rPageBuffer += "</td><td>";
    rPageBuffer += "<input type=\"checkbox\" id=\"net_stat_en\" "
                   "name=\"net_stat_en\" disabled ";

    if (config.isStatic) {
        rPageBuffer += "checked";
    }
    rPageBuffer += "/></td></tr>";

    /* Network Static IP */
    rPageBuffer += "<tr><td>Node IP</td><td>";
    rPageBuffer += config.ip;
    rPageBuffer += "</td></tr>";

    /* Network Static Gateway */
    rPageBuffer += "<tr><td>Gateway IP</td><td>";
    rPageBuffer += config.gateway;
    rPageBuffer += "</td></tr>";

    /* Network Subnet */
    rPageBuffer += "<tr><td>Subnet</td><td>";
    rPageBuffer += config.subnet;
    rPageBuffer += "</td></tr>";

    /* Network Primary DNS */
    rPageBuffer += "<tr><td>Primary DNS</td><td>";
    rPageBuffer += config.primaryDNS;
    rPageBuffer += "</td></tr>";

    /* Network Secondary DNS */
    rPageBuffer += "<tr><td>Secondary DNS</td><td>";
    rPageBuffer += config.secondaryDNS;
    rPageBuffer += "</td></tr>";

    rPageBuffer += "</table></div>";

    rPageBuffer += "<h2>==== Interfaces Settings ====</h2>";
    rPageBuffer += "<div>";
    rPageBuffer += "<table>";

    /* Web interface port */
    rPageBuffer += "<tr><td>Web Interface Port</td><td>";
    rPageBuffer += std::to_string(config.webPort);
    rPageBuffer += "</td></tr>";
    /* API interface port */
    rPageBuffer += "<tr><td>API Interface Port</td><td>";
    rPageBuffer += std::to_string(config.apiPort);
    rPageBuffer += "</td></tr>";

    rPageBuffer += "</table></div>";

    /* Execution modes */
    rPageBuffer += "<div>";
    rPageBuffer += "<h2>==== Maintenance Mode ====</h2>";
    rPageBuffer += "<table>";
    rPageBuffer += "<tr>";
    rPageBuffer += "<td><a href=\"/reboot?mode=0\">Reboot in nominal mode</a></td>";
    rPageBuffer += "<td><a href=\"/reboot?mode=1\">Reboot in maintenance mode</a></td>";
    rPageBuffer += "</tr>";
    rPageBuffer += "</table>";
    rPageBuffer += "</div>";
}

