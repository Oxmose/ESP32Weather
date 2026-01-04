/*******************************************************************************
 * @file Errors.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief Errors definitions.
 *
 * @details Errors definitions. This file contains the list of errors used in
 * the firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __ERRORS_H__
#define __ERRORS_H__

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
/** @brief Defines the list of return values available */
typedef enum {
    /** @brief No error, the action succeeded. */
    NO_ERROR,
    /** @brief WiFi Connection Error */
    ERR_WIFI_CONN,
    /** @brief Error when initializing the settings. */
    ERR_SETTING_INIT,
    /** @brief Error when a setting is not found. */
    ERR_SETTING_NOT_FOUND,
    /** @brief Error when a setting lock timed out. */
    ERR_SETTING_TIMEOUT,
    /** @brief Invalid setting name. */
    ERR_SETTING_INVALID,
    /** @brief Failed to save settings. */
    ERR_SETTING_COMMIT_FAILURE,
    /** @brief A memory related error occured. */
    ERR_MEMORY,
    /** @brief A specified ID was or item was not found. */
    ERR_NO_SUCH_ID,
    /** @brief Health Monitor action timed out. */
    ERR_HM_TIMEOUT,
    /** @brief Invalid parameter. */
    ERR_INVALID_PARAM,
    /** @brief Error while creating the web server task. */
    ERR_WEB_SERVER_TASK,
    /** @brief Error while creating the api server task. */
    ERR_API_SERVER_TASK,
    /** @brief Error while creating the web server lock. */
    ERR_WEB_SERVER_LOCK,
    /** @brief Error while creating the api server lock. */
    ERR_API_SERVER_LOCK,
    /** @brief HM actions cannot be added. */
    ERR_HM_FULL,
    /** @brief WiFi settings error: invalid switches */
    ERR_WIFI_INVALID_SWITCHES,
    /** @brief WiFi settings error: invalid SSID */
    ERR_WIFI_INVALID_SSID,
    /** @brief WiFi settings error: invalid passwerd */
    ERR_WIFI_INVALID_PASSWORD,
    /** @brief WiFi settings error: invalid ip */
    ERR_WIFI_INVALID_IP,
    /** @brief WiFi settings error: invalid gateway */
    ERR_WIFI_INVALID_GATEWAY,
    /** @brief WiFi settings error: invalid subnet */
    ERR_WIFI_INVALID_SUBNET,
    /** @brief WiFi settings error: invalid dns */
    ERR_WIFI_INVALID_DNS,
    /** @brief WiFi settings error: invalid ports */
    ERR_WIFI_INVALID_PORTS,
    /** @brief Error when an action lock timed out. */
    ERR_BTN_ACTION_TIMEOUT,
    /** @brief Unknown error. */
    ERR_UNKNOWN
} E_Return;

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
/* None */

#endif /* #ifndef __ERRORS_H__ */