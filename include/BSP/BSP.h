/*******************************************************************************
 * @file BSP.h
 *
 * @see BSP.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 14/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the hardware layer.
 *
 * @details This file defines the hardware layer. This layer provides services
 * to interact with the ESP32 module hardware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __BSP_H__
#define __BSP_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <string>    /* Standard string */
#include <cstdint>   /* Standard int types */
#include <Arduino.h> /* Arduino Framework */

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
/** @brief Define the GPIO pinmux behavior */
typedef enum {
    /** @brief Reset button GPIO */
    GPIO_BTN_RESET_MUX = INPUT_PULLDOWN,
} E_GPIOPull;

/** @brief Defines the GPIO routing */
typedef enum {
    /** @brief Reset button GPIO */
    GPIO_BTN_RESET = GPIO_NUM_4,
    /** @brief RGB led GPIO */
    GPIO_LED_INFO = BUILTIN_LED
} E_GPIORouting;

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
 * @brief Hardware manager class.
 *
 * @details Hardware manager class. This class provides the services
 * to access hwardware information, features and interract directly with the
 * different components of the ESP32 module.
 */
class HWManager
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Returns the ESP32 unique hardware ID.
         *
         * @details Returns the ESP32 unique hardware ID. The unique ID is
         * composed of a basic string (defined directly in the source code) and
         * a part of the ESP32 mac address.
         *
         * @return The ESP32 unique hardware ID.
         */
        static const char* GetHWUID(void) noexcept;

        /**
         * @brief Returns the ESP32 MAC address.
         *
         * @details Returns the ESP32 MAC address. The WiFi MAC address is
         * used.
         *
         * @return The ESP32 MAC address.
         */
        static const char* GetMacAddress(void) noexcept;

        /**
         * @brief Returns the time in nanoseconds.
         *
         * @details Returns the time in nanoseconds
         * . The 64bit value is
         * software managed as the ESP32 only has 32bits resolution for the
         * time.
         *
         * @return The execution time is returned.
         */
        static uint64_t GetTime(void) noexcept;

        /**
         * @brief Delays the calling thread.
         *
         * @details Delays the calling thread, if possible the passive option
         * will be used. If not, active wait will be made.
         *
         * @param[in] kDelayNs The details to wait in nanoseconds.
         */
        static void DelayExecNs(const uint64_t kDelayNs) noexcept;

        /**
         * @brief Reboots the board.
         *
         * @details Reboots the board. The reboot is effective immediatelly and
         * all pending operations should be completed before calling this
         * function.
         */
        static void Reboot(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief Stores the hwardware unique ID. */
        static std::string _SHWUID;
        /** @brief Stores the MAC address. */
        static std::string _SMACADDR;
};


#endif /* #ifndef __BSP_H__ */