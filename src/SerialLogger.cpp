/**
 * @file    SerialLogger.cpp
 * 
 * @brief   This class can be used to create a logger with different logging levels to make debug and
 *          log messages easier to read and differentiate. Uses Serial by default.
 * 
 * @author: Nils Christians
 * @date    11.11.2020
 */

#include <Arduino.h>
#include "SerialLogger.h"

#define DEBUG

/**
 * @brief   Defines the default log levels. These can be changed 
 *          via a call with a link to a new structure like this in @ref setLogLevels
 **/
const struct LOG_LEVEL defaultLevels[DEFAULT_LOG_LEVELS] = {
    {0 , "ERROR"}, 
    {1, "INFO"}, 
    {2, "DEBUG"}
};

char buf[255];

SerialLogger::SerialLogger(Stream * s)
    : serial(s), loggingEnabled(false), printLogPrefix(true), printLevelPrefix(false), logLevelCnt(DEFAULT_LOG_LEVELS), logBufMsgCnt(0), loggerPrefixStr("SYSMON")
{
    logLevels = (LOG_LEVEL *)defaultLevels;  
    curLogLevel = logLevels;
}

void SerialLogger::startLogging()
{
    this->loggingEnabled = true;
}

void SerialLogger::stopLogging()
{
    this->loggingEnabled = false;
}

void SerialLogger::setPrintLogPrefix(bool b)
{
    this->printLogPrefix = b;
}
void SerialLogger::setPrintLevelDesc(bool b)
{
    this->printLevelPrefix = b;
}

void SerialLogger::setLogLevels(const LOG_LEVEL * newLogLevels, uint8_t lCount)
{

    if(newLogLevels != NULL)
    {
        uint8_t oldLogLevelVal = curLogLevel->levelValue;

        this->logLevels = &newLogLevels[0];

        this->logLevelCnt = lCount;

        this->setCurrentLogLevelValue(oldLogLevelVal);
    }
}

void SerialLogger::setCurrentLogLevelValue(uint8_t lVal)
{
    const LOG_LEVEL * lPtr = this->logLevels;
    this->curLogLevel = NULL;
    while(lPtr != NULL)
    {
        if(lPtr->levelValue == lVal )
        {
            this->curLogLevel = lPtr;
            break;
        }
        lPtr ++;
    }

    if(this->curLogLevel == NULL)
        this->curLogLevel = &this->logLevels[0];
}

Stream * SerialLogger::getStream()
{
    return this->serial;
}

LOG_LEVEL * SerialLogger::getLogLevels()
{
    return (LOG_LEVEL *)logLevels;
}
const LOG_LEVEL * SerialLogger::getCurrentLogLevel()
{
    return curLogLevel;
}

uint8_t SerialLogger::getLogLevelCnt()
{
    return this->logLevelCnt;
}

bool SerialLogger::logPrintf(uint8_t level, const char * format, ...)
{
    const LOG_LEVEL * l = NULL;

    if(level <= curLogLevel->levelValue)
    {   
        for (uint8_t i = 0; i <= curLogLevel->levelValue; i++)
        {
            if(level == this->logLevels[i].levelValue)
            {
                l = &logLevels[i];
                break;
            }
        }
    }

    if(l != NULL)
    {
        //print prefixes
        int ind = 0;

        if(this->printLogPrefix)
            ind += sprintf(&buf[ind], "%s: ", this->loggerPrefixStr);

        if(this->printLevelPrefix)
            ind += sprintf(&buf[ind], "%s: ", l->desc);
        
        va_list ap;
        va_start(ap, format);
        vsnprintf(&buf[ind], sizeof(buf), format, ap);
        va_end(ap);

        if(this->loggingEnabled)
            serial->println(buf);
        else
        {
            this->logMsgBuf[this->logBufMsgCnt].levelValue = level;

            strncpy(this->logMsgBuf[this->logBufMsgCnt].msg, buf, sizeof(this->logMsgBuf[this->logBufMsgCnt].msg));
            strcat(this->logMsgBuf[this->logBufMsgCnt].msg, "\r\n");
            
            this->logBufMsgCnt ++;
        }
        
        return true;
    }

    return false;
}

bool SerialLogger::logPrintf(uint8_t level, const __FlashStringHelper *ifsh)
{
  return this->logPrintf(level, reinterpret_cast<const char *>(ifsh));
}

void SerialLogger::printLogBuf()
{
    if(this->loggingEnabled)
    {
        #ifdef DEBUG
            serial->printf("Printing %d buffered messages!\n", this->logBufMsgCnt);
        #endif

        for(uint8_t i = 0; i < this->logBufMsgCnt; i++)
        {
            #ifdef DEBUG
                serial->printf("Printing element %d: ", i);
            #endif
            serial->print(this->logMsgBuf[i].msg);
            serial->flush();
            this->logMsgBuf[i].msg[0] = '\0';   //clear the buffered msg
            this->logMsgBuf[i].levelValue = 0xFF;
        }

        this->logBufMsgCnt = 0;
        #ifdef DEBUG
            serial->printf("%d buffered messages!\n", this->logBufMsgCnt);
        #endif
    }
}

void SerialLogger::getLogBuf(LOG_MSG * buf)
{
    buf = (this->logMsgBuf);
}

void SerialLogger::printLogLevels()
{
    if(this->loggingEnabled)
    {
        serial->printf("LOG: Printing %d log levels:\n", logLevelCnt);
        for(uint8_t i = 0; i < logLevelCnt; i++)
        {
            serial->println(F("{"));
            serial->printf(" Name: \"%s\"\n Value: %d\n", logLevels[i].desc, logLevels[i].levelValue);
            serial->println(F("}"));
        }
        serial->printf("Current log level: %s=%d\n\n", this->curLogLevel->desc, this->curLogLevel->levelValue);
    }
}
