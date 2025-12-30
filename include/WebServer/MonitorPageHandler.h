/*******************************************************************************
 * @file MonitorPageHandler.h
 *
 * @see MonitorPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Monitor page handler.
 *
 * @details Monitor page handler. This file defines the monitor page handler
 * used to generate the monitor page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __MONITOR_PAGE_HANDLER_H__
#define __MONITOR_PAGE_HANDLER_H__

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
 * @brief The MonitorPageHandler class.
 *
 * @details The MonitorPageHandler class provides the necessary functions to
 * handle monitor page requests.
 */
class MonitorPageHandler : public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief MonitorPageHandler destructor.
         *
         * @details MonitorPageHandler destructor. Releases the used resources.
         */
        virtual ~MonitorPageHandler(void) noexcept;

        /**
         * @brief Generates the monitor page.
         *
         * @details Generate the monitor page. The function generates the
         * monitor page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the monitor page.
         * @param[out] rPageBody The body to give to the monitor page.
         *
         */
        virtual void Generate(std::string& rPageTitle,
                              std::string& rPageBody) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /* None */
};

#endif /* #ifndef __MONITOR_PAGE_HANDLER_H__ */