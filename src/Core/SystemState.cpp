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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* HW layer services */
#include <WiFi.h>            /* WiFi Services */
#include <string>            /* Standard strings */
#include <Logger.h>          /* Logging */
#include <IOLedManager.h>    /* IO Led Manager */
#include <HealthMonitor.h>   /* HM services */
#include <IOButtonManager.h> /* IO Button Manager */

/* Header file */
#include <SystemState.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/* None */

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
SystemState* SystemState::_SPINSTANCE = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
SystemState* SystemState::GetInstance(void) noexcept {
    /* Create the instance if needed */
    if (nullptr == SystemState::_SPINSTANCE) {
        SystemState::_SPINSTANCE = new SystemState();

        if (nullptr == SystemState::_SPINSTANCE) {
            LOG_ERROR("Failed to instanciate the System State object.\n");
            HWManager::Reboot();
        }
    }

    return SystemState::_SPINSTANCE;
}

void SystemState::SetWiFiModule(WiFiModule* pWiFiModule) noexcept {
    this->_pWiFiModule = pWiFiModule;
}

void SystemState::SetHealthMonitor(HealthMonitor* pHealthMonitor) noexcept {
    this->_pHealthMonitor = pHealthMonitor;
}

void SystemState::SetSettings(Settings* pSettings) noexcept {
    this->_pSettings = pSettings;
}

void SystemState::SetIOButtonManager(IOButtonManager* pIOButtonManager)
noexcept {
    this->_pIOButtonManager = pIOButtonManager;
}

void SystemState::SetIOLedManager(IOLedManager* pIOLedManager) noexcept {
    this->_pIOLedManager = pIOLedManager;
}

WiFiModule* SystemState::GetWiFiModule(void) const noexcept {
    return this->_pWiFiModule;
}

HealthMonitor* SystemState::GetHealthMonitor(void) const noexcept {
    return this->_pHealthMonitor;
}

Settings* SystemState::GetSettings(void) const noexcept {
    return this->_pSettings;
}

IOButtonManager* SystemState::GetIOButtonManager(void) const noexcept {
    return this->_pIOButtonManager;
}

IOLedManager* SystemState::GetIOLedManager(void) const noexcept {
    return this->_pIOLedManager;
}

SystemState::SystemState(void) noexcept {
    /* Nothing to do */
}