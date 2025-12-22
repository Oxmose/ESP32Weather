/*******************************************************************************
 * @file SystemState.h
 *
 * @see SystemState.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 21/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Firmware system stage manager.
 *
 * @details Weather Station Firmware system stage manager. This modules keeps
 * track of the system information and provide an interface for all components
 * to access them.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <WiFi.h>          /* WiFi Services */
#include <string>          /* Standard strings */
#include <HealthMonitor.h> /* HM services */

/* Header file */
#include <SystemState.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

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
/** @brief Stores the current singleton instance. */
SystemState* SystemState::_SPINSTANCE = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
SystemState* SystemState::GetInstance(void) noexcept {
    /* Create the instance if needed */
    if (nullptr == SystemState::_SPINSTANCE) {
        SystemState::_SPINSTANCE = new SystemState();

        if (nullptr == SystemState::_SPINSTANCE) {
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_SYSTEM_STATE_INIT
            );
        }
    }

    return SystemState::_SPINSTANCE;
}

void SystemState::SetNetworkSSID(const std::string& krSSID) noexcept {
    this->_netSSID = krSSID;
}

void SystemState::GetNetworkSSID(std::string& rSSID) const noexcept {
    rSSID = this->_netSSID;
}

void SystemState::SetNetworkIP(const std::string& krIpAddress) noexcept {
    this->_netIPAddress = krIpAddress;
}

void SystemState::GetNetworkIP(std::string& rIpAddress) const noexcept {
    rIpAddress = this->_netIPAddress;
}

void SystemState::SetNetworkPassword(const std::string& krPassword) noexcept {
    this->_netPassword = krPassword;
}

void SystemState::GetNetworkPassword(std::string& rPassword) const noexcept {
    rPassword = this->_netPassword;
}

void SystemState::SetNetworkAPMode(const bool kIsAP) noexcept {
    this->_isAP = kIsAP;
}

void SystemState::GetNetworkAPMode(bool& rIsAP) const noexcept {
    rIsAP = this->_isAP;
}

void SystemState::GetNetworkRSSI(uint8_t& rRSSI) const noexcept {
    rRSSI = WiFi.RSSI();
}

void SystemState::SetNetworkConfigMode(const bool kIsStatic) noexcept {
    this->_isNetworkStatic = kIsStatic;
}

void SystemState::GetNetworkConfigMode(bool& rIsStatic) const noexcept {
    rIsStatic = this->_isNetworkStatic;
}

void SystemState::SetNetworkGatewayIP(const std::string& krIp) noexcept {
    this->_netGatewayIP = krIp;
}

void SystemState::GetNetworkGatewayIP(std::string& rIp) const noexcept {
    rIp = this-> _netGatewayIP;
}

void SystemState::SetNetworkSubnet(const std::string& krSubnet) noexcept {
    this->_netSubnet = krSubnet;
}

void SystemState::GetNetworkSubnet(std::string& rSubnet) const noexcept {
    rSubnet = this->_netSubnet;
}

void SystemState::SetNetworkPrimaryDNS(const std::string& krDnsIp) noexcept {
    this->_netPrimaryDns = krDnsIp;
}

void SystemState::GetNetworkPrimaryDNS(std::string& rDnsIP) const noexcept {
   rDnsIP = this->_netPrimaryDns;
}

void SystemState::SetNetworkSecondaryDNS(const std::string& krDnsIp) noexcept {
   this->_netSecondaryDns = krDnsIp;
}

void SystemState::GetNetworkSecondaryDNS(std::string& rDnsIP) const noexcept {
   rDnsIP = this-> _netSecondaryDns;
}

SystemState::SystemState(void) noexcept {
    /* Nothing to do */
}