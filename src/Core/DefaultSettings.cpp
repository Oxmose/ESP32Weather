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
static const char* sknode_ssid = "RTHR_NODE\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
/** @brief Default setting for node_pass item. */
static const char* sknode_pass = "RTHR_PASS\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
/** @brief Default setting for web_port item. */
static const uint16_t skweb_port = 80;
/** @brief Default setting for api_port item. */
static const uint16_t skapi_port = 8333;
/** @brief Default setting for node_static item. */
static const bool sknode_static = false;
/** @brief Default setting for node_st_ip item. */
static const char* sknode_st_ip = "192.168.1.200\0\0";
/** @brief Default setting for node_st_gate item. */
static const char* sknode_st_gate = "192.168.1.100\0\0";
/** @brief Default setting for node_st_subnet item. */
static const char* sknode_st_subnet = "255.255.255.0\0\0";
/** @brief Default setting for node_st_pdns item. */
static const char* sknode_st_pdns = "1.1.1.1\0\0\0\0\0\0\0\0";
/** @brief Default setting for node_st_sdns item. */
static const char* sknode_st_sdns = "4.4.4.4\0\0\0\0\0\0\0\0";

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
			.fieldSize = 1
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_SSID,
		S_SettingField {
			.pValue = (uint8_t*)sknode_ssid,
			.fieldSize = 32
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_PASS,
		S_SettingField {
			.pValue = (uint8_t*)sknode_pass,
			.fieldSize = 32
		}
	);
	this->_defaults.emplace(
		SETTING_WEB_PORT,
		S_SettingField {
			.pValue = (uint8_t*)&skweb_port,
			.fieldSize = 2
		}
	);
	this->_defaults.emplace(
		SETTING_API_PORT,
		S_SettingField {
			.pValue = (uint8_t*)&skapi_port,
			.fieldSize = 2
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_STATIC,
		S_SettingField {
			.pValue = (uint8_t*)&sknode_static,
			.fieldSize = 1
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_ST_IP,
		S_SettingField {
			.pValue = (uint8_t*)sknode_st_ip,
			.fieldSize = 15
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_ST_GATE,
		S_SettingField {
			.pValue = (uint8_t*)sknode_st_gate,
			.fieldSize = 15
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_ST_SUBNET,
		S_SettingField {
			.pValue = (uint8_t*)sknode_st_subnet,
			.fieldSize = 15
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_ST_PDNS,
		S_SettingField {
			.pValue = (uint8_t*)sknode_st_pdns,
			.fieldSize = 15
		}
	);
	this->_defaults.emplace(
		SETTING_NODE_ST_SDNS,
		S_SettingField {
			.pValue = (uint8_t*)sknode_st_sdns,
			.fieldSize = 15
		}
	);
}