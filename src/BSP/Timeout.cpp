/*******************************************************************************
 * @file Timeout.cpp
 *
 * @see Timeout.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the timeout features.
 *
 * @details This file defines the timeout features. The timeout and watchdog
 * functionalities are provided by the defined class.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>           /* Hardware services */
#include <cstdint>         /* Standard int types */
#include <HealthMonitor.h> /* Health monitor watchdogs */

/* Header file */
#include <Timeout.h>

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
/* None */

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/

Timeout::Timeout(const uint64_t kTimeoutNs,
                 const uint64_t kWatchdogNs /* = 0 */,
                 void           (*pHandler)(void) /* = nullptr */) noexcept {

    HealthMonitor* pHM;
    E_Return       error;

    /* Initialize the timeouts */
    this->_timeout = kTimeoutNs;
    this->_wdTimeout = kWatchdogNs;
    this->_pWDHandler = pHandler;
    this->_nextWatchdogEvent = 0;
    this->_nextTimeEvent = 0;


    /* First tick */
    Tick();

    /* If a watchdog is needed, add it */
    if (0 != this->_wdTimeout && nullptr != this->_pWDHandler) {
        pHM = SystemState::GetInstance()->GetHealthMonitor();
        error = pHM->AddWatchdog(this, this->_watchdogId);
        if (E_Return::NO_ERROR != error) {
            pHM->ReportHM(E_HMEvent::HM_EVENT_WD_TIMEOUT, (void*)error);
        }
    }
}

Timeout::~Timeout(void) noexcept {
    HealthMonitor* pHM;
    E_Return       error;

    if (0 != this->_wdTimeout && nullptr != this->_pWDHandler) {
        pHM = SystemState::GetInstance()->GetHealthMonitor();
        error = pHM->RemoveWatchdog(this->_watchdogId);
        if (E_Return::NO_ERROR != error) {
            pHM->ReportHM(E_HMEvent::HM_EVENT_WD_TIMEOUT, (void*)error);
        }
    }
}

void Timeout::Tick(void) noexcept{
    uint64_t currentTime;

    currentTime = HWManager::GetTime();

    this->_nextTimeEvent = currentTime + this->_timeout;
    if (0 != this->_wdTimeout) {
        this->_nextWatchdogEvent = currentTime + this->_wdTimeout;
    }
}

bool Timeout::Check(void) const noexcept{
    return (HWManager::GetTime() > this->_nextTimeEvent);
}

uint64_t Timeout::GetNextTimeEvent(void) const noexcept{
    return this->_nextTimeEvent;
}

uint64_t Timeout::GetNextWatchdogEvent(void) const noexcept{
    return this->_nextWatchdogEvent;
}

void Timeout::ExecuteHandler(void) const noexcept{
    if (nullptr != this->_pWDHandler) {
        this->_pWDHandler();
    }
}