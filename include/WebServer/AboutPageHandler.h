/*******************************************************************************
 * @file AboutPageHandler.h
 *
 * @see AboutPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief About page handler.
 *
 * @details About page handler. This file defines the about page handler
 * used to generate the about page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __ABOUT_PAGE_HANDLER_H__
#define __ABOUT_PAGE_HANDLER_H__

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
 * @brief The AboutPageHandler class.
 *
 * @details The AboutPageHandler class provides the necessary functions to
 * handle about page requests.
 */
class AboutPageHandler : public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief AboutPageHandler destructor.
         *
         * @details AboutPageHandler destructor. Releases the used resources.
         */
        virtual ~AboutPageHandler(void) noexcept;

        /**
         * @brief Generates the about page.
         *
         * @details Generate the about page. The function generates the
         * about page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the about page.
         * @param[out] rPageBody The body to give to the about page.
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

#endif /* #ifndef __ABOUT_PAGE_HANDLER_H__ */