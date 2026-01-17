/*******************************************************************************
 * @file ModeManager.cpp
 *
 * @see ModeManager.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Firmware mode manager.
 *
 * @details Firmware mode manager. The firmware has two execution mode: nominal
 * and maintenance. The maintenance mode is activated when maintenance is
 * required on the firmware.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>                          /* Hardware services*/
#include <Logger.h>                       /* Firmware logger */
#include <Errors.h>                       /* Error codes */
#include <IOTask.h>                       /* IO Task manager */
#include <rom/rtc.h>                      /* RTC services */
#include <Arduino.h>                      /* Arduino library */
#include <version.h>                      /* Versionning info */
#include <Storage.h>                      /* Storage manager */
#include <Settings.h>                     /* Settings services */
#include <WebServer.h>                    /* Web server services */
#include <WiFiModule.h>                   /* WiFi Module driver */
#include <SystemState.h>                  /* System state */
#include <IOLedManager.h>                 /* IO Led manager */
#include <ResetManager.h>                 /* Reset manager services */
#include <HealthMonitor.h>                /* Health Monitoring */
#include <IOButtonManager.h>              /* IO Button manager */
#include <MaintenanceWebServerHandlers.h> /* Maintenance mode URL handlers */

/* Header file */
#include <ModeManager.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the path to the execution mode setting file. */
#define FIRMWARE_MODE_PATH "rthrws_mode"

/** @brief Defines the maintenance web server port. */
#define MAINTENANCE_WEB_SERVER_PORT 8888

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/
/* None */

/*******************************************************************************
 * MACROS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * STATIC FUNCTIONS DECLARATIONS
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
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
ModeManager::ModeManager(void) noexcept {
    /* Init the mode */
    this->_currentMode = E_Mode::MODE_MAINTENANCE;

    /* Get the last reset reason */
    GetLastReset();
}

ModeManager::~ModeManager(void) noexcept {
    PANIC("Tried to destroy the Mode manager.\n");
}

E_Return ModeManager::SetMode(const E_Mode kMode) noexcept {
    E_Return     retVal;
    FsFile       file;
    Settings*    pSettings;
    SystemState* pSysState;

    pSettings = nullptr;

    pSysState = SystemState::GetInstance();
    if (nullptr != pSysState) {
        pSettings = pSysState->GetSettings();
    }

    if (nullptr != pSettings) {
        file = SystemState::GetInstance()->GetStorage()->Open(
            FIRMWARE_MODE_PATH,
            O_RDWR | O_CREAT
        );
    }

    if (file.isOpen()) {
        if (sizeof(E_Mode) !=
            file.write(&kMode, sizeof(E_Mode))) {
            retVal = E_Return::ERR_MODE_FILE_WRITE;
        }
        else {
            retVal = E_Return::NO_ERROR;
        }

        if (!file.close()) {
            LOG_ERROR("Failed to close mode file.\n");
        }
    }
    else {
        LOG_ERROR("Failed to write execution mode.\n");
        retVal = E_Return::ERR_MODE_FILE_OPEN;
    }

    if (E_Return::NO_ERROR == retVal) {
        HWManager::Reboot(false);
    }

    return retVal;
}

E_Mode ModeManager::GetMode(void) const noexcept {
    return this->_currentMode;
}

void ModeManager::StartFirmware(void) noexcept {
    SystemState* pSystemState;
    Storage*     pStorage;
    FsFile       file;

    /* Init system state */
    pSystemState = SystemState::GetInstance();
    if (nullptr != pSystemState) {
        /* Set system state */
        pSystemState->SetModeManager(this);

        /* Init storage */
        pStorage = pSystemState->GetStorage();
        if (nullptr != pStorage) {
            /* Read the current mode */
            file = pStorage->Open(FIRMWARE_MODE_PATH, O_RDONLY);
            if (file.isOpen()) {
                if (sizeof(E_Mode) !=
                    file.read(&this->_currentMode, sizeof(E_Mode))) {
                    LOG_ERROR("Failed to read execution mode.\n");
                    this->_currentMode = E_Mode::MODE_MAINTENANCE;
                }

                if (!file.close()) {
                    LOG_ERROR("Failed to close mode file.\n");
                }
            }
            else {
                LOG_ERROR("Failed to load execution mode.\n");
                this->_currentMode = E_Mode::MODE_MAINTENANCE;
            }
        }
        else {
            LOG_ERROR("Failed to instanciate the Storage Manager.\n");
            this->_currentMode = E_Mode::MODE_MAINTENANCE;
        }

        /* Check force maintenance */
        if (this->_forceMaintenance) {
            LOG_INFO("Maintenance mode is forced.\n");
            this->_currentMode = E_Mode::MODE_MAINTENANCE;
        }
    }
    else {
        /* Set maintenance mode */
        LOG_ERROR("Failed to instanciate the System State.\n");
        this->_currentMode = E_Mode::MODE_FAULTED;
    }

    /* Load the current mode */
    if (E_Mode::MODE_NOMINAL == this->_currentMode) {
        LOG_INFO("Booting in nominal mode.\n");
        StartNominal();
    }
    else if (E_Mode::MODE_MAINTENANCE == this->_currentMode) {
        LOG_INFO("Booting in maintenance mode.\n");
        StartMaintenance();
    }
    else {
        LOG_ERROR("Faulted mode enacted.\n");
    }
}

void ModeManager::StartNominal(void) const noexcept {
    HealthMonitor*   pHealthMon;
    WiFiModule*      pWifiModule;
    Settings*        pSettings;
    IOButtonManager* pBtnManager;
    IOLedManager*    pLedManager;
    ResetManager*    pResetManager;
    IOTask*          pIOTask;
    E_Return         result;
    uint32_t         resetActionId;

    pHealthMon = new HealthMonitor();
    if (nullptr == pHealthMon) {
        PANIC("Failed to instanciate the Health Monitor.\n");
    }
    pSettings = new Settings();
    if (nullptr == pSettings) {
        PANIC("Failed to instanciate the Settings.\n");
    }
    pWifiModule = new WiFiModule();
    if (nullptr == pWifiModule) {
        PANIC("Failed to instanciate the WiFi Module.\n");
    }
    pBtnManager = new IOButtonManager();
    if (nullptr == pBtnManager) {
        PANIC("Failed to instanciate the IO Button Manager.\n");
    }
    pLedManager = new IOLedManager();
    if (nullptr == pLedManager) {
        PANIC("Failed to instanciate the IO Led Manager.\n");
    }

    /* Setup reset */
    pResetManager = new ResetManager();
    result = pBtnManager->AddAction(pResetManager, resetActionId);
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to add reset action. Error %d\n", result);
    }

    /* Create the IO task */
    pIOTask = new IOTask();
    if (nullptr == pIOTask) {
        PANIC("Failed to instanciate the IO Task.\n");
    }

    /* Initialize the WiFi */
    result = pWifiModule->Start();
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to start the WiFi module. Error: %d\n", result);
    }

    /* Start the web servers */
    result = pWifiModule->StartWebServers();
    if (E_Return::NO_ERROR != result) {
        PANIC("Failed to start the Web Servers. Error: %d\n", result);
    }
}

void ModeManager::StartMaintenance(void) noexcept {
    char   wifiIp[IP_ADDR_SIZE_BYTES + 1];
    String ip;

    /* Disable persistence */
    WiFi.persistent(false);

    /* Set AP */
    if (WiFi.softAP(HWManager::GetHWUID(), HWManager::GetMacAddress())) {
        LOG_INFO("Started the WiFi Module in AP mode\n");
        LOG_INFO("    SSID: %s\n", HWManager::GetHWUID());
        LOG_INFO("    Password: %s\n", HWManager::GetMacAddress());

        /* Wait to get IP */
        ip = WiFi.softAPIP().toString();
        memset(wifiIp, 0, IP_ADDR_SIZE_BYTES + 1);
        memcpy(wifiIp, ip.c_str(), ip.length());

        LOG_INFO("    IP Address: %s\n", wifiIp);

        /* Start the maintenance web server */
        StartMaintenanceServer();
    }
    else {
        LOG_ERROR("Failed to create the Access Point.\n");
    }
}

void ModeManager::StartMaintenanceServer(void) noexcept {
    MaintenanceWebServerHandlers* pHandlers;

    this->_pMaintServer = new WebServer(MAINTENANCE_WEB_SERVER_PORT);
    if (nullptr != this->_pMaintServer) {

        /* Create the handler */
        pHandlers = new MaintenanceWebServerHandlers(this->_pMaintServer);
        if (nullptr != pHandlers) {
            /* Start the server */
            this->_pMaintServer->begin();

            LOG_INFO(
                "Started maintenance interface on port %d.\n",
                MAINTENANCE_WEB_SERVER_PORT
            );
        }
        else {
            LOG_ERROR("Failed to create the maintenance server handlers.\n");
        }
    }
    else {
        LOG_ERROR("Failed to instanciate the maintenance web server.\n");
    }
}

void ModeManager::PeriodicUpdate(void) noexcept {
    if (E_Mode::MODE_MAINTENANCE == this->_currentMode &&
        nullptr != this->_pMaintServer) {
        /* Handle maintenance web server */
        this->_pMaintServer->handleClient();
    }
    else if (E_Mode::MODE_FAULTED == this->_currentMode) {
        LOG_ERROR("Faulted instance. Please re-flash the firmware.\n");
        vTaskDelay(1000);
    }
    else {
        /* Nothing to do, just wait */
        vTaskDelay(1000);
    }
}

void ModeManager::GetLastReset(void) noexcept {
    esp_reset_reason_t cpuReset;
    /* Get the CPUs resets */
    cpuReset = esp_reset_reason();
    LOG_INFO("Reset reason: %d.\n", cpuReset);

    switch (cpuReset) {
        case ESP_RST_UNKNOWN:
        case ESP_RST_POWERON:
        case ESP_RST_EXT:
        case ESP_RST_SW:
        case ESP_RST_DEEPSLEEP:
        case ESP_RST_SDIO:
            this->_forceMaintenance = false;
            break;

        case ESP_RST_PANIC:
        case ESP_RST_INT_WDT:
        case ESP_RST_TASK_WDT:
        case ESP_RST_WDT:
        case ESP_RST_BROWNOUT:
        default:
            this->_forceMaintenance = true;
    }
}