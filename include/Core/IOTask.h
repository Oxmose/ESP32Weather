/*******************************************************************************
 * @file IOTask.h
 *
 * @see IOTask.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 06/01/2026
 *
 * @version 1.0
 *
 * @brief IO Task class.
 *
 * @details IO Task class. Provides the functionalities to create and execute
 * the IO task responsible of inputs and outputs management.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __CORE_IO_TASK_H__
#define __CORE_IO_TASK_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <Arduino.h>     /* Arduino framework */
#include <Timeout.h>     /* Timeout manager */
#include <SystemState.h> /* System State services. */

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
 * @brief IO Task class.
 *
 * @details IO Task class. Provides the functionalities to create and execute
 * the IO task responsible of inputs and outputs management.
 */
class IOTask {
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief The IO Task constructor.
         *
         * @details The IO Task constructor. Allocates and initializes the
         * task resources.
         */
        IOTask(void) noexcept;

        /**
         * @brief The IO Task destructor.
         *
         * @details The IO Task destructor. Releases the resources used
         * by the IO task.
         */
        ~IOTask(void) noexcept;

        /**
         * @brief Starts the IO task.
         *
         * @details Starts the IO task. This will effectively start to run the
         * IO management.
         */
        void Start(void) noexcept;

        /**
         * @brief Stops the IO task.
         *
         * @details Stops the IO task. This will stop the IO task, that can
         * later be restarted without reinitialization.
         */
        void Stop(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Handles the main loop watchdog trigger.
         *
         * @details Handles the main loop watchdog trigger. Health monitor is
         * used to correct the error.
         */
        static void DeadlineMissHandler(void) noexcept;

        /**
         * @brief IO task routine.
         *
         * @details IO task routine. Executes the periodic IO operations. The
         * routine is executed by a timer interrupt and should keep its
         * execution as minimalistic as possible.
         *
         * @param[in] pParam The IOTask instance pointer to use with the
         * routine.
         */
        static void IOTaskRoutine(void* pParam) noexcept;

        /**
         * @brief IO Task timer handler.
         *
         * @brief IO Task timer handler. The handler will notify the IO task to
         * execute.
         */
        static void IRAM_ATTR IOTaskTimerHandler(void) noexcept;

        /** @brief Deadline miss manager. */
        Timeout* _pTimeout;

        /** @brief System state object instance. */
        SystemState* _pSysState;

        /** @brief Stores the IO task handle. */
        TaskHandle_t _IOTaskHandle;

        /** @brief Stores the started status. */
        bool _started;
};

#endif /* #ifndef __CORE_IO_TASK_H__ */