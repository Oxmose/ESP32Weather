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

#include <unity.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the settings lock timeout in ticks. */
#define SETTINGS_LOCK_TIMEOUT 10000

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
         * @brief Initializes the instance of the Settings object.
         *
         * @details Initializes the instance of the Settings object.
         * If this is the first time this function is called, the Settings
         * singleton is initialized and configured.
         *
         * @return The function returns the success or error status.
         */
        static E_Return InitInstance(void) noexcept;

        /**
         * @brief Get the instance of the Settings object.
         *
         * @details Get the instance of the Settings object. If the preference
         * singleton has not been initialized, nullptr is returned.
         *
         * @return The instance of the Settings object is returned. nullptr is
         * returned on error.
         */
        static Settings* GetInstance(void) noexcept;

        /**
         * @brief Reads the setting value based on the settings name.
         *
         * @details Reads the setting value based on the settings name. If the
         * settings does not exist, an error is returned.
         *
         * @param[in] krName The name of the setting to read.
         * @param[out] kpValue The buffer of the value to return.
         *
         * @return The function returns the success or error status.
         */
        template<typename T>
        E_Return GetSettings(const std::string& krName, T* kpValue) noexcept {
            std::map<std::string, S_SettingField>::const_iterator it;
            E_Return                                              error;

            if (xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT)) {
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
                    if (sizeof(T) == it->second.fieldSize) {
                        /* Get the value */
                        memcpy(kpValue, it->second.pValue, sizeof(T));

                        error = E_Return::NO_ERROR;
                    }
                    else {
                        LOG_ERROR(
                            "Invalid setting size: %s (%d vs %d).\n",
                            krName.c_str(),
                            sizeof(T),
                            it->second.fieldSize
                        );

                        error = E_Return::ERR_SETTING_NOT_FOUND;
                    }
                }
                else {
                    LOG_ERROR("Failed to get setting: %s.\n", krName.c_str());

                    error = E_Return::ERR_SETTING_NOT_FOUND;
                }

                if (pdFALSE == xSemaphoreGive(this->_lock)) {
                    /* TODO: Error Health Monitor */
                }
            }
            else {
                LOG_ERROR("Failed to acquire settings lock.\n");

                error = E_Return::ERR_SETTING_TIMEOUT;
            }
            return error;
        }

        /**
         * @brief Writes the setting value based on the settings name.
         *
         * @details Writes the setting value based on the settings name. If the
         * settings does not exist it is created
         *
         * @param[in] krName The name of the setting to read.
         * @param[in] kpValue The buffer of the value to write.
         *
         * @return The function returns the success or error status.
         */
        template<typename T>
        E_Return SetSettings(const std::string& krName, const T* kpValue)
        noexcept {
            std::map<std::string, S_SettingField>::iterator it;
            E_Return                                        error;
            S_SettingField                                  setting;

            if (krName.size() < 15) {
                setting.pValue = new uint8_t[sizeof(T)];

                if (nullptr != setting.pValue) {
                    /* Set the field size */
                    setting.fieldSize = sizeof(T);
                    memcpy(setting.pValue, kpValue, sizeof(T));

                    if (xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT)) {
                        try {
                            /* Get the setting and remove it exists */
                            it = this->_cache.find(krName);
                            if (it != this->_cache.end()) {
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

                        if (pdFALSE == xSemaphoreGive(this->_lock)) {
                            /* TODO: Error Health Monitor */
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
         * @brief Settings object constructor.
         *
         * @details Settings object constructor. The function will initialize the
         * preference and create the instance when successfull.
         */
        Settings(void);

        /** @brief Stores the current singleton instance. */
        static Settings* _SP_INSTANCE;

        /** @brief Stores the settings mutex. */
        SemaphoreHandle_t _lock;

        /** @brief Stores the preference instance. */
        Preferences _stcPrefs;

        /** @brief Stores the settings cache. */
        std::map<std::string, S_SettingField> _cache;
};

#endif /* #ifndef __SETTINGS_H__ */