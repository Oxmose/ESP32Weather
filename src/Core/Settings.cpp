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
#include <string>          /* Standard strings */
#include <Logger.h>        /* Logger services */
#include <Errors.h>        /* Errors definitions */
#include <Storage.h>       /* Preference storage */
#include <Arduino.h>       /* Arduino framework */
#include <unordered_map>   /* Settings map */
#include <HealthMonitor.h> /* HM services */

/* Header file */
#include <Settings.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the name of the preference instance. */
#define SETTINGS_PREF_NAME "rthrws_settings"

/** @brief Defines the settings lock timeout in nanoseconds. */
#define SETTINGS_LOCK_TIMEOUT_NS 20000000ULL
/** @brief Defines the settings lock timeout in ticks. */
#define SETTINGS_LOCK_TIMEOUT_TICKS \
    (pdMS_TO_TICKS(SETTINGS_LOCK_TIMEOUT_NS / 1000000ULL))

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

Settings::Settings(void) noexcept {
    /* Start the settings preference */
    this->_pStorage = SystemState::GetInstance()->GetStorage();

    /* Create the lock */
    this->_lock = xSemaphoreCreateMutex();
    if (nullptr == this->_lock) {
        PANIC("Failed to create the Settings lock.\n");
    }

    /* Initializes the default settings */
    InitializeDefault();

    /* Add to system state */
    SystemState::GetInstance()->SetSettings(this);

    LOG_DEBUG("Settings initialized.\n");
}

Settings::~Settings(void) noexcept {
    PANIC("Tried to destroy the Settings.\n");
}

E_Return Settings::GetSettings(const std::string& krName,
                               uint8_t*           pData,
                               const size_t       kDataLength) noexcept {
    std::unordered_map<std::string, S_SettingField>::const_iterator it;
    E_Return                                                        error;

    LOG_DEBUG("Getting setting %s.\n", krName.c_str());

    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {
        /* Get the setting */
        it = this->_cache.find(krName);

        /* First check, if not exists, try to load from storage */
        if (this->_cache.end() == it &&
            E_Return::NO_ERROR == LoadFromStorage()) {
                LOG_DEBUG("Loaded setting %s from NVS\n", krName.c_str());
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
            PANIC("Failed to release the settings lock.\n");
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
    std::unordered_map<std::string, const S_SettingField>::const_iterator it;
    E_Return                                                              error;

    LOG_DEBUG("Getting default setting %s.\n", krName.c_str());

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
        LOG_ERROR("Default setting %s not found.\n", krName.c_str());
        error = E_Return::ERR_SETTING_NOT_FOUND;
    }

    return error;
}

E_Return Settings::SetSettings(const std::string& krName,
                               const uint8_t*     kpData,
                               const size_t       kDataLength) noexcept {
    std::unordered_map<std::string, S_SettingField>::iterator it;
    E_Return                                                  error;
    S_SettingField                                            setting;

    LOG_DEBUG("Setting setting %s.\n", krName.c_str());

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
                    PANIC("Failed to release the setting lock.\n");
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
#include <BSP.h>
E_Return Settings::Commit(void) noexcept {
    std::unordered_map<std::string, S_SettingField>::const_iterator it;
    E_Return                                                        error;
    FsFile                                                          file;

    LOG_DEBUG("Commiting settings.\n");

    error = E_Return::NO_ERROR;
    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {

        /* Clear the preference file */
        this->_pStorage->Remove(SETTINGS_PREF_NAME);

        /* Create the file */
        file = this->_pStorage->Open(SETTINGS_PREF_NAME, O_RDWR | O_CREAT);
        if (file.isOpen()) {
            /* For all items in the cache, write to the preference */
            for (it = this->_cache.begin(); this->_cache.end() != it; ++it) {
                /* Write name */
                if (file.write(it->first.c_str(), it->first.size() + 1) !=
                    it->first.size() + 1) {
                    LOG_ERROR("Failed to write setting name.\n");
                    error = E_Return::ERR_SETTING_COMMIT_FAILURE;
                    break;
                }

                /* Write size */
                if (file.write(&it->second.fieldSize, sizeof(size_t)) !=
                    sizeof(size_t)) {
                    LOG_ERROR("Failed to write setting size.\n");
                    error = E_Return::ERR_SETTING_COMMIT_FAILURE;
                    break;
                }

                /* Write content */
                if (file.write(it->second.pValue, it->second.fieldSize) !=
                    it->second.fieldSize) {
                    LOG_ERROR("Failed to write setting value.\n");
                    error = E_Return::ERR_SETTING_COMMIT_FAILURE;
                    break;
                }
            }

            if (!file.close()) {
                PANIC("Failed to close settings file.\n");
            }
        }
        else {
            LOG_ERROR(
                "Failed to open setting file. Error %d\n",
                file.getError()
            );
            error = E_Return::ERR_SETTING_FILE_ERROR;
        }

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            PANIC("Failed to release the settings lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire settings lock.\n");

        error = E_Return::ERR_SETTING_TIMEOUT;
    }

    return error;
}

E_Return Settings::ClearCache(void) noexcept {
    std::unordered_map<std::string, S_SettingField>::const_iterator it;
    E_Return                                                        error;

    LOG_DEBUG("Clearing settings cache.\n");

    error = E_Return::NO_ERROR;
    if (pdPASS == xSemaphoreTake(this->_lock, SETTINGS_LOCK_TIMEOUT_TICKS)) {

        /* For all items in the cache, write to the preference */
        for (it = this->_cache.begin(); this->_cache.end() != it; ++it) {
            delete[] it->second.pValue;
        }

        this->_cache.clear();

        if (pdPASS != xSemaphoreGive(this->_lock)) {
            PANIC("Failed to release the settings lock.\n");
        }
    }
    else {
        LOG_ERROR("Failed to acquire settings lock.\n");

        error = E_Return::ERR_SETTING_TIMEOUT;
    }

    return error;
}

E_Return Settings::LoadFromStorage(void) noexcept {
    E_Return                                                        error;
    S_SettingField                                                  setting;
    size_t                                                          fieldSize;
    int32_t                                                         readSize;
    FsFile                                                          file;
    std::unordered_map<std::string, S_SettingField>::const_iterator it;
    std::string                                                     name;

    LOG_DEBUG("Loading setting from storage.\n");

    file = this->_pStorage->Open(SETTINGS_PREF_NAME, O_RDONLY);
    if (file.isOpen()) {
        error = E_Return::NO_ERROR;

        while (file.available()) {
            name = "";
            /* Read name */
            name = GetSettingName(file);
            if (0 == name.size()) {
                LOG_ERROR("Failed to read setting name from storage.\n");
                error = E_Return::ERR_SETTING_INVALID;
                break;
            }
            LOG_DEBUG("Loading %s from storage.\n", name.c_str());

            /* Read size */
            readSize = file.read(&fieldSize, sizeof(size_t));
            if (sizeof(size_t) != readSize) {
                LOG_ERROR("Failed to read setting size from storage.\n");
                error = E_Return::ERR_SETTING_INVALID;
                break;
            }
            setting.fieldSize = fieldSize;
            LOG_DEBUG("\t Size %u\n", fieldSize);

            /* Read value */
            setting.pValue = new uint8_t[fieldSize];
            if (nullptr != setting.pValue) {
                readSize = file.read(setting.pValue, fieldSize);
                if (fieldSize != readSize) {
                    LOG_ERROR("Failed to load setting %s.\n",name.c_str());
                    delete[] setting.pValue;
                    error = E_Return::ERR_SETTING_INVALID;
                    break;
                }
            }
            else {
                LOG_ERROR("Failed to allocate setting %s.\n", name.c_str());
                error = E_Return::ERR_MEMORY;
                break;
            }

            /* Add to cache */
             try {
                /* Get the setting and remove it exists */
                it = this->_cache.find(name);
                if (this->_cache.end() != it) {
                    /* Release memory and clear entry */
                    delete[] it->second.pValue;
                    this->_cache.erase(it);
                }

                this->_cache.emplace(std::make_pair(name, setting));

                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {

                LOG_ERROR("Failed to load setting %s.\n",name.c_str());
                delete[] setting.pValue;

                error = E_Return::ERR_MEMORY;
                break;
            }
        }
    }
    else {
        error = E_Return::ERR_SETTING_FILE_ERROR;
    }

    return error;
}

std::string Settings::GetSettingName(FsFile& rFile) const noexcept {
    std::string retStr;
    int32_t     byte;
    bool        readTerminator;

    readTerminator = false;
    while(-1 != (byte = rFile.read())) {
        /* Check if null terminator */
        if (0 == byte) {
            readTerminator = true;
            break;
        }

        /* Add to string */
        retStr += (char)byte;
    }

    /* If no terminator is read error */
    if (!readTerminator) {
        retStr.clear();
    }

    return retStr;
}