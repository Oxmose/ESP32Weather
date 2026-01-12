/*******************************************************************************
 * @file Storage.h
 *
 * @see Storage.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 08/01/2026
 *
 * @version 1.0
 *
 * @brief The Storage classe provides the functionalities to access the SD
 * card.
 *
 * @details The Storage class provides the functionalities to access the SD
 * card. It abstracts the storage medium.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __BSP_STORAGE_H__
#define __BSP_STORAGE_H__

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#define DISABLE_FS_H_WARNING
#include <SdFat.h>  /* SD Card driver */

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
 * @brief The Storage class.
 *
 * @details The Storage class provides the functionalities to access the SD
 * card. It abstracts the storage medium.
 */
class Storage
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Storage classe constructor.
         *
         * @details Storage classe constructor. It initializes the underlying
         * persistent storage method and the resources used by the storage.
         */
        Storage(void) noexcept;

        /**
         * @brief Destroys a Storage.
         *
         * @details Destroys a Storage. Since only one object is allowed
         * in the firmware, the destructor will generate a critical error.
         */
        ~Storage(void) noexcept;

        /**
         * @brief Opens a file.
         *
         * @details Opens a file. The file is opened with the provided path
         * and the provided open mode.
         *
         * @param[in] kpPath The path to the file to open.
         * @param[in] kpMode The open mode to use.
         *
         * @return The function returns a File object corresponding to the
         * opened file.
         */
        FsFile Open(const char* kpPath, const oflag_t kpMode) noexcept;

        /**
         * @brief Removes a file.
         *
         * @details Removes a file. The file pointed by the provided path is
         * removed. If no file exist, this function has no effect.
         */
        bool Remove(const char* kpPath) noexcept;

        /**
         * @brief Formats the persistent memory.
         *
         * @details Formats the persisten memory. This will wipe all data
         * present on the underlying storage device.
         */
        void Format(void) noexcept;

    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /** @brief Stores the SD card instance */
        SdFs _sdCard;
};


#endif /* #ifndef __BSP_STORAGE_H__ */