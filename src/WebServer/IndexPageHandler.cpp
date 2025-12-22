/*******************************************************************************
 * @file IndexPageHandler.h
 *
 * @see IndexPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Index page handler.
 *
 * @details Index page handler. This file defines the index page handler used
 * to generate the index page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>         /* BSP Services */
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <version.h>     /* Versioning */
#include <WiFiModule.h>  /* WiFi services */
#include <PageHandler.h> /* Page Handler interface */
#include <SystemState.h> /* System state provider */

/* Header file */
#include <IndexPageHandler.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the index page title */
#define INDEX_PAGE_TITLE "Home"

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
IndexPageHandler::~IndexPageHandler(void) noexcept {
    /* Nothing to do */
}

void IndexPageHandler::Generate(std::string& rPageTitle,
                                std::string& rPageBody) noexcept {

    rPageTitle = INDEX_PAGE_TITLE;

    /* Title + Header information */
    rPageBody =
        "<div>"
        "   <h1>Real-Time High-Reliability Weather Station</h1>"
        "   <h2>HWUID: ";
    rPageBody += HWManager::GetHWUID();
    rPageBody +=
        "  | " VERSION "</h2>"
        "</div>";

    /* Network information */
    GenerateNetwork(rPageBody);

    /* General system information */
    GenerateSystem(rPageBody);
}

void IndexPageHandler::GenerateNetwork(std::string& rBuffer) const noexcept {
    SystemState* pSysState;
    std::string  buffer;
    bool         isAp;
    uint8_t      rssi;

    pSysState = SystemState::GetInstance();

    rBuffer += "<div>";
    rBuffer += "<h3>==== Network ====</h3>";

    rBuffer += "<table>";
    rBuffer += "<tr>";
    rBuffer += "<td>MAC Address</td><td>" +
                 std::string(HWManager::GetMacAddress()) +
                 "</td>";
    rBuffer += "</tr>";
    rBuffer += "<tr>";
    rBuffer += "<td>Mode</td>";
    pSysState->GetNetworkAPMode(isAp);
    if (isAp) {
        rBuffer += "<td>Access Point</td>";
    }
    else {
        rBuffer += "<td>Node</v>";
    }
    rBuffer += "</tr>";
    rBuffer += "<tr>";

    pSysState->GetNetworkSSID(buffer);
    rBuffer += "<td>SSID</td><td>" +
                 buffer +
                 "</td>";
    rBuffer += "</tr>";
    if (isAp) {
        rBuffer += "<tr>";
        pSysState->GetNetworkPassword(buffer);
        rBuffer += "<td>Password</td><td>" +
                     buffer +
                     "</td>";
        rBuffer += "</tr>";
    }
    rBuffer += "<tr>";
    pSysState->GetNetworkIP(buffer);
    rBuffer += "<td>IP Address</td><td>" +
                 buffer +
                 "</td>";
    rBuffer += "</tr>";
    if (!isAp) {
        rBuffer += "<tr>";
        pSysState->GetNetworkRSSI(rssi);
        rBuffer += "<td>RSSI</td><td>" +
                    std::to_string(rssi) +
                    "</td>";
        rBuffer += "</tr>";
    }
    rBuffer += "</table>";
    rBuffer += "</div>";
}

void IndexPageHandler::GenerateSystem(std::string& rBuffer) const noexcept {
    uint64_t uptime;

    rBuffer += "<div>";
    rBuffer += "<h3>==== System ====</h3>";

    rBuffer += "<table>";
    rBuffer += "<tr>";
    rBuffer += "<th>Uptime D</th><th>Uptime h</th><th>Uptime m</th>"
               "<th>Uptime s</th><th>Uptime ms</th><th>Uptime us</th>"
               "<th>Uptime ns</th>";
    rBuffer += "</tr>";

    uptime = HWManager::GetTime();

    rBuffer += "<tr>";
    rBuffer += "<td>";
    rBuffer += std::to_string(uptime / 86400000000000ULL);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string((uptime / 3600000000000ULL) % 24);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string((uptime / 60000000000ULL) % 60);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string((uptime / 1000000000ULL) % 60);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string((uptime / 1000000ULL) % 1000);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string((uptime / 1000ULL) % 1000);
    rBuffer += "</td>";
    rBuffer += "<td>";
    rBuffer += std::to_string(uptime % 1000);
    rBuffer += "</td>";
    rBuffer += "</tr>";
    rBuffer += "</table>";
    rBuffer += "</div>";
}