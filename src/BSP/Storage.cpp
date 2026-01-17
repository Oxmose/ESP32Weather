/*******************************************************************************
 * @file Storage.cpp
 *
 * @see Storage.h
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

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#define DISABLE_FS_H_WARNING
#include <BSP.h>         /* HW Manager */
#include <SdFat.h>       /* SD card driver */
#include <Logger.h>      /* Logger services */
#include <SystemState.h> /* System state object */

/* Header File */
#include <Storage.h>

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
 * CLASS METHODS
 ******************************************************************************/
Storage::Storage(void) noexcept {
    /* Create the configuration */
    SdSpiConfig config = SdSpiConfig(
        GPIO_SPI_CS_SD,
        SHARED_SPI,
        SD_SCK_MHZ(40),
        HWManager::GetSPIBus()
    );

    /* Init the SD card */
    if (!this->_sdCard.begin(config)) {
        PANIC(
            "Failed to initialize the SD card (%d).\n",
            this->_sdCard.sdErrorCode()
        );
    }

    if(this->_sdCard.card()->type() != (uint8_t)-1)
    {
        LOG_DEBUG(
            "SD card detected: %d\n",
            this->_sdCard.card()->type()
        );
    }
    else
    {
        PANIC("No SD card detected\n");
    }

    SystemState::GetInstance()->SetStorage(this);
}

Storage::~Storage(void) noexcept {
    PANIC("Tried to destroy the Storage Manager.\n");
}

FsFile Storage::Open(const char* kpPath, const oflag_t kpMode) noexcept {
    FsFile file;

    file.open(kpPath, kpMode);

    return file;
}

bool Storage::Remove(const char* kpPath) noexcept {
    return this->_sdCard.remove(kpPath);
}

void Storage::Format(void) noexcept {
    if (!this->_sdCard.format()) {
        PANIC("Failed to format SD Card.\n");
    }
}