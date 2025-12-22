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
HMReporter::HMReporter(const S_HMReporterParam& krParam) noexcept {
    if (0 != krParam.checkPeriodNs % HW_RT_TASK_PERIOD_NS) {
        LOG_ERROR(
            "Check period for %s is not a multiple of the HM period (%lluns).\n",
            krParam.name.c_str(),
            HW_RT_TASK_PERIOD_NS
        );
    }
    this->_checkPeriodNs = krParam.checkPeriodNs;
    this->_nextCheckNs = HWManager::GetTime() + krParam.checkPeriodNs;

    if (0 == krParam.failToDegrade) {
        LOG_ERROR(
            "Failure to degraded must not be 0 for %s.\n",
            krParam.name.c_str()
        );

        this->_failBeforeDegraded = 1;
    }
    else {
        this->_failBeforeDegraded = krParam.failToDegrade;
    }

    if (0 == krParam.failToUnhealthy) {
        LOG_ERROR(
            "Failure to unhealthy must not be 0 for %s.\n",
            krParam.name.c_str()
        );

        this->_failBeforeUnhealthy = 1;
    }
    else {
        this->_failBeforeUnhealthy = krParam.failToUnhealthy;
    }

    this->_name = krParam.name;

    this->_failCount = 0;
    this->_totalFailCount = 0;
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
            ++this->_totalFailCount;
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

uint64_t HMReporter::GetTotalFailureCount(void) const noexcept {
    return this->_totalFailCount;
}

const std::string& HMReporter::GetName(void) const noexcept {
    return this->_name;
}