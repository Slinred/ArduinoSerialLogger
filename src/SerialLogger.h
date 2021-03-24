/**
 * @file    SerialLogger.h
 * 
 * @brief   This class can be used to create a logger with different logging levels to make debug and
 *          log messages easier to read and differentiate. Uses Serial by default.
 * 
 * @author: Nils Christians
 * @date    11.11.2020
 */

#ifndef _SERIALLOGGER_H_
#define _SERIALLOGGER_H_

#define DEFAULT_LOG_LEVELS          3   /**< the default number of log levels */
#define DEFAULT_LOG_LEVEL           0   /**< the default log level value */
#define MAX_LOG_LEVEL_CNT           10  /**< the maximal possible number of log levels */
#define MAX_LOG_LEVEL_DESC_CHARS    10  /**< the maximal number of chars for the log level descriptor */
#define LOG_MSG_BUFFER_CHARS        100
#define LOG_MSG_BUFFER_SIZE         50


/**
 * @brief   This structure defines a log level with value and a descriptor
 **/
struct LOG_LEVEL
{
    uint8_t levelValue; // value for this level
    char desc[MAX_LOG_LEVEL_DESC_CHARS];      // string descriptor for this log level
};

/**
 *  @brief  This structure defines a log message for the buffer
 **/
struct LOG_MSG
{
    uint8_t levelValue;
    char msg[LOG_MSG_BUFFER_CHARS];
};
/**
 * @brief   The logger class which modifies a given serial interfaces and build a logger-like construct around it,
 *          to seperate messages in different topics and priorities   
 **/
class SerialLogger
{
    private:
        Stream * serial;
        //Uart * serial;
        bool loggingEnabled;
        bool printLogPrefix;
        bool printLevelPrefix;
        uint8_t logLevelCnt;
        const LOG_LEVEL * curLogLevel;
        const LOG_LEVEL * logLevels;
    public:
        SerialLogger(Stream *);
        LOG_MSG logMsgBuf[LOG_MSG_BUFFER_SIZE];
        uint8_t logBufMsgCnt;
        char loggerPrefixStr[10];
        void startLogging();
        void stopLogging();
        void setPrintLogPrefix(bool);
        void setPrintLevelDesc(bool);
        void setLogLevels(const LOG_LEVEL *, uint8_t );
        void setCurrentLogLevelValue(uint8_t );
        Stream * getStream();
        LOG_LEVEL * getLogLevels();
        uint8_t getLogLevelCnt();
        const LOG_LEVEL * getCurrentLogLevel();
        bool logPrintf(uint8_t level, const char * format, ...);
        bool logPrintf(uint8_t level, const __FlashStringHelper *ifsh);
        void printLogBuf();
        void printLogLevels();
        void getLogBuf(LOG_MSG *);
};

#endif