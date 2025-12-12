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
}

E_Return WiFiModule::Start(void) noexcept {
  bool retVal;

  /* Check the AP Type */
  if (this->_isAP) {
    retVal = WiFi.softAP(
      this->_ssid,
      this->_password,
      1,
      0,
      WIFI_MODULE_MAX_CONN,
      false
    );
  }

  if (!retVal) {
    return E_Return::ERR_WIFI_CONN;
  }

  return E_Return::NO_ERROR;
}