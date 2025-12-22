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

#ifndef __SYSTEM_STATE_H__
#define __SYSTEM_STATE_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string> /* Stardard string */

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

/**
 * @brief The SystemState class.
 *
 * @details The SystemState is a singleton that provides the current system
 * state and information to all components in the system.
 */
class SystemState
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Get the instance of the SystemState object.
         *
         * @details Get the instance of the SystemState object. If the
         * SystemState singleton has not been initialized, it is created.
         *
         * @return The instance of the SystemState object is returned. nullptr
         * is returned on error.
         */
        static SystemState* GetInstance(void) noexcept;

        /**
         * @brief Sets the current network SSID.
         *
         * @details Sets the current network SSID.
         *
         * @param[in] krSSID The current SSID to set.
         */
        void SetNetworkSSID(const std::string& krSSID) noexcept;

        /**
         * @brief Provides the current network SSID.
         *
         * @details Provides the current network SSID.
         *
         * @param[out] rSSID The buffer used to provide the current SSID.
         */
        void GetNetworkSSID(std::string& rSSID) const noexcept;

        /**
         * @brief Sets the current network IP address.
         *
         * @details Sets the current network IP address.
         *
         * @param[in] krIpAddress The current IP address to set.
         */
        void SetNetworkIP(const std::string& krIpAddress) noexcept;

        /**
         * @brief Provides the current network IP address.
         *
         * @details Provides the current network IP address.
         *
         * @param[out] rIpAddress The buffer used to provide the current IP
         * address.
         */
        void GetNetworkIP(std::string& rIpAddress) const noexcept;

        /**
         * @brief Sets the current network password.
         *
         * @details Sets the current network password.
         *
         * @param[in] krPassword The current password to set.
         */
        void SetNetworkPassword(const std::string& krPassword) noexcept;

        /**
         * @brief Provides the current network password.
         *
         * @details Provides the current network password.
         *
         * @param[out] rPassword The buffer used to provide the current network
         * password.
         */
        void GetNetworkPassword(std::string& rPassword) const noexcept;

        /**
         * @brief Sets the current network AP mode.
         *
         * @details Sets the current network AP mode.
         *
         * @param[in] kIsAP The current AP mode to set.
         */
        void SetNetworkAPMode(const bool kIsAP) noexcept;

        /**
         * @brief Provides the current network AP mode.
         *
         * @details Provides the current network AP mode.
         *
         * @param[out] rIsAP The buffer used to provide the current network
         * AP mode.
         */
        void GetNetworkAPMode(bool& rIsAP) const noexcept;

        /**
         * @brief Provides the current network RSSI.
         *
         * @details Provides the current network RSSI.
         *
         * @param[out] rRSSI The buffer used to provide the current network
         * RSSI.
         */
        void GetNetworkRSSI(uint8_t& rRSSI) const noexcept;

        /**
         * @brief Sets the current network configuration mode.
         *
         * @details Sets the current network configuration mode.
         *
         * @param[in] kIsStatic The current configuration mode to set.
         */
        void SetNetworkConfigMode(const bool kIsStatic) noexcept;

        /**
         * @brief Provides the current network configuration mode.
         *
         * @details Provides the current network configuration mode.
         *
         * @param[out] rIsStatic The buffer used to provide the current network
         * configuration mode.
         */
        void GetNetworkConfigMode(bool& rIsStatic) const noexcept;

        /**
         * @brief Sets the current network static gateway IP.
         *
         * @details Sets the current network static gateway IP.
         *
         * @param[in] krIp The current static gateway IP to set.
         */
        void SetNetworkGatewayIP(const std::string& krIp) noexcept;

        /**
         * @brief Provides the current network static gateway IP.
         *
         * @details Provides the current network static gateway IP.
         *
         * @param[out] rIp The buffer used to provide the current network
         * static gateway IP.
         */
        void GetNetworkGatewayIP(std::string& rIp) const noexcept;

        /**
         * @brief Sets the current network static subnet.
         *
         * @details Sets the current network static subnet.
         *
         * @param[in] krSubnet The current static subnet to set.
         */
        void SetNetworkSubnet(const std::string& krSubnet) noexcept;

        /**
         * @brief Provides the current network static subnet.
         *
         * @details Provides the current network static subnet.
         *
         * @param[out] rSubnet The buffer used to provide the current network
         * static subnetP.
         */
        void GetNetworkSubnet(std::string& rSubnet) const noexcept;

        /**
         * @brief Sets the current network static primary DNS IP.
         *
         * @details Sets the current network static primary DNS IP.
         *
         * @param[in] krDnsIP The current static primary DNS IP to set.
         */
        void SetNetworkPrimaryDNS(const std::string& krDnsIp) noexcept;

        /**
         * @brief Provides the current network static primary DNS IP.
         *
         * @details Provides the current network static primary DNS IP.
         *
         * @param[out] rDnsIp The buffer used to provide the current network
         * static primary DNS IP.
         */
        void GetNetworkPrimaryDNS(std::string& rDnsIp) const noexcept;

        /**
         * @brief Sets the current network static secondary DNS IP.
         *
         * @details Sets the current network static secondary DNS IP.
         *
         * @param[in] krDnsIP The current static secondary DNS IP to set.
         */
        void SetNetworkSecondaryDNS(const std::string& krDnsIP) noexcept;

        /**
         * @brief Provides the current network static secondary DNS IP.
         *
         * @details Provides the current network static secondary DNS IP.
         *
         * @param[out] rDnsIp The buffer used to provide the current network
         * static secondary DNS IP.
         */
        void GetNetworkSecondaryDNS(std::string& rDnsIp) const noexcept;


    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief The SystemState constructor.
         *
         * @details The SystemState constructor. Not public to provide singleton
         * interface.
         */
        SystemState(void) noexcept;

        /** @brief Stores the current network SSID. */
        std::string _netSSID;

        /** @brief Stores the current network IP address. */
        std::string _netIPAddress;

        /** @brief Stores the current network password. */
        std::string _netPassword;

        /** @brief Stores the current network gateway IP. */
        std::string _netGatewayIP;

        /** @brief Stores the current network subnet. */
        std::string _netSubnet;

        /** @brief Stores the current network primary dns. */
        std::string _netPrimaryDns;

        /** @brief Stores the current network seoncdary dns. */
        std::string _netSecondaryDns;

        /** @brief Stores the current network configuration mode. */
        bool _isNetworkStatic;

        /** @brief Stores the current network AP mode. */
        bool _isAP;

        /** @brief The singleton instance. */
        static SystemState* _SPINSTANCE;

};

#endif /* #ifndef __SYSTEM_STATE_H__ */