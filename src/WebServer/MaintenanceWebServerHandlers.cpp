/*******************************************************************************
 * @file MaintenanceWebServerHandlers.cpp
 *
 * @see MaintenanceWebServerHandlers.h
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/01/2026
 *
 * @version 1.0
 *
 * @brief Maintenance Web Server URL handlers.
 *
 * @details Maintenance Web Server URL handlers. This file defines all the
 * handlers used for the maintenance web server.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/

/* Included headers */
#include <BSP.h>           /* Hardware layer */
#include <Errors.h>        /* Errors definitions */
#include <Logger.h>        /* Logger services */
#include <Arduino.h>       /* Arduino Framework */
#include <version.h>       /* Versioning  */
#include <Settings.h>      /* Settings services */
#include <WebServer.h>     /* Web server services */
#include <SystemState.h>   /* Get the system state */
#include <ModeManager.h>   /* Mode management */

/* Header file */
#include <MaintenanceWebServerHandlers.h>


/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/
/** @brief Defines the index URL */
#define PAGE_URL_INDEX "/"
/** @brief Defines the reboot URL */
#define PAGE_URL_MONITOR "/reboot"
/** @brief Defines the RAM log loading request URL. */
#define RAM_LOGS_LOAD_URL "/loadram"
/** @brief Defines the journal log loading request URL. */
#define JOURNAL_LOGS_LOAD_URL "/loadjournal"
/** @brief Defines the clear log request URL. */
#define CLEAR_LOGS_URL "/clearlogs"

/** @brief Defines the amount of logs to lazy load. */
#define LOG_LAZY_LOAD_SIZE 512

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
/** @brief Stores the current web server instance. */
static MaintenanceWebServerHandlers* spInstance = nullptr;

/*******************************************************************************
 * FUNCTIONS
 ******************************************************************************/
/* None */

/*******************************************************************************
 * CLASS METHODS
 ******************************************************************************/
MaintenanceWebServerHandlers::MaintenanceWebServerHandlers(WebServer* pServer)
noexcept {
    this->_pServer = pServer;

    /* Configure the handlers */
    this->_pServer->onNotFound(HandleNotFound);
    this->_pServer->on(PAGE_URL_INDEX, HandleIndex);
    this->_pServer->on(PAGE_URL_MONITOR, HandleReboot);
    this->_pServer->on(RAM_LOGS_LOAD_URL, HandleRamLoad);
    this->_pServer->on(JOURNAL_LOGS_LOAD_URL, HandleJournalLoad);
    this->_pServer->on(CLEAR_LOGS_URL, HandleClearLogs);

    /* Set the instance */
    spInstance = this;

    LOG_DEBUG("Maintenance Web Server Page Handlers manager initialized.\n");
}

MaintenanceWebServerHandlers::~MaintenanceWebServerHandlers(void) {
    PANIC(
        "Tried to destroy the Maintenance Web Server page handlers manager.\n"
    );
}

void MaintenanceWebServerHandlers::HandleNotFound(void) noexcept {
    std::string header;
    std::string footer;
    std::string page;

    LOG_DEBUG(
        "Handling Web page not found: %s\n",
        spInstance->_pServer->uri().c_str()
    );

    /* Generate the page */
    spInstance->GetPageHeader(header, "Not Found");
    spInstance->GetPageFooter(footer);
    page = header + "<h1>Not Found</h1>" + footer;

    /* Send */
    spInstance->GenericHandler(page, 404);
}

void MaintenanceWebServerHandlers::HandleIndex(void) noexcept {
    std::string title;
    std::string page;
    std::string footer;
    std::string header;

    page =
        "<div>"
        "   <h1>Real-Time High-Reliability Weather Station</h1>"
        "   <h2>HWUID: ";
    page += HWManager::GetHWUID();
    page +=
        "  | " VERSION "</h2>"
        "</div>";

    page += "<div>";
    page += "<h3>==== Maintenance Mode ====</h3>";
    page += "<table>";
    page += "<tr>";
    page += "<td><a href=\"/reboot?mode=0\">Reboot in nominal</a></td>";
    page += "<td><a href=\"/reboot?mode=1\">Reboot in maintenance</a></td>";
    page += "</tr>";
    page += "</table>";
    page += "</div>";

    spInstance->GetFormatedLogs(page);

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);


    /* Send */
    spInstance->GenericHandler(header + page + footer, 200);
}

void MaintenanceWebServerHandlers::HandleReboot(void) noexcept {
    std::string title;
    std::string page;
    std::string footer;
    std::string header;
    String      arg;
    E_Return    error;

    /* Generate the page */
    spInstance->GetPageHeader(header, title);
    spInstance->GetPageFooter(footer);

    if (spInstance->_pServer->hasArg("mode")) {
        arg = spInstance->_pServer->arg("mode");

        /* Get the reboot mode */
        if (arg.equals("0")) {
            LOG_DEBUG("Setting firmware to nominal mode.\n");

            /* Send */
            page = "<div><h1>Rebooting in nominal mode.</h1></div>";
            spInstance->GenericHandler(header + page + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_NOMINAL
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set nominal mode.\n");
            }
        }
        else if (arg.equals("1")) {
            LOG_DEBUG("Setting firmware to maintenance mode.\n");

            /* Send */
            page = "<div><h1>Rebooting in maintenance mode.</h1></div>";
            spInstance->GenericHandler(header + page + footer, 200);

            error = SystemState::GetInstance()->GetModeManager()->SetMode(
                E_Mode::MODE_MAINTENANCE
            );
            if (error != E_Return::NO_ERROR) {
                LOG_ERROR("Failed to set maintenance mode.\n");
            }
        }
        else {
            page = "<div><h1>Unknown reboot mode.</h1></div>";
        }
    }
    else {
        page = "<div><h1>Unknown reboot mode.</h1></div>";
    }

    /* Send */
    spInstance->GenericHandler(header + page + footer, 200);
}

void MaintenanceWebServerHandlers::HandleRamLoad(void) noexcept {
    Logger*                pLogger;
    S_RamJournalDescriptor logDesc;
    char                   pBuffer[LOG_LAZY_LOAD_SIZE + 1];
    size_t                 readBytes;
    String                 arg;

    pLogger = Logger::GetInstance();

    /* Get the current offset */
    if (spInstance->_pServer->hasArg("offset")) {
        arg = spInstance->_pServer->arg("offset");

        sscanf(arg.c_str(), "%zu", &readBytes);

        /* Open and offset */
        pLogger->OpenRamJournal(&logDesc);
        pLogger->SeekRamJournal(&logDesc, readBytes);

        /* Read journal */
        readBytes = pLogger->ReadRamJournal(
            (uint8_t*)pBuffer,
            LOG_LAZY_LOAD_SIZE,
            &logDesc
        );
        pBuffer[readBytes] = 0;
    }
    else {
        readBytes = 0;
    }

    /* Update page length and send */
    spInstance->_pServer->setContentLength(readBytes);
    spInstance->_pServer->send(200, "text/html", pBuffer);
}

void MaintenanceWebServerHandlers::HandleJournalLoad(void) noexcept {
    Logger* pLogger;
    char    pBuffer[LOG_LAZY_LOAD_SIZE + 1];
    size_t  readBytes;
    size_t  offset;
    size_t  fileSize;
    String  arg;
    FsFile  journal;

    pLogger = Logger::GetInstance();

    /* Get the current offset */
    if (spInstance->_pServer->hasArg("offset")) {
        arg = spInstance->_pServer->arg("offset");

        sscanf(arg.c_str(), "%zu", &readBytes);

        /* Open */
        journal = pLogger->OpenPersistenJournal();
        if (journal.isOpen()) {

            /* Read from end of file, get the size */
            fileSize = journal.size();
            if (fileSize > readBytes) {
                /* Get the current cursor */
                offset = fileSize - readBytes;

                /* Get the next cursor */
                if (LOG_LAZY_LOAD_SIZE < offset) {
                    offset -= LOG_LAZY_LOAD_SIZE;
                    readBytes = LOG_LAZY_LOAD_SIZE;
                }
                else {
                    readBytes = offset;
                    offset = 0;
                }
                /* Get the number of bytes to read */
                journal.seek(offset);
                readBytes = journal.read(pBuffer, readBytes);
                pBuffer[readBytes] = 0;
            }
            else {
                readBytes = 0;
            }
            journal.close();
        }
        else {
            readBytes = 0;
        }
    }
    else {
        readBytes = 0;
    }

    /* Update page length and send */
    spInstance->_pServer->setContentLength(readBytes);
    spInstance->_pServer->send(200, "text/html", pBuffer);
}

void MaintenanceWebServerHandlers::HandleClearLogs(void) noexcept {
    Logger* pLogger;
    int     param;
    String  arg;

    pLogger = Logger::GetInstance();

    /* Get the current offset */
    if (spInstance->_pServer->hasArg("logtype")) {
        arg = spInstance->_pServer->arg("logtype");

        sscanf(arg.c_str(), "%d", &param);

        if (0 == param) {
            pLogger->ClearRamJournal();
        }
        else if (1 == param) {
            pLogger->ClearPersistentJournal();
        }
    }

    /* Update page length and send */
    spInstance->_pServer->setContentLength(0);
    spInstance->_pServer->send(200, "text/html", "");
}

void MaintenanceWebServerHandlers::GetPageHeader(std::string&       rHeaderStr,
                                                 const std::string& krTitle)
const noexcept {
    rHeaderStr.clear();
    rHeaderStr += "<!DOCTYPE html>\n"
                  "<html lang='en'>\n"
                  "<head>\n";
    rHeaderStr += "<meta name='viewport' ";
    rHeaderStr += "content='width=device-width, initial-scale=1' "
                  "charset='UTF-8'/>\n"
                  "<title>\n";
    rHeaderStr += krTitle;
    rHeaderStr += "</title>\n";
    GeneratePageCSS(rHeaderStr);
    GeneratePageScripts(rHeaderStr);
    rHeaderStr += "</head>\n"
                  "<body>";
}

void MaintenanceWebServerHandlers::GetPageFooter(std::string& rFooterStr)
const noexcept {
    rFooterStr.clear();
    rFooterStr += "</div></body>\n</html>";
}

void MaintenanceWebServerHandlers::GeneratePageCSS(std::string& rHeaderStr)
const noexcept {
    rHeaderStr +=
        "<style>"
        "body {"
        "font-family: monospace;"
        "}"
        "table, th, td {"
        "border: 1px dashed gray;"
        "border-collapse: collapse;"
        "}"
        "td, th {"
        "padding: 5px;"
        "}"
        ".log_text {"
        "border: 1px dashed gray;"
        "padding: 5px;"
        "}"
        "</style>";
}

void MaintenanceWebServerHandlers::GeneratePageScripts(std::string& rHeaderStr)
const noexcept {
    rHeaderStr +=
        "<script>"

        /* Logs lazy loading generic */
        "function loadLogs(offset, update_item, item, url){"
        "var xhr = new XMLHttpRequest();"
        "xhr.onreadystatechange = function() {"
        "if (xhr.readyState === 4){"
        "update_item.innerHTML = xhr.responseText + update_item.innerHTML;"
        "item.setAttribute('loaded', parseInt(offset) + xhr.responseText.length);"
        "};"
        "};"
        "xhr.open('GET', url + '?offset=' + offset);"
        "xhr.send();"
        "}"

        /* Logs clear generic */
        "function clearLogs(logId){"
        "var xhr = new XMLHttpRequest();"
        "xhr.onreadystatechange = function() {"
        "if (xhr.readyState === 4){"
        "item = 0;"
        "if (logId == 0) {"
        "item=document.getElementById('load_more_ram');"
        "update_item=document.getElementById('ram_logs');"
        "}"
        "else if (logId == 1) {"
        "item=document.getElementById('load_more_journal');"
        "update_item=document.getElementById('journal_logs');"
        "}"
        "update_item.innerHTML = '';"
        "item.setAttribute('loaded', 0);"
        "};"
        "};"
        "xhr.open('GET', '" CLEAR_LOGS_URL"?logtype=' + logId);"
        "xhr.send();"
        "}"

        /* Document ready */
        "document.addEventListener('DOMContentLoaded', function() {"

        /* Ram lazy loading */
        "loadMoreRam = document.getElementById('load_more_ram');"
        "loadMoreRam.onclick = function(){"
        "loadLogs(loadMoreRam.getAttribute('loaded'),"
        "document.getElementById('ram_logs'), loadMoreRam, '"
        RAM_LOGS_LOAD_URL
        "');"
        "return false;};"

        /* Journal lazy loading */
        "loadMoreJour = document.getElementById('load_more_journal');"
        "loadMoreJour.onclick = function(){"
        "loadLogs(loadMoreJour.getAttribute('loaded'),"
        "document.getElementById('journal_logs'), loadMoreJour, '"
        JOURNAL_LOGS_LOAD_URL
        "');"
        "return false;};"

        /* Log Clear */
        "resetJour = document.getElementById('reset_file');"
        "resetJour.onclick = function(){clearLogs(1); return false;};"
        "resetRam = document.getElementById('reset_ram');"
        "resetRam.onclick = function(){clearLogs(0); return false;};"

        "});"

        "</script>";
}

void MaintenanceWebServerHandlers::GenericHandler(const std::string& krPage,
                                                  const int32_t      kCode)
noexcept {
    /* Update page length and send */
    this->_pServer->setContentLength(krPage.size());
    this->_pServer->send(kCode, "text/html", krPage.c_str());
}

void MaintenanceWebServerHandlers::GetFormatedLogs(std::string& rPage) const
noexcept {
    Logger*                pLogger;
    S_RamJournalDescriptor logDesc;
    char                   pBuffer[LOG_LAZY_LOAD_SIZE + 1];
    size_t                 readBytes;
    FsFile                 journal;
    size_t                 fileSize;

    pLogger = Logger::GetInstance();

    rPage += "<div><h2>==== Log Journals ====</h2></div>";
    rPage += "<table>";
    rPage += "<tr>";
    rPage += "<td><a id=\"reset_ram\" href=\"#\">Clear RAM Logs</a></td>";
    rPage += "<td><a id=\"reset_file\" href=\"#\">Clear Journal Logs</a></td>";
    rPage += "</tr>";
    rPage += "</table>";


    /* Get the RAM logs */
    pLogger->OpenRamJournal(&logDesc);
    rPage += "<div><h3>==== RAM Logs ====</h3></div>";
    rPage += "<div class=\"log_text\"><p>";


    readBytes = pLogger->ReadRamJournal(
        (uint8_t*)pBuffer,
        LOG_LAZY_LOAD_SIZE,
        &logDesc
    );
    pBuffer[readBytes] = 0;

    rPage += "<p>";
    rPage += "<a id=\"load_more_ram\" loaded=\"";
    rPage += std::to_string(readBytes);
    rPage += "\" href=\"#\">Load previous...<a><br />";
    rPage += "<pre id=\"ram_logs\">";
    rPage += pBuffer;
    rPage += "</pre></p>";
    rPage += "</div>";


    /* Get the journal logs */
    rPage += "<div><h3>==== Journal Logs ====</h3></div>";
    rPage += "<div class=\"log_text\"><p>";

    journal = pLogger->OpenPersistenJournal();
    if (journal.isOpen()) {
        /* Read from end of file, get the size */
        fileSize = journal.size();

        /* Get the next cursor */
        if (LOG_LAZY_LOAD_SIZE < fileSize) {
            journal.seek(fileSize - LOG_LAZY_LOAD_SIZE);
            readBytes = LOG_LAZY_LOAD_SIZE;
        }
        else {
            readBytes = fileSize;
        }
        /* Get the number of bytes to read */
        readBytes = journal.read(pBuffer, readBytes);
        pBuffer[readBytes] = 0;
        journal.close();
    }
    else {
        readBytes = 0;
    }
    pBuffer[readBytes] = 0;

    rPage += "<p>";
    rPage += "<a id=\"load_more_journal\" loaded=\"";
    rPage += std::to_string(readBytes);
    rPage += "\" href=\"#\">Load previous...<a><br />";
    rPage += "<pre id=\"journal_logs\">";
    rPage += pBuffer;

    rPage += "</pre></p>";
    rPage += "</div>";
}

