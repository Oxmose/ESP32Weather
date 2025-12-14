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
#include <string>   /* Standard string */
#include <WiFi.h>   /* WiFi services */
#include <cstdint>  /* Standard integer definitions */
#include <Errors.h> /* Error definitions */
#include <Logger.h> /* Logger services */

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
WiFiModule::WiFiModule(const std::string& krSSID, const std::string& krPassword)
{
    /* Setup the WiFi service as Access Point with the provided SSID and
    * password
    */
    this->_isAP = true;
    this->_ssid = krSSID;
    this->_password = krPassword;

    /* Set as not started */
    this->_isStarted = false;
}

E_Return WiFiModule::Start(const bool kStartAP) noexcept {
    bool        retVal;
    E_Return    error;

    error = E_Return::NO_ERROR;

    if (!this->_isStarted) {

        this->_isAP = kStartAP;

        /* Check the AP Type */
        if (this->_isAP) {
            /* Set AP */
            retVal = WiFi.softAP(
                this->_ssid.c_str(),
                this->_password.c_str(),
                1,
                0,
                WIFI_MODULE_MAX_CONN
            );

            /* On Success, get the IP address */
            if (retVal) {
                this->_ipAddress = WiFi.softAPIP().toString().c_str();

                LOG_INFO("Started the WiFi Module in AP mode\n");
                LOG_INFO("    SSID: %s\n", this->_ssid.c_str());
                LOG_INFO("    Password: %s\n", this->_password.c_str());
                LOG_INFO("    IP Address: %s\n", this->_ipAddress.c_str());

                /* Success */
                this->_isStarted = true;
            }
            else {
                LOG_ERROR("Failed to create the Access Point\n");

                error = E_Return::ERR_WIFI_CONN;
            }
        }
        else {
            LOG_INFO("Connecting to network: %s.\n", this->_ssid.c_str());

            /* Connect to existing network */
            WiFi.begin(this->_ssid.c_str(), this->_password.c_str());
            while (WL_CONNECTED != WiFi.status()) { // TODO: Add timeout
                delay(500); // TODO: Add HAL dunction
            }

            if (WL_CONNECTED == WiFi.status()) {
                this->_ipAddress = WiFi.localIP().toString().c_str();

                LOG_INFO("Connected the WiFi Module to network\n");
                LOG_INFO("    SSID: %s\n", this->_ssid.c_str());
                LOG_INFO("    IP Address: %s\n", this->_ipAddress.c_str());
            }

            /* Success */
            this->_isStarted = true;
        }
    }

    return error;
}