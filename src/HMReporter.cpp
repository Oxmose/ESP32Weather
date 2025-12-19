/*******************************************************************************
 * @file HMReporter.cpp
 *
 * @see HMReporter.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 16/12/2025
 *
 * @version 1.0
 *
 * @brief Health Monitor Reporter abstract.
 *
 * @details Health Monitor Reporter abstract. Defines what the health monitor
 * reporters should implement to be used as a periodic health status checker.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* Hardware layer */
#include <string>            /* Standard string */
#include <cstdint>           /* Standard int types */
#include <Logger.h>          /* Logger services */
#include <HealthMonitor.h>   /* Health Monitor definitions */

/* Header file */
#include <HMReporter.h>

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
HMReporter::HMReporter(const uint64_t     kCheckPeriodNs,
                       const uint32_t     kFailToDegrade,
                       const uint32_t     kFailToUnhealthy,
                       const std::string& krName) noexcept {
    if (0 != kCheckPeriodNs % HW_RT_TASK_PERIOD_NS) {
        LOG_ERROR(
            "Check period for %s is not a multiple of the HM period (%lluns).\n",
            krName.c_str(),
            HW_RT_TASK_PERIOD_NS
        );
    }
    this->_checkPeriodNs = kCheckPeriodNs;
    this->_nextCheckNs = HWManager::GetTime() + kCheckPeriodNs;

    if (0 == kFailToDegrade) {
        LOG_ERROR(
            "Failure to degraded must not be 0 for %s.\n",
            krName.c_str()
        );

        this->_failBeforeDegraded = 1;
    }
    else {
        this->_failBeforeDegraded = kFailToDegrade;
    }

    if (0 == kFailToUnhealthy) {
        LOG_ERROR(
            "Failure to unhealthy must not be 0 for %s.\n",
            krName.c_str()
        );

        this->_failBeforeUnhealthy = 1;
    }
    else {
        this->_failBeforeUnhealthy = kFailToUnhealthy;
    }

    this->_name = krName;

    this->_failCount = 0;
    this->_status = E_HMStatus::HM_DISABLED;
    this->_hasRunningAction = false;
}

HMReporter::~HMReporter(void) noexcept {

}

void HMReporter::HealthCheck(const uint64_t kTime) noexcept {
    HealthMonitor* pHM;
    E_Return       result;

    if (kTime > this->_nextCheckNs) {
        /* Update next check */
        this->_nextCheckNs += this->_checkPeriodNs;

        if (!PerformCheck()) {
            /* On failure, increment the fail count */
            ++this->_failCount;
            if (!this->_hasRunningAction) {

                /* Check if we reached an unhealthy state */
                if (this->_failBeforeUnhealthy <= this->_failCount) {
                    this->_status = E_HMStatus::HM_UNHEALTHY;

                    /* Execute action, status update will be made next iteration */
                    this->_hasRunningAction = true;

                }
                else if (this->_failBeforeDegraded <= this->_failCount) {
                    this->_status = E_HMStatus::HM_DEGRADED;

                    /* Execute action, status update will be made next iteration */
                    this->_hasRunningAction = true;
                }

                /* Add action if necessary */
                if (this->_hasRunningAction) {
                    pHM = HealthMonitor::GetInstance();
                    result = pHM->AddHMAction(this);
                    if (E_Return::NO_ERROR != result) {
                        pHM->ReportHM(
                            E_HMEvent::HM_EVENT_HM_ADD_ACTION,
                            (void*)result
                        );
                    }
                }
            }
        }
        else {
            /* No error, reset the status and fail count. */
            this->_failCount = 0;
            this->_status = E_HMStatus::HM_HEALTHY;
        }
    }
}

void HMReporter::ExecuteAction(void) noexcept {
    /* Check if we reached an unhealthy state */
    if (E_HMStatus::HM_UNHEALTHY == this->_status) {
        OnUnhealthy();
    }
    else if (E_HMStatus::HM_DEGRADED == this->_status) {
        OnDegraded();
    }
    /* No more action */
    this->_hasRunningAction = false;
}

E_HMStatus HMReporter::GetStatus(void) const noexcept {
    return this->_status;
}

uint32_t HMReporter::GetFailureCount(void) const noexcept {
    return this->_failCount;
}

const std::string& HMReporter::GetName(void) const noexcept {
    return this->_name;
}