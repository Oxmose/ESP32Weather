/*******************************************************************************
 * @file SensorsPageHandler.h
 *
 * @see SensorsPageHandler.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 19/12/2025
 *
 * @version 1.0
 *
 * @brief Sensors page handler.
 *
 * @details Sensors page handler. This file defines the sensors page handler
 * used to generate the sensors page for the web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __SENSORS_PAGE_HANDLER_H__
#define __SENSORS_PAGE_HANDLER_H__

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
 * @brief The SensorsPageHandler class.
 *
 * @details The SensorsPageHandler class provides the necessary functions to
 * handle sensors page requests.
 */
class SensorsPageHandler : public PageHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Destroys a SensorsPageHandler.
         *
         * @details Destroys a SensorsPageHandler. Since only one object is
         * allowed in the firmware, the destructor will generate a critical
         * error.
         */
        virtual ~SensorsPageHandler(void) noexcept;

        /**
         * @brief Generates the sensors page.
         *
         * @details Generate the sensors page. The function generates the
         * sensors page and title based on the current state of the system.
         *
         * @param[out] rPageTitle The title to give to the sensors page.
         * @param[out] rPageBody The body to give to the sensors page.
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

#endif /* #ifndef __SENSORS_PAGE_HANDLER_H__ */