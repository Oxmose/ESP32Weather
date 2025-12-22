/*******************************************************************************
 * @file SettingsPageHandler.h
 *
 * @see SettingsPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Settings page handler.
 *
 * @details Settings page handler. This file defines the settings page handler
 * used to generate the settings page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __SETTINGS_PAGE_HANDLER_H__
#define __SETTINGS_PAGE_HANDLER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>        /* Standard string */
#include <Errors.h>      /* Errors definitions */
#include <PageHandler.h> /* Page Handler interface */

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
 * @brief The SettingsPageHandler class.
 *
 * @details The SettingsPageHandler class provides the necessary functions to
 * handle settings page requests.
 */
class SettingsPageHandler :public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief SettingsPageHandler destructor.
         *
         * @details SettingsPageHandler destructor. Releases the used resources.
         */
        virtual ~SettingsPageHandler(void) noexcept;

        /**
         * @brief Generates the settings page.
         *
         * @details Generate the settings page. The function generates the
         * settings page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the settings page.
         * @param[out] rPageBody The body to give to the settings page.
         *
         */
        virtual void Generate(std::string& rPageTitle,
                              std::string& rPageBody) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Generates the networks settings.
         *
         * @details Generates the networks settings. The content is added to the
         * page buffer given as parameter.
         *
         * @param[out] rPageBuffer The page buffer to fill.
         */
        void GenerateNetworkSettings(std::string& rPageBuffer) const noexcept;
};

#endif /* #ifndef __SETTINGS_PAGE_HANDLER_H__ */