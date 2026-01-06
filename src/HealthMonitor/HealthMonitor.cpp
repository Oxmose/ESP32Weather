/*******************************************************************************
 * @file HealthMonitor.cpp
 *
 * @see HealthMonitor.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief Firmware Health Monitor functionalities.
 *
 * @details Firmware Health Monitor functionalities. Provides watchdogs, error
 * handling and monitoring.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>             /* Hardware services */
#include <string>            /* Standard string */
#include <cstdint>           /* Standard int types */
#include <Logger.h>          /* Logger services */
#include <Arduino.h>         /* Arduino library */
#include <Settings.h>        /* Firmware settings */
#include <APIHandler.h>      /* API Handler errors */
#include <HMConfiguration.h> /* HM Configuration */
/* Header file */
#include <HealthMonitor.h>

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the real-time task period tolerance in nanoseconds. */
#define HW_RT_TASK_PERIOD_TOLERANCE_NS 500000ULL
/** @brief Real-time task watchdog timeout in nanoseconds. */
#define HW_RT_TASK_WD_TIMEOUT_NS (2 * HW_RT_TASK_PERIOD_NS)

/** @brief Hardware Real-Time Task name. */
#define HW_RT_TASK_NAME "HW-RT_TASK"
/** @brief Hardware Real-Time Task stack size in bytes. */
#define HW_RT_TASK_STACK 4096
/** @brief Hardware Real-Time Task priority. */
#define HW_RT_TASK_PRIO (configMAX_PRIORITIES - 1)
/** @brief Hardware Real-Time Task mapped core ID. */
#define HW_RT_TASK_CORE 0
/** @brief Defines the watchdogs lock timeout in nanoseconds. */
#define WD_LOCK_TIMEOUT_NS 20000000ULL
/** @brief Defines the watchdogs lock timeout in ticks. */
#define WD_LOCK_TIMEOUT_TICKS (pdMS_TO_TICKS(WD_LOCK_TIMEOUT_NS / 1000000ULL))

/** @brief Actions Task name. */
#define HM_ACTIONS_TASK_NAME "HM_ACTIONS_TASK"
/** @brief Actions Task stack size in bytes. */
#define HM_ACTIONS_TASK_STACK 4096
/** @brief Actions Task priority. */
#define HM_ACTIONS_TASK_PRIO (configMAX_PRIORITIES - 2)
/** @brief Actions Task mapped core ID. */
#define HM_ACTIONS_TASK_CORE 0
/** @brief Defines the size of the actions queue. */
#define HM_ACTIONS_TASK_QUEUE_LENGTH 10

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/**
 * @brief Defines the HM configuration entry, associating a HM event to a
 * specific handler.
 */
typedef void(*T_EventHandler)(void*);

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
 ******************************************************************************/

/**
 * @brief HM Event hander for HM_EVENT_WEB_SERVER_INIT_ERROR event.
 *
 * @details HM Event hander for HM_EVENT_WEB_SERVER_INIT_ERROR event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWebServerInitErrorHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_API_SERVER_INIT_ERROR event.
 *
 * @details HM Event hander for HM_EVENT_API_SERVER_INIT_ERROR event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMAPIServerInitErrorHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_HW_MAC_NOT_AVAIL event.
 *
 * @details HM Event hander for HM_EVENT_HW_MAC_NOT_AVAIL event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMHWMACNotAvailHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_HM_LOCK event.
 *
 * @details HM Event hander for HM_EVENT_HM_LOCK event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMHWLockHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_WD_TIMEOUT event.
 *
 * @details HM Event hander for HM_EVENT_WD_TIMEOUT event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWdTimeoutHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_RT_TASK_CREATE event.
 *
 * @details HM Event hander for HM_EVENT_RT_TASK_CREATE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMRTTaskCreateHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_RT_TASK_WAIT event.
 *
 * @details HM Event hander for HM_EVENT_RT_TASK_WAIT event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMRTTaskWait(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_RT_TASK_DEADLINE_MISS event.
 *
 * @details HM Event hander for HM_EVENT_RT_TASK_DEADLINE_MISS event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMRTTaskDeadlineMissHandler(void* pParam);


/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_CREATE event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_CREATE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsCreateHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_LOCK event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_LOCK event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsLockHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_LOAD event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_LOAD event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsAccessLoadHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_STORE event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_STORE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsAccessStoreHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_DEFAULT event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_DEFAULT event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsAccessDefaultHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_WIFI_CREATE event.
 *
 * @details HM Event hander for HM_EVENT_WIFI_CREATE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWifiCreateHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_WEB_START event.
 *
 * @details HM Event hander for HM_EVENT_WEB_START event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWebStartHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_HM_ADD_ACTION event.
 *
 * @details HM Event hander for HM_EVENT_HM_ADD_ACTION event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMAddActionHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_HM_ACTION_RECV_FAILED event.
 *
 * @details HM Event hander for HM_EVENT_HM_ACTION_RECV_FAILED event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMActionQCreateFailedHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_ACTION_TASK_CREATE event.
 *
 * @details HM Event hander for HM_EVENT_ACTION_TASK_CREATE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMActionTaskCreateHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_WEB_SERVER_NOT_FOUND event.
 *
 * @details HM Event hander for HM_EVENT_WEB_SERVER_NOT_FOUND event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWebServerNotFoundHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_API_SERVER_NOT_FOUND event.
 *
 * @details HM Event hander for HM_EVENT_API_SERVER_NOT_FOUND event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMAPIServerNotFoundHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_HM_REMOVE_ACTION event.
 *
 * @details HM Event hander for HM_EVENT_HM_REMOVE_ACTION event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMRemoveActionHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_WIFI_STOP event.
 *
 * @details HM Event hander for HM_EVENT_WIFI_STOP event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMWiFiStopHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_SETTINGS_COMMIT event.
 *
 * @details HM Event hander for HM_EVENT_SETTINGS_COMMIT event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMSettingsAccessCommitHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_MAIN_LOOP_DEADLINE_MISS event.
 *
 * @details HM Event hander for HM_EVENT_MAIN_LOOP_DEADLINE_MISS event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMMainLoopDeadlineMissHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_IO_BUTTON_LOCK event.
 *
 * @details HM Event hander for HM_EVENT_IO_BUTTON_LOCK event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMIOButtonLockHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_IO_TASK_CREATE event.
 *
 * @details HM Event hander for HM_EVENT_IO_TASK_CREATE event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMIOTaskCreateHandler(void* pParam);

/**
 * @brief HM Event hander for HM_EVENT_IO_TASK_DEADLINE_MISS event.
 *
 * @details HM Event hander for HM_EVENT_IO_TASK_DEADLINE_MISS event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
static void HMIOTaskDeadlineMissHandler(void* pParam);

#ifdef HM_TEST_EVENT
/**
 * @brief HM Event hander for HM_EVENT_TEST event.
 *
 * @details HM Event hander for HM_EVENT_TEST event.
 *
 * @param[in] pParam The parameter used when notifying the HM of the event.
 */
void test_hm_event_handler(void* pParam);
#endif

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/

/************************* Imported global variables **************************/
/* None */

/************************* Exported global variables **************************/
/* None */

/************************** Static global variables ***************************/

/** @brief Stores the HM event handlers. */
static T_EventHandler sHMHandlers[HM_EVENT_MAX] {
    HMWebServerInitErrorHandler,        /* HM_EVENT_WEB_SERVER_INIT_ERROR */
    HMAPIServerInitErrorHandler,        /* HM_EVENT_API_SERVER_INIT_ERROR */
    HMHWMACNotAvailHandler,             /* HM_EVENT_HW_MAC_NOT_AVAIL */
    HMHWLockHandler,                    /* HM_EVENT_HM_LOCK */
    HMWdTimeoutHandler,                 /* HM_EVENT_WD_TIMEOUT */
    HMRTTaskCreateHandler,              /* HM_EVENT_RT_TASK_CREATE */
    HMRTTaskWait,                       /* HM_EVENT_RT_TASK_WAIT */
    HMRTTaskDeadlineMissHandler,        /* HM_EVENT_RT_TASK_DEADLINE_MISS */
    HMSettingsCreateHandler,            /* HM_EVENT_SETTINGS_CREATE */
    HMSettingsLockHandler,              /* HM_EVENT_SETTINGS_LOCK */
    HMSettingsAccessLoadHandler,        /* HM_EVENT_SETTINGS_LOAD */
    HMSettingsAccessStoreHandler,       /* HM_EVENT_SETTINGS_STORE */
    HMSettingsAccessDefaultHandler,     /* HM_EVENT_SETTINGS_DEFAULT */
    HMWifiCreateHandler,                /* HM_EVENT_WIFI_CREATE */
    HMWebStartHandler,                  /* HM_EVENT_WEB_START */
    HMAddActionHandler,                 /* HM_EVENT_HM_ADD_ACTION */
    HMActionQCreateFailedHandler,       /* HM_EVENT_HM_ACTION_RECV_FAILED */
    HMActionTaskCreateHandler,          /* HM_EVENT_ACTION_TASK_CREATE */
    HMWebServerNotFoundHandler,         /* HM_EVENT_WEB_SERVER_NOT_FOUND */
    HMAPIServerNotFoundHandler,         /* HM_EVENT_API_SERVER_NOT_FOUND */
    HMRemoveActionHandler,              /* HM_EVENT_HM_REMOVE_ACTION */
    HMWiFiStopHandler,                  /* HM_EVENT_WIFI_STOP */
    HMSettingsAccessCommitHandler,      /* HM_EVENT_SETTINGS_COMMIT */
    HMMainLoopDeadlineMissHandler,      /* HM_EVENT_MAIN_LOOP_DEADLINE_MISS */
    HMIOButtonLockHandler,              /* HM_EVENT_IO_BUTTON_LOCK */
    HMIOTaskCreateHandler,              /* HM_EVENT_IO_TASK_CREATE */
    HMIOTaskDeadlineMissHandler,        /* HM_EVENT_IO_TASK_DEADLINE_MISS */
#ifdef HM_TEST_EVENT
    test_hm_event_handler,              /* HM_EVENT_TEST */
#endif
};

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/

static void HMWebServerInitErrorHandler(void* pParam) {
    LOG_ERROR("Web Server initialization error.\n");
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("Web server is already initialized.\n");
    }
    else if (1 == (uint32_t)pParam) {
        LOG_ERROR("Web server lock failed to initialize.\n");
    }
    else if (2 == (uint32_t)pParam) {
        LOG_ERROR("Web server handler failed to be allocated.\n");
    }
    HWManager::Reboot();
}

static void HMAPIServerInitErrorHandler(void* pParam) {
    LOG_ERROR("API Server initialization error.\n");
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("API server is already initialized.\n");
    }
    else if (1 == (uint32_t)pParam) {
        LOG_ERROR("API server lock failed to initialize.\n");
    }
    else if (2 == (uint32_t)pParam) {
        LOG_ERROR("API server handler failed to be allocated.\n");
    }
    HWManager::Reboot();
}

static void HMHWMACNotAvailHandler(void* pParam) {
    LOG_ERROR("Error while reading MAC address.\n");
    HWManager::Reboot();
}

static void HMHWLockHandler(void* pParam) {
    LOG_ERROR("HM lock error:\n");
    if ((bool)pParam) {
        LOG_ERROR("\tFailed to lock the HM.\n");
    }
    else {
        LOG_ERROR("\tFailed to unlock the HM.\n");
    }
    HWManager::Reboot();
}

static void HMWdTimeoutHandler(void* pParam) {
    LOG_ERROR(
        "Error while addind or removing a watchdog: error %d.\n",
        (uint32_t)pParam
    );
    HWManager::Reboot();
}

static void HMRTTaskCreateHandler(void* pParam) {
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("Failed to create the RT HM Task timer.\n");
    }
    else if (1 == (uint32_t)pParam) {
        LOG_ERROR("Failed to create the RT HM Task.\n");
    }
    else {
        LOG_ERROR("Unknown RT HM Task error.\n");
    }
    HWManager::Reboot();
}

static void HMRTTaskWait(void* pParam) {
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("Failed to notify the RT HM Task timer.\n");
    }
    else if (1 == (uint32_t)pParam) {
        LOG_ERROR("Failed to wait for the RT HM Task timer.\n");
    }
    else {
        LOG_ERROR("Unknown RT HM Task error.\n");
    }
    HWManager::Reboot();
}

static void HMRTTaskDeadlineMissHandler(void* pParam) {
    LOG_ERROR("RT HM Task deadline miss.\n");
    HWManager::Reboot();
}

static void HMSettingsCreateHandler(void* pParam) {
    LOG_ERROR("Failed to instanciate Settings. Error: %d\n", (uint32_t)pParam);
    HWManager::Reboot();
}

static void HMSettingsLockHandler(void* pParam) {
    LOG_ERROR("Settings lock error:\n");
    if ((bool)pParam) {
        LOG_ERROR("\tFailed to lock the settings.\n");
    }
    else {
        LOG_ERROR("\tFailed to unlock the settings.\n");
    }
    HWManager::Reboot();
}

static void HMSettingsAccessLoadHandler(void* pParam) {
    S_HMParamSettingAccess* pAccessParam;
    E_Return                result;
    Settings*               pSettings;

    pAccessParam = (S_HMParamSettingAccess*)pParam;

    LOG_ERROR(
        "Failed to load setting %s. Applying default value.\n",
        pAccessParam->kpSettingName
    );

    /* Propagate the default setting */
    pSettings = SystemState::GetInstance()->GetSettings();

    result = pSettings->GetDefault(
        pAccessParam->kpSettingName,
        (uint8_t*)pAccessParam->pSettingBuffer,
        pAccessParam->settingBufferSize
    );
    if (E_Return::NO_ERROR != result) {
        HMSettingsAccessDefaultHandler(pParam);
    }

    /* Apply the default setting */
    result = pSettings->SetSettings(
        pAccessParam->kpSettingName,
        (uint8_t*)pAccessParam->pSettingBuffer,
        pAccessParam->settingBufferSize
    );
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR(
            "Failed to set default setting %s.",
            pAccessParam->kpSettingName
        );
        HWManager::Reboot();
    }

    /* Commit */
    result = pSettings->Commit();
    if (E_Return::NO_ERROR != result) {
        LOG_ERROR(
            "Failed to commit default setting %s.",
            pAccessParam->kpSettingName
        );
        HWManager::Reboot();
    }
}

static void HMSettingsAccessStoreHandler(void* pParam) {
    S_HMParamSettingAccess* pAccessParam;

    pAccessParam = (S_HMParamSettingAccess*)pParam;

    LOG_ERROR(
        "Failed to set value for setting %s.\n",
        pAccessParam->kpSettingName
    );

    HWManager::Reboot();
}

static void HMSettingsAccessCommitHandler(void* pParam) {
    LOG_ERROR("Failed to commit settings. Error %d\n", (uint32_t)pParam);

    HWManager::Reboot();
}

static void HMSettingsAccessDefaultHandler(void* pParam) {
    S_HMParamSettingAccess* pAccessParam;

    pAccessParam = (S_HMParamSettingAccess*)pParam;

    LOG_ERROR(
        "Failed to get default value for setting %s.\n",
        pAccessParam->kpSettingName
    );
    HWManager::Reboot();
}

static void HMWifiCreateHandler(void* pParam) {
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("Failed to instanciate the WiFi module.\n");
    }
    else {
        LOG_ERROR(
            "Failed to create the Web Servers, error: %d\n",
            (uint32_t)pParam
        );
    }
    HWManager::Reboot();
}

static void HMWebStartHandler(void* pParam) {
    LOG_ERROR("Failed to start the Web Servers, error: %d\n", (uint32_t)pParam);
    HWManager::Reboot();
}

static void HMAddActionHandler(void* pParam) {
    LOG_ERROR("Failed to add HM action, error: %d\n", (uint32_t)pParam);
    HWManager::Reboot();
}

static void HMActionQCreateFailedHandler(void* pParam) {
    LOG_ERROR("Failed create HM action queue, error: %d\n", (uint32_t)pParam);
    HWManager::Reboot();
}

static void HMActionTaskCreateHandler(void* pParam) {
    if (0 == (uint32_t)pParam) {
        LOG_ERROR("Failed to create the HM action queue.\n");
    }
    else {
        LOG_ERROR(
            "Failed to create the HM action task, error: %d\n",
            (uint32_t)pParam
        );
    }
    HWManager::Reboot();
}

static void HMWebServerNotFoundHandler(void* pParam) {
    S_HMParamWebServerError* pWebParam;

    pWebParam = (S_HMParamWebServerError*)pParam;

    /* Update the page content */
    *(pWebParam->pPage) = std::string("<h1>HM Error: ") +
        pWebParam->pPageURL +
        std::string(" not found</h1>");
}

static void HMAPIServerNotFoundHandler(void* pParam) {
    S_HMParamAPIServerError* pAPIParam;

    pAPIParam = (S_HMParamAPIServerError*)pParam;

    /* Update the page content */
    *(pAPIParam->pResponse) = std::string("{\"result\": ") +
        std::to_string(E_APIResult::API_RES_HM_ERROR) +
        std::string(", \"msg\": \"Unknown API ") +
        pAPIParam->pAPIURL +
        std::string(".\"}");
}

static void HMRemoveActionHandler(void* pParam) {
    LOG_ERROR("Failed to remove HM action, error: %d\n",(uint32_t)pParam);
    HWManager::Reboot();
}

static void HMWiFiStopHandler(void* pParam) {
    (void)pParam;
    LOG_ERROR("Failed to stop the WiFi module.\n");
    HWManager::Reboot();
}

static void HMMainLoopDeadlineMissHandler(void* pParam) {
    (void)pParam;
    LOG_ERROR("Main Loop has missed its deadline.\n");
    HWManager::Reboot();
}

static void HMIOButtonLockHandler(void* pParam) {
    LOG_ERROR("IO Button lock error:\n");
    if ((bool)pParam) {
        LOG_ERROR("\tFailed to lock.\n");
    }
    else {
        LOG_ERROR("\tFailed to unlock.\n");
    }
    HWManager::Reboot();
}

static void HMIOTaskCreateHandler(void* pParam) {
    LOG_ERROR("Failed to create the IO task.\n");
    HWManager::Reboot();
}

static void HMIOTaskDeadlineMissHandler(void* pParam) {
    LOG_ERROR("IO Task deadline miss. Error %d\n", (uint32_t)pParam);
    HWManager::Reboot();
}

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/

HealthMonitor::HealthMonitor(void) noexcept {
    this->_lastWDId = 0;
    this->_lastReporterId = 0;

    this->_wdLock = xSemaphoreCreateMutex();
    if (nullptr == this->_wdLock) {
        LOG_ERROR("Failed to initialize Health Monitor Watchdogs lock.\n");
        HWManager::Reboot();
    }
    this->_reportersLock = xSemaphoreCreateMutex();
    if (nullptr == this->_reportersLock) {
        LOG_ERROR("Failed to initialize Health Monitor Reporters lock.\n");
        HWManager::Reboot();
    }

    /* Add to system state */
    SystemState::GetInstance()->SetHealthMonitor(this);

    /* Initialize the HM tasks */
    RealTimeTaskInit();
    ActionsTaskInit();
}

E_Return HealthMonitor::AddWatchdog(Timeout* pTimeout, uint32_t& rId) noexcept {
    E_Return error;

    /* Check settings */
    if (0 != pTimeout->GetNextWatchdogEvent()) {
        if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
            if (this->_lastWDId < UINT32_MAX) {
                try {
                    this->_watchdogs.emplace(
                        std::make_pair(this->_lastWDId, pTimeout)
                    );
                    rId = this->_lastWDId++;
                    error = E_Return::NO_ERROR;
                }
                catch (std::exception& rExc) {
                    error = E_Return::ERR_UNKNOWN;
                }
            }
            else {
                error = E_Return::ERR_MEMORY;
            }

            if (pdPASS != xSemaphoreGive(this->_wdLock)) {
                HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
            }
        }
        else {
            LOG_ERROR("Failed to acquire watchdogs lock.\n");
            error = E_Return::ERR_HM_TIMEOUT;
        }
    }
    else {
        error = E_Return::ERR_INVALID_PARAM;
    }

    return error;
}

E_Return HealthMonitor::RemoveWatchdog(const uint32_t kId) noexcept {
    E_Return                                         error;
    std::unordered_map<uint32_t, Timeout*>::iterator it;

    if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
        it = this->_watchdogs.find(kId);
        if (this->_watchdogs.end() != it) {
            try {
                this->_watchdogs.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_wdLock)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
        }
    }
    else {
        LOG_ERROR("Failed to acquire watchdogs lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::AddReporter(HMReporter* pReporter,
                                    uint32_t&   rId) noexcept {
    E_Return error;

    /* Check settings */
    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        if (this->_lastReporterId < UINT32_MAX) {
            try {
                this->_reporters.emplace(
                    std::make_pair(this->_lastReporterId, pReporter)
                );
                rId = this->_lastReporterId++;
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            error = E_Return::ERR_MEMORY;
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
        }
    }
    else {
        LOG_ERROR("Failed to acquire reporters lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::RemoveReporter(const uint32_t kId) noexcept {
    E_Return                                            error;
    std::unordered_map<uint32_t, HMReporter*>::iterator it;

    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        it = this->_reporters.find(kId);
        if (this->_reporters.end() != it) {
            try {
                this->_reporters.erase(it);
                error = E_Return::NO_ERROR;
            }
            catch (std::exception& rExc) {
                error = E_Return::ERR_UNKNOWN;
            }
        }
        else {
            error = E_Return::ERR_NO_SUCH_ID;
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
        }
    }
    else {
        LOG_ERROR("Failed to acquire reporters lock.\n");
        error = E_Return::ERR_HM_TIMEOUT;
    }

    return error;
}

E_Return HealthMonitor::AddHMAction(HMReporter* pReporter) noexcept {
    BaseType_t result;
    E_Return   retVal;

    /* Place the action in the actions queue */
    result = xQueueSend(this->_actionsQueue, (void*)&pReporter, 0);
    if (pdPASS != result) {
        /* We should never have too many actions */
        retVal = E_Return::ERR_HM_FULL;
    }
    else {
        retVal = E_Return::NO_ERROR;
    }

    return retVal;
}

void HealthMonitor::ReportHM(const E_HMEvent kEvent,
                             void*           pParam /* = nullptr */) const
noexcept {
    if (HM_EVENT_MAX > kEvent) {
        /* Execute the registered handler */
        sHMHandlers[kEvent](pParam);
    }
    else {
        LOG_ERROR("HM Event unknown: %d.\n", kEvent);
        HWManager::Reboot();
    }
}

void HealthMonitor::RealTimeTaskRoutine(void* pHealthMonitor) noexcept {
    BaseType_t     result;
    HealthMonitor* pHM;
    TickType_t     lastWakeTime;

    /* Get HM instance */
    pHM = (HealthMonitor*)pHealthMonitor;

    /* First tick */
    pHM->_pTimeout->Tick();
    lastWakeTime = xTaskGetTickCount();

    while (true) {
        /* Manage deadline miss */
        if (pHM->_pTimeout->Check()) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_RT_TASK_DEADLINE_MISS, 0);
        }
        pHM->_pTimeout->Tick();

        /* Perform HM checks */
        pHM->CheckWatchdogs();
        pHM->CheckReporters();

        /* Wait for period */
        result = xTaskDelayUntil(
            &lastWakeTime,
            HW_RT_TASK_PERIOD_NS / 1000000 / portTICK_PERIOD_MS
        );
        if (pdPASS != result) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_RT_TASK_DEADLINE_MISS, 1);
        }
    }
}

void HealthMonitor::HMActionTaskRoutine(void* pHealthMonitor) noexcept {
    HealthMonitor* pHM;
    HMReporter*    pReporter;
    BaseType_t     result;

    pHM = (HealthMonitor*)pHealthMonitor;

    while (true) {
        /* Get the next job */
        result = xQueueReceive(
            pHM->_actionsQueue,
            (void*)&pReporter,
            portMAX_DELAY
        );

        if (pdPASS != result) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_ACTION_RECV_FAILED, result);
        }

        /* Execute the action */
        pReporter->ExecuteAction();
    }
}

void HealthMonitor::CheckWatchdogs(void) const noexcept {
    uint64_t                                               currentTime;
    std::unordered_map<uint32_t, Timeout*>::const_iterator it;

    /* Check for watchdogs */
    currentTime = HWManager::GetTime();
    if (pdPASS == xSemaphoreTake(this->_wdLock, WD_LOCK_TIMEOUT_TICKS)) {
        for (it = this->_watchdogs.begin();
             this->_watchdogs.end() != it;
             ++it) {
            /* Check for dealine miss */
            if (it->second->GetNextWatchdogEvent() < currentTime) {
                it->second->ExecuteHandler();
            }
        }

        if (pdPASS != xSemaphoreGive(this->_wdLock)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
        }
    }
    else {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 0);
    }
}

void HealthMonitor::CheckReporters(void) const noexcept {
    uint64_t                                                  currentTime;
    std::unordered_map<uint32_t, HMReporter*>::const_iterator it;

    /* Check for HM reporters */
    currentTime = HWManager::GetTime();
    if (pdPASS == xSemaphoreTake(this->_reportersLock, WD_LOCK_TIMEOUT_TICKS)) {
        for (it = this->_reporters.begin();
             this->_reporters.end() != it;
             ++it) {
            it->second->HealthCheck(currentTime);
        }

        if (pdPASS != xSemaphoreGive(this->_reportersLock)) {
            HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 1);
        }
    }
    else {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_HM_LOCK, 0);
    }
}

void HealthMonitor::RealTimeTaskInit(void) noexcept {
    BaseType_t result;

    /* Create the timeout */
    this->_pTimeout = new Timeout(
        HW_RT_TASK_PERIOD_NS + HW_RT_TASK_PERIOD_TOLERANCE_NS,
        HW_RT_TASK_WD_TIMEOUT_NS,
        HealthMonitor::DeadlineMissHandler
    );
    if (nullptr == this->_pTimeout) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_RT_TASK_CREATE, 0);
    }

    /* Create the real-time high-priority task */
    result = xTaskCreatePinnedToCore(
        HealthMonitor::RealTimeTaskRoutine,
        HW_RT_TASK_NAME,
        HW_RT_TASK_STACK,
        this,
        HW_RT_TASK_PRIO,
        &this->_RTTaskHandle,
        HW_RT_TASK_CORE
    );
    if (pdPASS != result) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_RT_TASK_CREATE, 1);
    }
}

void HealthMonitor::ActionsTaskInit(void) noexcept {
    BaseType_t result;

    /* Create the actions queue */
    this->_actionsQueue = xQueueCreate(
        HM_ACTIONS_TASK_QUEUE_LENGTH,
        sizeof(HMReporter*)
    );
    if (nullptr == this->_actionsQueue) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_ACTION_TASK_CREATE, 0);
    }

    /* Create the real-time high-priority task */
    result = xTaskCreatePinnedToCore(
        HMActionTaskRoutine,
        HM_ACTIONS_TASK_NAME,
        HM_ACTIONS_TASK_STACK,
        this,
        HM_ACTIONS_TASK_PRIO,
        &this->_actionsTaskHandle,
        HM_ACTIONS_TASK_CORE
    );
    if (pdPASS != result) {
        HM_REPORT_EVENT(E_HMEvent::HM_EVENT_ACTION_TASK_CREATE, 1);
    }
}

void HealthMonitor::DeadlineMissHandler(void) noexcept {
    HM_REPORT_EVENT(E_HMEvent::HM_EVENT_RT_TASK_DEADLINE_MISS, 3);
}