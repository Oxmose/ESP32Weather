/*******************************************************************************
 * @file IndexPageHandler.h
 *
 * @see IndexPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Index page handler.
 *
 * @details Index page handler. This file defines the index page handler used
 * to generate the index page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __INDEX_PAGE_HANDLER_H__
#define __INDEX_PAGE_HANDLER_H__

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
 * @brief The IndexPageHandler class.
 *
 * @details The IndexPageHandler class provides the necessary functions to
 * handle index page requests.
 */
class IndexPageHandler :public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief IndexPageHandler destructor.
         *
         * @details IndexPageHandler destructor. Releases the used resources.
         */
        virtual ~IndexPageHandler(void) noexcept;

        /**
         * @brief Generates the index page.
         *
         * @details Generate the index page. The function generates the index
         * page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the index page.
         * @param[out] rPageBody The body to give to the index page.
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
         * @brief Generates the network section.
         *
         * @details Generates the network section. This contains information
         * about the system network status.
         *
         * @param[out] rBuffer The buffer that will receive the section.
         */
        void GenerateNetwork(std::string& rBuffer) const noexcept;

        /**
         * @brief Generates the system section.
         *
         * @details Generates the system section. This contains information
         * about the system general status.
         *
         * @param[out] rBuffer The buffer that will receive the section.
         */
        void GenerateSystem(std::string& rBuffer) const noexcept;
};

#endif /* #ifndef __INDEX_PAGE_HANDLER_H__ */