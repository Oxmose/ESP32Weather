/*******************************************************************************
 * @file PageHandler.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Page handler interface.
 *
 * @details Page handler interface. The interface defines what a page handler
 * should be able to provide.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __PAGE_HANDLER_H__
#define __PAGE_HANDLER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>              /* Standard strings */
#include <Errors.h>            /* Error definitions */

/* Forward declarations */
class WebServerHandlers;

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
 * @brief The PageHandler interface.
 *
 * @details The PageHandler interface provides the contract that all page
 * handler implementation should provide.
 */
class PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief PageHandler constructor.
         *
         * @details PageHandler constructor. The page handler should implement
         * this method.
         */
        PageHandler(WebServerHandlers* pHandlers) noexcept {
            this->_pHandlers = pHandlers;
        }

        /**
         * @brief PageHandler destructor.
         *
         * @details PageHandler destructor. The page handler should implement
         * this method.
         */
        virtual ~PageHandler(void) noexcept {};

        /**
         * @brief Generates the page.
         *
         * @details Generate the page. The function should generate the page and
         * fill the string in parameters with the content of the page title and
         * body.
         *
         * @param[out] rPageTitle The title to give to the page.
         * @param[out] rPageBody The body to give to the page.
         */
        virtual void Generate(std::string& rPageTitle,
                              std::string& rPageBody) noexcept = 0;

        /**
         * @brief Returns the handlers with wich the page was generated.
         *
         * @details Returns the handlers with wich the page was generated.
         *
         * @return The function returns the handlers with wich the page was
         * generated.
         */
        virtual WebServerHandlers* GetHandlers(void) const noexcept {
            return this->_pHandlers;
        }

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        WebServerHandlers* _pHandlers;
};

#endif /* #ifndef __PAGE_HANDLER_H__ */