/*******************************************************************************
 * @file WifiValidator.cpp
 *
 * @see WifiValidator.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <cstdint>      /* Standard integer definitions */
#include <WiFiModule.h> /* WiFi module configuration */

/* Header file */
#include <WiFiValidator.h>

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
/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
bool WiFiValidator::ValidateSwitches(const S_WiFiConfigRequest& krConfig)
noexcept {
    if (!krConfig.isAP.second || !krConfig.isStatic.second) {
        return false;
    }

    return true;
}

bool WiFiValidator::ValidateSSID(const S_WiFiConfigRequest& krConfig) noexcept {
    size_t size;

    /* Check if set */
    if (!krConfig.ssid.second) {
        return false;
    }

    /* Check size */
    size = krConfig.ssid.first.size();

    return (0 < size && 33 > size);
}

bool WiFiValidator::ValidatePassword(const S_WiFiConfigRequest& krConfig)
noexcept {
    size_t  size;
    uint8_t i;

    /* Check if set */
    if (!krConfig.password.second) {
        return false;
    }

    /* Check size */
    size = krConfig.password.first.size();
    if (MIN_PASS_SIZE_BYTES > size || PASS_SIZE_BYTES < size) {
        return false;
    }

    /* Check content */
    for (i = 0; i < size; ++i) {
        if ((uint8_t)krConfig.password.first[i] < 32 ||
            (uint8_t)krConfig.password.first[i] > 126) {
            return false;
        }
    }

    return true;
}

bool WiFiValidator::ValidateIP(const S_WiFiConfigRequest& krConfig) noexcept {
    /* Check if static, allow empty */
    if (!krConfig.isStatic.first &&
        (!krConfig.ip.second || 0 == krConfig.ip.first.size())) {
        return true;
    }

    /* Check if set */
    if (!krConfig.ip.second) {
        return false;
    }
    /* Check content */
    return CheckIpFormat(krConfig.ip.first.c_str());
}

bool WiFiValidator::ValidateGateway(const S_WiFiConfigRequest& krConfig)
noexcept {
    /* Check if static, allow empty */
    if (!krConfig.isStatic.first &&
        (!krConfig.gateway.second || 0 == krConfig.gateway.first.size())) {
        return true;
    }

    /* Check if set */
    if (!krConfig.gateway.second) {
        return false;
    }

    /* Check content */
    return CheckIpFormat(krConfig.gateway.first.c_str());
}

bool WiFiValidator::ValidateSubnet(const S_WiFiConfigRequest& krConfig)
noexcept {
    /* Check if static, allow empty */
    if (!krConfig.isStatic.first &&
        (!krConfig.subnet.second || 0 == krConfig.subnet.first.size())) {
        return true;
    }

    /* Check if set */
    if (!krConfig.subnet.second) {
        return false;
    }

    /* Check content */
    return CheckIpFormat(krConfig.subnet.first.c_str());
}

bool WiFiValidator::ValidateDNS(const S_WiFiConfigRequest& krConfig) noexcept {
    /* Check if static, allow empty */
    if (!krConfig.isStatic.first) {
        if (krConfig.primaryDNS.second &&
            0 != krConfig.primaryDNS.first.size()) {
            if (!CheckIpFormat(krConfig.primaryDNS.first.c_str())) {
                return false;
            }
        }
        if (krConfig.secondaryDNS.second &&
            0 != krConfig.secondaryDNS.first.size()) {
            if (!CheckIpFormat(krConfig.secondaryDNS.first.c_str())) {
                return false;
            }
        }
        return true;
    }

    /* Check if set */
    if (!krConfig.primaryDNS.second || !krConfig.secondaryDNS.second) {
        return false;
    }

    /* Check content */
    return CheckIpFormat(krConfig.primaryDNS.first.c_str()) &&
           CheckIpFormat(krConfig.secondaryDNS.first.c_str());
}

bool WiFiValidator::ValidatePorts(const S_WiFiConfigRequest& krConfig)
noexcept {
    /* Check if set */
    if (!krConfig.webPort.second || !krConfig.apiPort.second) {
        return false;
    }

    /* Check content */
    if (krConfig.webPort.first == krConfig.apiPort.first) {
        return false;
    }

    return true;
}

bool WiFiValidator::CheckIpFormat(const char* kpIp) noexcept {
    char     current;
    uint8_t  dots;
    uint32_t nible;

    dots = 0;
    nible = 0;
    while (*kpIp) {
        current = *kpIp++;
        if ('0' <= current && '9' >= current) {
            nible = nible * 10 + (current - '0');

            /* Out of range */
            if (255 < nible) {
                return false;
            }
        }
        else if (current == '.') {
            /* Too many dots */
            if (3 == dots) {
                return false;
            }
            ++dots;
            nible = 0;
        }
        else {
            /* Invalid character */
            return false;
        }
    }

    /* Incorrect number of dots */
    if (3 != dots) {
        return false;
    }

    return true;
}