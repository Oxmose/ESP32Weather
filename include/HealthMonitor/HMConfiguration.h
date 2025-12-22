/*******************************************************************************
 * @file HMConfiguration.h
 *
 * @see HealthMonitor.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 16/12/2025
 *
 * @version 1.0
 *
 * @brief Firmware Health Monitor configuration.
 *
 * @details Firmware Health Monitor configuration. Provides the list of event
 * and handlers to manage HM events.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __HM_CONFIGURATION_H__
#define __HM_CONFIGURATION_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
/* None */

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
/** @brief Defines the HM events. */
typedef enum {
    /** @brief HM Event: API Server has not attached Web Server. */
    HM_EVENT_API_SERVER_NO_SERVER,
    /** @brief HM Event: API Server lock error. */
    HM_EVENT_API_SERVER_LOCK,
    /** @brief HM Event: Web Server has initialized corretly. */
    HM_EVENT_WEB_SERVER_INIT_ERROR,
    /** @brief HM Event: Web Server lock error. */
    HM_EVENT_WEB_SERVER_LOCK,
    /** @brief HM Event: MAC Address error. */
    HM_EVENT_HW_MAC_NOT_AVAIL,
    /** @brief HM Event: Health Monitor lock error. */
    HM_EVENT_HM_LOCK,
    /** @brief HM Event: Watchdog Timeout error. */
    HM_EVENT_WD_TIMEOUT,
    /** @brief HM Event: HM Reat Time Task creation error. */
    HM_EVENT_RT_TASK_CREATE,
    /** @brief HM Event: HM Reat Time Task wait error. */
    HM_EVENT_RT_TASK_WAIT,
    /** @brief HM Event: HM Reat Time Task deadline miss. */
    HM_EVENT_RT_TASK_DEADLINE_MISS,
    /** @brief HM Event: Settings lock error. */
    HM_EVENT_SETTINGS_LOCK,
    /** @brief HM Event: Settings instantiation error. */
    HM_EVENT_SETTINGS_CREATE,
    /** @brief HM Event: Settings access error. */
    HM_EVENT_SETTINGS_LOAD,
    /** @brief HM Event: Settings access error. */
    HM_EVENT_SETTINGS_STORE,
    /** @brief HM Event: Settings access error. */
    HM_EVENT_SETTINGS_DEFAULT,
    /** @brief HM Event: WiFi Module creation error. */
    HM_EVENT_WIFI_CREATE,
    /** @brief HM Event: Web Servers start error. */
    HM_EVENT_WEB_START,
    /** @brief HM Event: Adding action failed. */
    HM_EVENT_HM_ADD_ACTION,
    /** @brief HM Event: Receiving the action queue failed. */
    HM_EVENT_HM_ACTION_QRECV_FAILED,
    /** @brief HM Event: Creating the action queue or task failed. */
    HM_EVENT_ACTION_TASK_CREATE,
    /** @brief HM Event: Registered WebServer page handler not found. */
    HM_EVENT_WEB_SERVER_NOT_FOUND,
    /** @brief HM Event: Failed to initialize the system state. */
    HM_EVENT_SYSTEM_STATE_INIT,

#ifdef HM_TEST_EVENT
    /** @brief HM Event: Test event */
    HM_EVENT_TEST,
#endif

    /** @brief Maximal allowed event ID. */
    HM_EVENT_MAX
} E_HMEvent;

/** @brief Defines the HM parameter for the HM_EVENT_SETTINGS_ACCESS event. */
typedef struct {
    /** @brief The setting that was accessed. */
    const char* kpSettingName;
    /** @brief The buffer that will receive the corrected value. */
    void* pSettingBuffer;
    /** @brief Setting buffer size in bytes. */
    size_t settingBufferSize;
} S_HMParamSettingAccess;

/**
 * @brief Defines the HM parameter for the HM_EVENT_WEB_SERVER_NOT_FOUND event.
 */
typedef struct {
    /** @brief The URL of the requested page. */
    const char* pPageURL;

    /** @brief The string pointer to the page buffer to update. */
    std::string* pPage;
} S_HMParamWebServerError;

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
 * CLASSES
 ******************************************************************************/
/* None */

#endif /* #ifndef __HM_CONFIGURATION_H__ */