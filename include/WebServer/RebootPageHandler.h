/*******************************************************************************
 * @file RebootHandler.h
 *
 * @see RebootHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Reboot page handler.
 *
 * @details Reboot page handler. This file defines the reboot page handler
 * used to generate the reboot page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __REBOOT_PAGE_HANDLER_H__
#define __REBOOT_PAGE_HANDLER_H__

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
 * @brief The RebootPageHandler class.
 *
 * @details The RebootPageHandler class provides the necessary functions to
 * handle reboot page requests.
 */
class RebootPageHandler : public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Creates a RebootPageHandler.
         *
         * @details Creates a RebootPageHandler. Allocates the resources for
         * the handler.
         */
        RebootPageHandler(WebServerHandlers* pHandlers) noexcept;

        /**
         * @brief Destroys a RebootPageHandler.
         *
         * @details Destroys a RebootPageHandler. Since only one object is
         * allowed in the firmware, the destructor will generate a critical
         * error.
         */
        virtual ~RebootPageHandler(void) noexcept;

        /**
         * @brief Generates the reboot page.
         *
         * @details Generate the reboot page. The function generates the
         * reboot page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the reboot page.
         * @param[out] rPageBody The body to give to the reboot page.
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

#endif /* #ifndef __REBOT_PAGE_HANDLER_H__ */