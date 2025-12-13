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
#include <Errors.h>      /* Errors definitions */
#include <Arduino.h>     /* Arduino framework */
#include <Preferences.h> /* Preference storage */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/* None */

/*******************************************************************************
 * STRUCTURES AND TYPES
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

class ISettingsField
{
    public:
        ~ISettingsField() = default;
        size_t _fieldSize; 
    protected:
    private:
};

template <typename T>
class SettingsField : public ISettingsField {
    public:
        SettingsField(const T& krValue) noexcept {
            _value = krValue;
            _fieldSize = sizeof(T);
        }
        T _value; 
    protected:
    private:
};


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
         * @param[in] kpName The name of the setting to read.
         * @param[out] rValue The buffer or the value to return.
         * 
         * @return The function returns the success or error status.
         */
        template<typename T>
        E_Return GetSettings(const char* kpName, T& rValue) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
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
        std::map<std::string, ISettingsField*> _cache;
};

#endif /* #ifndef __SETTINGS_H__ */