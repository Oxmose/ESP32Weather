/*******************************************************************************
 * @file WifiValidator.h
 *
 * @see WifiValidator.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 31/12/2025
 *
 * @version 1.0
 *
 * @brief WiFi module validator.
 *
 * @details WiFi module validator. Provides utility functions to validate the
 * WiFi settings and other attributes.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __WIFI_VALIDATOR_H__
#define __WIFI_VALIDATOR_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <WiFiModule.h> /* WiFi configuration */

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
 * @brief The WiFiValidator class.
 *
 * @details The WiFiValidator class provides the necessary utility functions to
 * validate the WiFi settings and other attributes.
 */
class WiFiValidator {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Validates the configuration switches.
         *
         * @details Validates the configuration switches. Checks are performed
         * to validate the size, content and validity of the switches.
         *
         * @return True is returned is the switches is valid, otherwise false,
         * is returned.
         */
        static bool ValidateSwitches(const S_WiFiConfigRequest& krConfig)
        noexcept;

        /**
         * @brief Validates the configuration SSID.
         *
         * @details Validates the configuration SSID. Checks are performed to
         * validate the size, content and validity of the SSID.
         *
         * @return True is returned is the SSID is valid, otherwise false, is
         * returned.
         */
        static bool ValidateSSID(const S_WiFiConfigRequest& krConfig) noexcept;

        /**
         * @brief Validates the configuration SSID.
         *
         * @details Validates the configuration SSID. Checks are performed to
         * validate the size, content and validity of the SSID.
         *
         * @return True is returned is the SSID is valid, otherwise false, is
         * returned.
         */
        static bool ValidatePassword(const S_WiFiConfigRequest& krConfig)
        noexcept;

        /**
         * @brief Validates the configuration IP.
         *
         * @details Validates the configuration IP. Checks are performed
         * to validate the size, content and validity of the IP.
         *
         * @return True is returned is the IP is valid, otherwise false,
         * is returned.
         */
        static bool ValidateIP(const S_WiFiConfigRequest& krConfig) noexcept;

        /**
         * @brief Validates the configuration gateway IP.
         *
         * @details Validates the configuration gateway IP. Checks are performed
         * to validate the size, content and validity of the gateway IP.
         *
         * @return True is returned is the gateway IP is valid, otherwise false,
         * is returned.
         */
        static bool ValidateGateway(const S_WiFiConfigRequest& krConfig)
        noexcept;

        /**
         * @brief Validates the configuration subnet IP.
         *
         * @details Validates the configuration subnet IP. Checks are performed
         * to validate the size, content and validity of the subnet IP.
         *
         * @return True is returned is the subnet IP is valid, otherwise false,
         * is returned.
         */
        static bool ValidateSubnet(const S_WiFiConfigRequest& krConfig)
        noexcept;

        /**
         * @brief Validates the configuration DNS IPs.
         *
         * @details Validates the configuration DNS IPs. Checks are performed to
         * validate the size, content and validity of the DNS IPs.
         *
         * @return True is returned is the DNS IPs are valid, otherwise false,
         * is returned.
         */
        static bool ValidateDNS(const S_WiFiConfigRequest& krConfig) noexcept;

        /**
         * @brief Validates the configuration ports.
         *
         * @details Validates the configuration ports. Checks are performed to
         * validate the size, content and validity of the ports.
         *
         * @return True is returned is the ports is valid, otherwise false, is
         * returned.
         */
        static bool ValidatePorts(const S_WiFiConfigRequest& krConfig) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Validates the format of an IP address.
         *
         * @details Validates the format of an IP address. Checks the content,
         * number of point and  range of each nible.
         *
         * @return True is returned is the format is valid, otherwise false is
         * returned.
         */
        static bool CheckIpFormat(const char* kpIp) noexcept;

};

#endif /* #ifndef __WIFI_VALIDATOR_H__ */