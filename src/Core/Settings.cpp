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
#include <map>             /* Settings map */
#include <string>          /* Standard strings */
#include <Logger.h>        /* Logging services */
#include <Errors.h>        /* Errors definitions */
#include <Arduino.h>       /* Arduino framework */
#include <Preferences.h>   /* Preference storage */
#include <HealthMonitor.h> /* HM services */

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
/* None */

/*******************************************************************************
 * CLASS METHODS
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
            E_Return::ERR_SETTING_INIT :
            E_Return::NO_ERROR
    );
}

Settings* Settings::GetInstance(void) noexcept {
    return Settings::_SP_INSTANCE;
}

E_Return Settings::GetSettings(const std::string& krName,
                               uint8_t*           pData,
                               const size_t       kDataLength) noexcept {
    std::map<std::string, S_SettingField>::const_iterator it;
    E_Return                                              error;

    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {
        /* Get the setting */
        it = this->_cache.find(krName);

        /* First check, if not exists, try to load from storage */
        if (this->_cache.end() == it &&
            E_Return::NO_ERROR == LoadFromNVS(krName)) {
                LOG_DEBUG(
                    "Loaded setting %s from NVS\n",
                    krName.c_str()
                );
                it = this->_cache.find(krName);
        }

        /* Second check, if still not exists, it is a failure */
        if (this->_cache.end() != it) {

            /* Check size */
            if (kDataLength == it->second.fieldSize) {
                /* Get the value */
                memcpy(pData, it->second.pValue, kDataLength);

                error = E_Return::NO_ERROR;
            }
            else {
                LOG_ERROR(
                    "Invalid setting size: %s (%d vs %d).\n",
                    krName.c_str(),
                    kDataLength,
                    it->second.fieldSize
                );

                error = E_Return::ERR_SETTING_NOT_FOUND;
            }
        }
        else {
            LOG_ERROR("Failed to get setting: %s.\n", krName.c_str());

            error = E_Return::ERR_SETTING_NOT_FOUND;
        }

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_SETTINGS_LOCK
            );
        }
    }
    else {
        LOG_ERROR("Failed to acquire settings lock.\n");

        error = E_Return::ERR_SETTING_TIMEOUT;
    }
    return error;
}

E_Return Settings::GetDefault(const std::string& krName,
                              uint8_t*           pData,
                              const size_t       kDataLength) noexcept {
    std::map<std::string, const S_SettingField>::const_iterator it;
    E_Return                                                    error;

    /* Get the setting */
    it = this->_defaults.find(krName);

    /* Second check, if still not exists, it is a failure */
    if (this->_defaults.end() != it) {
        /* Check size */
        if (kDataLength == it->second.fieldSize) {
            /* Get the value */
            memcpy(pData, it->second.pValue, kDataLength);

            error = E_Return::NO_ERROR;
        }
        else {
            LOG_ERROR(
                "Invalid setting size: %s (%d vs %d).\n",
                krName.c_str(),
                kDataLength,
                it->second.fieldSize
            );

            error = E_Return::ERR_SETTING_NOT_FOUND;
        }
    }
    else {
        error = E_Return::ERR_SETTING_NOT_FOUND;
    }

    return error;
}

E_Return Settings::SetSettings(const std::string& krName,
                               const uint8_t*     kpData,
                               const size_t       kDataLength) noexcept {
    std::map<std::string, S_SettingField>::iterator it;
    E_Return                                        error;
    S_SettingField                                  setting;

    if (15 > krName.size()) {
        setting.pValue = new uint8_t[kDataLength];

        if (nullptr != setting.pValue) {
            /* Set the field size */
            setting.fieldSize = kDataLength;
            memcpy(setting.pValue, kpData, kDataLength);

            if (pdPASS == xSemaphoreTake(
                    this->_lock,
                    SETTINGS_LOCK_TIMEOUT_TICKS)
                ) {
                try {
                    /* Get the setting and remove it exists */
                    it = this->_cache.find(krName);
                    if (this->_cache.end() != it) {
                        /* Release memory and clear entry */
                        delete[] it->second.pValue;
                        this->_cache.erase(it);
                    }

                    this->_cache.emplace(krName, setting);

                    error = E_Return::NO_ERROR;
                }
                catch (std::exception& rExc) {

                    LOG_ERROR(
                        "Failed to set setting %s.\n",
                        krName.c_str()
                    );

                    delete[] setting.pValue;
                    error = E_Return::ERR_MEMORY;
                }

                if (pdPASS != xSemaphoreGive(this->_lock)) {
                    HealthMonitor::GetInstance()->ReportHM(
                        E_HMEvent::HM_EVENT_SETTINGS_LOCK
                    );
                }
            }
            else {
                LOG_ERROR("Failed to acquire settings lock.\n");
                delete[] setting.pValue;
                error = E_Return::ERR_SETTING_TIMEOUT;
            }
        }
        else {
            LOG_ERROR(
                "Failed to allocate setting %s.\n",
                krName.c_str()
            );

            error = E_Return::ERR_MEMORY;
        }
    }
    else {
        LOG_ERROR("Invalid setting name: %s.\n", krName.c_str());

        error = E_Return::ERR_SETTING_INVALID;
    }

    return error;
}

E_Return Settings::Commit(void) noexcept {
    std::map<std::string, S_SettingField>::const_iterator it;
    E_Return                                              error;
    size_t                                                saveLen;

    error = E_Return::NO_ERROR;
    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {

        /* For all items in the cache, write to the preference */
        for (it = this->_cache.begin(); this->_cache.end() != it; ++it) {
            saveLen = this->_stcPrefs.putBytes(
                it->first.c_str(),
                it->second.pValue,
                it->second.fieldSize
            );

            if (saveLen != it->second.fieldSize) {
                LOG_ERROR("Failed to save setting: %s.\n", it->first.c_str());

                error = E_Return::ERR_SETTING_COMMIT_FAILURE;
            }
        }

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_SETTINGS_LOCK
            );
        }
    }
    else {
        LOG_ERROR("Failed to acquire settings lock.\n");

        error = E_Return::ERR_SETTING_TIMEOUT;
    }

    return error;
}

E_Return Settings::ClearCache(void) noexcept {
    std::map<std::string, S_SettingField>::const_iterator it;
    E_Return                                              error;

    error = E_Return::NO_ERROR;
    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {

        /* For all items in the cache, write to the preference */
        for (it = this->_cache.begin(); this->_cache.end() != it; ++it) {
            delete[] it->second.pValue;
        }

        this->_cache.clear();

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            HealthMonitor::GetInstance()->ReportHM(
                E_HMEvent::HM_EVENT_SETTINGS_LOCK
            );
        }
    }
    else {
        LOG_ERROR("Failed to acquire settings lock.\n");

        error = E_Return::ERR_SETTING_TIMEOUT;
    }

    return error;
}

E_Return Settings::LoadFromNVS(const std::string& krName) noexcept {
    E_Return                                        error;
    S_SettingField                                  setting;
    size_t                                          fieldSize;
    std::map<std::string, S_SettingField>::iterator it;

    /* Search for field */
    if (this->_stcPrefs.isKey(krName.c_str())) {
        /* Get the field */
        fieldSize = this->_stcPrefs.getBytesLength(krName.c_str());
        setting.pValue = new uint8_t[fieldSize];
        setting.fieldSize = fieldSize;

        if (nullptr != setting.pValue) {
            if (this->_stcPrefs.getBytes(
                    krName.c_str(),
                    setting.pValue,
                    fieldSize) == fieldSize) {
                try {
                    /* Get the setting and remove it exists */
                    it = this->_cache.find(krName);
                    if (this->_cache.end() != it) {
                        /* Release memory and clear entry */
                        delete[] it->second.pValue;
                        this->_cache.erase(krName);
                    }

                    this->_cache.emplace(
                        std::make_pair(krName, setting)
                    );

                    error = E_Return::NO_ERROR;
                }
                catch (std::exception& rExc) {

                    LOG_ERROR("Failed to load setting %s.\n",krName.c_str());
                    delete[] setting.pValue;

                    error = E_Return::ERR_MEMORY;
                }
            }
            else {
                LOG_ERROR("Failed to load setting %s.\n",krName.c_str());
                delete[] setting.pValue;
                error = E_Return::ERR_SETTING_NOT_FOUND;
            }
        }
        else {
            LOG_ERROR("Failed to allocate setting %s.\n", krName.c_str());
            error = E_Return::ERR_MEMORY;
        }
    }
    else {
        LOG_ERROR("Failed to find setting %s.\n", krName.c_str());
        error = E_Return::ERR_SETTING_NOT_FOUND;
    }

    return error;
}

Settings::Settings(void) noexcept {
    /* Initializes the default settings */
    InitializeDefault();
}