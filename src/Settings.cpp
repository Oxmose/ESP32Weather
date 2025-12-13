/*******************************************************************************
 * @file Settings.cpp
 *
 * @see Settings.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <map>           /* Settings map */
#include <string>        /* Standard strings */
#include <Logger.h>      /* Logging services */
#include <Errors.h>      /* Errors definitions */
#include <Arduino.h>     /* Arduino framework */
#include <Preferences.h> /* Preference storage */

/* Header file */
#include <Settings.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the name of the preference instance. */
#define SETTINGS_PREF_NAME "rthrws_settings"

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
Settings* Settings::_SP_INSTANCE = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

E_Return Settings::InitInstance(void) noexcept {
    /* Create the instance if needed */
    if (nullptr == Settings::_SP_INSTANCE) {
        Settings::_SP_INSTANCE = new Settings();

        /* Start the settings preference */
        if (Settings::_SP_INSTANCE->_stcPrefs.begin(
            SETTINGS_PREF_NAME, 
            false
        )) {
            /* Allocate the lock */
            Settings::_SP_INSTANCE->_lock = xSemaphoreCreateMutex();
            if (nullptr == Settings::_SP_INSTANCE->_lock) {
                delete Settings::_SP_INSTANCE;
                Settings::_SP_INSTANCE = nullptr;

                LOG_ERROR("Failed to initialize the Preference Instance.\n");
            }
        }
        else {
            delete Settings::_SP_INSTANCE;
            Settings::_SP_INSTANCE = nullptr;

            LOG_ERROR("Failed to start the Preference Instance.\n");
        } 
    }

    return (
        (nullptr == Settings::_SP_INSTANCE) ? 
            E_Return::ERR_PREFERENCE_INIT : 
            E_Return::NO_ERROR
    );
}

Settings* Settings::GetInstance(void) noexcept {
    return Settings::_SP_INSTANCE;
}

template<typename T>
E_Return Settings::GetSettings(const char* kpName, T& rValue) noexcept {
    std::map<std::string, ISettingsField*>::const_iterator it;
    
    /* Get the setting */
    it = this->_cache.find(kpName);
    

    return E_Return::NO_ERROR;
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/