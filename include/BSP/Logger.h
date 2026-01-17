/*******************************************************************************
 * @file Logger.h
 *
 * @see Logger.cpp
 *
 * @author Alexy Torres Aurora Dugo
 *
 * @date 12/12/2025
 *
 * @version 1.0
 *
 * @brief This file defines the logging module.
 *
 * @details This file defines the logging module. This comprises a set of
 * functions used to log at different verbose levels.
 *
 * @copyright Alexy Torres Aurora Dugo
 ******************************************************************************/

#ifndef __LOGGER_H_
#define __LOGGER_H_

/*******************************************************************************
 * INCLUDES
 ******************************************************************************/
#include <cstdint>   /* Standard Int Types */
#include <stddef.h>  /* Standard definitions */
#include <Storage.h> /* File */

/*******************************************************************************
 * CONSTANTS
 ******************************************************************************/

/** @brief Defines the current logger level. */
#define LOG_LEVEL LOG_LEVEL_DEBUG

/*******************************************************************************
 * MACROS
 ******************************************************************************/

/**
 * @brief Logs an information to the log buffer.
 *
 * @details Logs an information to the log buffer. The INFO tag will be added to
 * the log.
 *
 * @param[in] FMT The format string used for the log.
 * @param[in] ... The format arguments.
 */
#define LOG_INFO(FMT, ...) {                                \
        Logger::GetInstance()->LogLevel(                    \
        LOG_LEVEL_INFO,                                     \
        __FILE__,                                           \
        __LINE__,                                           \
        FMT,                                                \
        ##__VA_ARGS__                                       \
    );                                                      \
}

/**
 * @brief Logs an error to the log buffer.
 *
 * @details Logs an error to the log buffer. The ERROR tag will be added to
 * the log.
 *
 * @param[in] FMT The format string used for the log.
 * @param[in] ... The format arguments.
 */
#define LOG_ERROR(FMT, ...) {                               \
    Logger::GetInstance()->LogLevel(                        \
        LOG_LEVEL_ERROR,                                    \
        __FILE__,                                           \
        __LINE__,                                           \
        FMT,                                                \
        ##__VA_ARGS__                                       \
    );                                                      \
}

/**
 * @brief Logs a critical error to the log buffer.
 *
 * @details Logs a critical error to the log buffer. The CRIT tag will be added
 * to the log and the firmware will go into panic.
 *
 * @param[in] FMT The format string used for the log.
 * @param[in] ... The format arguments.
 */
#define PANIC(FMT, ...) {                                   \
    Logger::GetInstance()->LogLevel(                        \
        LOG_LEVEL_CRITICAL,                                 \
        __FILE__,                                           \
        __LINE__,                                           \
        FMT,                                                \
        ##__VA_ARGS__                                       \
    );                                                      \
}

/**
 * @brief Flushes the logs.
 *
 * @details Flushes the logs. This ensures the buffers are correctly written.
 */
#define LOG_FLUSH() {                       \
    Logger::GetInstance()->Flush();         \
}

#if LOGGER_DEBUG_ENABLED

/**
 * @brief Logs a debug to the log buffer.
 *
 * @details Logs a debug to the log buffer. The DEBUG tag will be added to
 * the log.
 *
 * @param[in] FMT The format string used for the log.
 * @param[in] ... The format arguments.
 */
#define LOG_DEBUG(FMT, ...) {                               \
    Logger::GetInstance()->LogLevel(                        \
        LOG_LEVEL_DEBUG,                                    \
        __FILE__,                                           \
        __LINE__,                                           \
        FMT,                                                \
        ##__VA_ARGS__                                       \
    );                                                      \
}

#else

#define LOG_DEBUG(FMT, ...)

#endif

/*******************************************************************************
 * STRUCTURES AND TYPES
 ******************************************************************************/

/** @brief Defines the log level. */
typedef enum
{
    /** @brief Logging level: log only critical error */
    LOG_LEVEL_CRITICAL = 0,
    /** @brief Logging level: log only errors */
    LOG_LEVEL_ERROR = 1,
    /** @brief Logging level: log previous levels and information */
    LOG_LEVEL_INFO = 2,
    /** @brief Logging level: log previous levels and debug output */
    LOG_LEVEL_DEBUG = 3
} E_LogLevel;

/** @brief RAM journal definition. */
typedef struct {
    /** @brief Start address of the journal in memory. */
    uint8_t* pStartAddress;
    /** @brief End address of the journal in memory. */
    uint8_t* pEndAddress;
    /** @brief Current cursor in the memory. */
    uint8_t* pCursor;
    /** @brief Tells if the journal buffer as already circled. */
    bool hasCircled;
} S_RamJournal;

/** @brief RAM journal descriptor. */
typedef struct {
    /** @brief Current offset of the cursor. */
    uint32_t pCursor;
} S_RamJournalDescriptor;

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
 * @brief The logger class.
 *
 * @details The logger class that provides the necessary functions to log
 * different log level messages to the serial port.
 */
class Logger
{
    /********************* PUBLIC METHODS AND ATTRIBUTES **********************/
    public:
        /**
         * @brief Returns the Logger instance singleton.
         *
         * @details Returns the Logger instance singleton.
         *
         * @return Returns the Logger instance singleton.
         */
        static Logger* GetInstance(void) noexcept;

        /**
         * @brief Logs a message to the logger.
         *
         * @details Logs a message to the logger. If the log level is above the
         * logger current level, the message is discarded.
         *
         * @param[in] kLevel The lovel of the message to log.
         * @param[in] pkFile The file where the log was generated.
         * @param[in] kLine The line where the log was generated.
         * @param[in] pkStr The format string used for the log
         * @param[in] ... The format arguments.
         */
        void LogLevel(const E_LogLevel kLevel,
                      const char*      pkFile,
                      const uint32_t   kLine,
                      const char*      kStr,
                      ...) noexcept;

        /**
         * @brief Flushes the logs.
         *
         * @details Flushes the logs. This ensures the buffers are correctly
         * written.
         */
        void Flush(void) noexcept;

        /**
         * @brief Opens the logger persistent journal.
         *
         * @details Opens the logger persistent journal. This provides a file
         * that can be read to get the current logs.
         */
        FsFile OpenPersistenJournal(void) const noexcept;

        /**
         * @brief Clears the persistent journal.
         *
         * @details Clears the persistent journal. This effectively removes the
         * log file.
         */
        void ClearPersistentJournal(void) const noexcept;

        /**
         * @brief Opens the logger RAM journal.
         *
         * @details Opens the logger RAM journal. This provides a descriptor
         * that can be read to get the current logs.
         *
         * @param[out] pDesc The RAM journal descriptor used to open.
         */
        void OpenRamJournal(S_RamJournalDescriptor* pDesc) const noexcept;

        /**
         * @brief Read from the RAM journal.
         *
         * @details Read from the RAM journal. This function will return up to
         * length bytes and update the descriptor. Ram journal is read from
         * the last log to the first log upward.
         *
         * @param[out] pBuffer The buffer used to receive the journal data.
         * @param[in] length The maximum number of bytes to fill in the buffer.
         * @param[out] pDesc The RAM journal descriptor used to open.
         */
        size_t ReadRamJournal(uint8_t*                pBuffer,
                              size_t                  length,
                              S_RamJournalDescriptor* pDesc) const noexcept;

        /**
         * @brief Sets the RAM journal descriptor cursor.
         *
         * @details Sets the RAM journal descriptor cursor. If the provided
         * offset is out of bound, the cursor is set to the end of the RAM
         * journal. Ram journal is seek from the last log to the first log
         * upward.
         *
         * @param[out] pDesc The RAM journal descriptor used to seek.
         * @param[in] kOffset The offset to set to the descriptor.
         */
        void SeekRamJournal(S_RamJournalDescriptor* pDesc,
                            const size_t            kOffset) const noexcept;

        /**
         * @brief Clears the RAM journal.
         *
         * @details Clears the RAM journal. This effectively resets the RAM
         * logs.
         */
        void ClearRamJournal(void) noexcept;
    /******************* PROTECTED METHODS AND ATTRIBUTES *********************/
    protected:
        /* None */

    /********************* PRIVATE METHODS AND ATTRIBUTES *********************/
    private:
        /**
         * @brief Construct the logger instance.
         *
         * @details Construct the logger instance. Initializes the serial output
         * and allocates the required resources for the logger.
         */
        Logger(void) noexcept;

        /**
         * @brief Writes the log to the log file in persisten storage.
         *
         * @details brief Writes the log to the log file in persisten storage.
         * On error, this function failes silently as the log woud be recursive.
         *
         * @param[in] kpStr The string log to write to the file.
         * @param[in] kLen The length of the string to write.
         */
        void WritePersistentJournal(const char* kpStr, size_t len)
        noexcept;

        /**
         * @brief Writes the log to the log RAM journal in persistent storage.
         *
         * @details Writes the log to the log RAM journal in persistent storage.
         * This function will always succeed.
         *
         * @param[in] kpStr The string log to write to the RAM journal.
         * @param[in] kLen The length of the string to write.
         */
        void WriteRamJournal(const char* kpStr, size_t len) noexcept;

        /** @brief The logger buffer. */
        char* _logBuffer;
        /** @brief The logger journal in RAM. */
        S_RamJournal _logJournalRam;

        /** @brief Stores the log file. */
        FsFile _logfile;

        /** @brief Stores the singleton instance. */
        static Logger* _SPINSTANCE;

};

#endif /* #ifndef __LOGGER_H_ */