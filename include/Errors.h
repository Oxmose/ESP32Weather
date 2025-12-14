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