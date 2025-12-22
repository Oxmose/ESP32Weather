/*******************************************************************************
 * @file APIHandler.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 22/12/2025
 *
 * @version 1.0
 *
 * @brief API handler interface.
 *
 * @details API handler interface. The interface defines what a API handler
 * should be able to provide.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __API_HANDLER_H__
#define __API_HANDLER_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>   /* Standard strings */
#include <Errors.h> /* Error definitions */

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
 * @brief The APIHandler interface.
 *
 * @details The APIHandler interface provides the contract that all API
 * handler implementation should provide.
 */
class APIHandler {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief APIHandler destructor.
         *
         * @details APIHandler destructor. The API handler should implement
         * this method.
         */
        virtual ~APIHandler(void) noexcept {};

        /**
         * @brief Handle the API call.
         *
         * @details Generate the page. The function should generate API response
         * and process the API call.
         *
         * @param[out] rReponse The response to the API call.
         */
        virtual void Handle(std::string& rReponse) noexcept = 0;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /* None */
};

#endif /* #ifndef __API_HANDLER_H__ */