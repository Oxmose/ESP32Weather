/*******************************************************************************
 * @file Settings.h
 *
 * @see Settings.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 13/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Firmware settings manager.
 *
 * @details Weather Station Firmware settings manager. This module provides the
 * functionalities to load, save, read and update the firmware's settings.
 * The settings are currently stored in persistent memory.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __SETTINGS_H__
#define __SETTINGS_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <map>           /* Settings map */
#include <string>        /* Standard strings */
#include <Logger.h>      /* Logging services */
#include <Errors.h>      /* Errors definitions */
#include <Arduino.h>     /* Arduino framework */
#include <Preferences.h> /* Preference storage */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the isAP setting key. */
#define SETTING_IS_AP "is_ap"
/** @brief Defines the node ssid setting key. */
#define SETTING_NODE_SSID "node_ssid"
/** @brief Defines the node password setting key. */
#define SETTING_NODE_PASS "node_pass"
/** @brief Defines the web interface port setting key. */
#define SETTING_WEB_PORT "web_port"
/** @brief Defines the api interface port setting key. */
#define SETTING_API_PORT "api_port"
/** @brief Defines the static mode configuration. */
#define SETTING_NODE_STATIC "node_static"
/** @brief Defines the static IP configuration. */
#define SETTING_NODE_ST_IP "node_st_ip"
/** @brief Defines the static gateway configuration. */
#define SETTING_NODE_ST_GATE "node_st_gate"
/** @brief Defines the static subnet configuration. */
#define SETTING_NODE_ST_SUBNET "node_st_subnet"
/** @brief Defines the static primary DNS configuration. */
#define SETTING_NODE_ST_PDNS "node_st_pdns"
/** @brief Defines the static secondary DNS configuration. */
#define SETTING_NODE_ST_SDNS "node_st_sdns"

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/

/** @brief Describes a setting field. */
typedef struct {
    /** @brief The Value of the setting field. */
    uint8_t* pValue;
    /** @brief THe size of the setting field. */
    size_t fieldSize;
} S_SettingField;

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
 * @brief The Settings class.
 *
 * @details The Settings class that provides the necessary functions to read,
 * update, load and store settings. The objects instanciated are the same
 * instance of the settings singleton. To prevent persistent storage aging,
 * the settings are only saved using a commit function.
 */
class Settings
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Settings object constructor.
         *
         * @details Settings object constructor. The function will initialize
         * the preference and create the instance when successfull.
         */
        Settings(void) noexcept;

        /**
         * @brief Reads the setting value based on the settings name.
         *
         * @details Reads the setting value based on the settings name.
         * If the settings does not exist, an error is returned.
         *
         * @param[in] krName The name of the setting to read.
         * @param[out] pData The buffer of the value to return.
         * @param[in] kDataLength The exact length of the data to load.
         *
         * @return The function returns the success or error status.
         */
        E_Return GetSettings(const std::string& krName,
                             uint8_t*           pData,
                             const size_t       kDataLength) noexcept;

        /**
         * @brief Reads the default setting value based on the settings name.
         *
         * @details Reads default the setting value based on the settings name.
         * If the settings does not exist, an error is returned.
         *
         * @param[in] krName The name of the setting to read.
         * @param[out] pData The buffer of the value to return.
         * @param[in] kDataLength The exact length of the data to load.
         *
         * @return The function returns the success or error status.
         */
        E_Return GetDefault(const std::string& krName,
                            uint8_t*           pData,
                            const size_t       kDataLength) noexcept;

        /**
         * @brief Writes the setting value based on the settings name.
         *
         * @details Writes the setting value based on the settings name.
         * If the settings does not exist it is created
         *
         * @param[in] krName The name of the setting to read.
         * @param[in] kpData The buffer of the value to write.
         * @param[in] kDataLength The length of the data to write.
         *
         * @return The function returns the success or error status.
         */
        E_Return SetSettings(const std::string& krName,
                             const uint8_t*     kpData,
                             const size_t       kDataLength) noexcept;

        /**
         * @brief Commits the changes made to the configuration to the
         * non-volatile memory.
         *
         * @details Commits the changes made to the configuration to the
         * non-volatile memory. Try grouping changes to ease the memory wear
         * when commiting changes.
         *
         * @return The function returns the success or error status.
         */
        E_Return Commit(void) noexcept;

        /**
         * @brief Clears the settings cache.
         *
         * @brief Clears the settings cache. All settings will be reloaded from
         * non-volatile memory.
         *
         * @return The function returns the success or error status.
         */
        E_Return ClearCache(void) noexcept;



    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Loads setting from non-volatile memory.
         *
         * @details Loads setting from non-volatile memory. This will
         * automatically cast saved settings.
         *
         * @param[in] krName The name of the setting to load.
         *
         * @return The function returns the success or error status.
         */
        E_Return LoadFromNVS(const std::string& krName) noexcept;

        /**
         * @brief Initializes the default values for the settings.
         *
         * @details Initializes the default values for the settings. Those are
         * not stored in non-volatile memory and generated at each boot.
         */
        void InitializeDefault(void) noexcept;

        /** @brief Stores the settings mutex. */
        SemaphoreHandle_t _lock;

        /** @brief Stores the preference instance. */
        Preferences _stcPrefs;

        /** @brief Stores the settings cache. */
        std::map<std::string, S_SettingField> _cache;

        /** @brief Stores the default settings. */
        std::map<std::string, const S_SettingField> _defaults;
};

#endif /* #ifndef __SETTINGS_H__ */