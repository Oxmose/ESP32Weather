/*******************************************************************************
 * @file SystemState.h
 *
 * @see SystemState.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 21/12/2025
 *
 * @version 1.0
 *
 * @brief Weather Station Firmware system stage manager.
 *
 * @details Weather Station Firmware system stage manager. This modules keeps
 * track of the system information and provide an interface for all components
 * to access them.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __SYSTEM_STATE_H__
#define __SYSTEM_STATE_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>           /* Stardard string */

/* Forwar class declarations to avoid recursive inclusions */
class WiFiModule;
class HealthMonitor;
class Settings;

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

/**
 * @brief The SystemState class.
 *
 * @details The SystemState is a singleton that provides the current system
 * state and information to all components in the system.
 */
class SystemState
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Get the instance of the SystemState object.
         *
         * @details Get the instance of the SystemState object. If the
         * SystemState singleton has not been initialized, it is created.
         *
         * @return The instance of the SystemState object is returned. nullptr
         * is returned on error.
         */
        static SystemState* GetInstance(void) noexcept;

        /**
         * @brief Sets the current WiFi module instance.
         *
         * @details Sets the current WiFi module instance. This stores a pointer
         * in the system state object.
         *
         * @param[in] pWiFiModue The WiFi module to store in the system state.
         */
        void SetWiFiModule(WiFiModule* pWiFiModule) noexcept;

        /**
         * @brief Sets the current health monitor instance.
         *
         * @details Sets the current health monitor instance. This stores a
         * pointer in the system state object.
         *
         * @param[in] pHealthMonitor The health monitor to store in the
         * system state.
         */
        void SetHealthMonitor(HealthMonitor* pHealthMonitor) noexcept;

        /**
         * @brief Sets the current health monitor instance.
         *
         * @details Sets the current health monitor instance. This stores a
         * pointer in the system state object.
         *
         * @param[in] pSettings The settings instance to store in the
         * system state.
         */
        void SetSettings(Settings* pSettings) noexcept;

        /**
         * @brief Returns the current WiFi module instance.
         *
         * @details Returns the current WiFi module instance. This instance is
         * stored in the system state.
         *
         * @return The WiFi module stored in the system state is returned.
         */
        WiFiModule* GetWiFiModule(void) const noexcept;

        /**
         * @brief Returns the current Health Monitor instance.
         *
         * @details Returns the current Health Monitor instance. This instance
         * is stored in the system state.
         *
         * @return The Health Monitor stored in the system state is returned.
         */
        HealthMonitor* GetHealthMonitor(void) const noexcept;

        /**
         * @brief Returns the current Settings instance.
         *
         * @details Returns the current Settings instance. This instance
         * is stored in the system state.
         *
         * @return The Settings stored in the system state is returned.
         */
        Settings* GetSettings(void) const noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief The SystemState constructor.
         *
         * @details The SystemState constructor. Not public to provide singleton
         * interface.
         */
        SystemState(void) noexcept;

        /** @brief Stores the current WiFi module instance. */
        WiFiModule* _pWiFiModule;

        /** @brief Stores the current Health Monitor instance. */
        HealthMonitor* _pHealthMonitor;

        /** @brief Stores the current Settings instance. */
        Settings* _pSettings;

        /** @brief The singleton instance. */
        static SystemState* _SPINSTANCE;

};

#endif /* #ifndef __SYSTEM_STATE_H__ */