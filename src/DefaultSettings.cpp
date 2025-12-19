/*******************************************************************************
 * @file SettingsDefault.cpp
 *
 * @see Settings.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 18/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Firmware default setting repository.
 *
 * @details Weather Station Firmware default setting repository. This file 
 * is auto-generated and contains the default settings used by the firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <map>        /* Standard hashmap */
#include <cstdint>    /* Standard int types */
#include <Settings.h> /* Settings */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

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
/** @brief Default setting for is_ap item. */
static const bool skis_ap = true;
/** @brief Default setting for node_ssid item. */
static const char* sknode_ssid = "RTHR_NODE";
/** @brief Default setting for node_pass item. */
static const char* sknode_pass = "RTHR_PASS";
/** @brief Default setting for web_port item. */
static const uint16_t skweb_port = 80;
/** @brief Default setting for api_port item. */
static const uint16_t skapi_port = 8333;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
void Settings::InitializeDefault(void) noexcept {
	this->_defaults.emplace(
		SETTING_IS_AP,
		S_SettingField {
			.pValue = (uint8_t*)&skis_ap,
			.fieldSize = sizeof(bool)
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_SSID,
		S_SettingField {
			.pValue = (uint8_t*)&sknode_ssid,
			.fieldSize = sizeof(char*)
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_PASS,
		S_SettingField {
			.pValue = (uint8_t*)&sknode_pass,
			.fieldSize = sizeof(char*)
		}
	);
	this->_defaults.emplace(
		SETTING_WEB_PORT,
		S_SettingField {
			.pValue = (uint8_t*)&skweb_port,
			.fieldSize = sizeof(uint16_t)
		}
	);
	this->_defaults.emplace(
		SETTING_API_PORT,
		S_SettingField {
			.pValue = (uint8_t*)&skapi_port,
			.fieldSize = sizeof(uint16_t)
		}
	);
}