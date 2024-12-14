#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

enum LogLevel {
    LEVEL_INFO = 1000,   // 普通信息
    LEVEL_ERROR,  // 错误信息
    LEVEL_FATAL,  // core信息
    LEVEL_DEBUG   // 调试信息
};

#define LOG_INFO_BASE(logmsgformat, file, line, ...) \
do \
    {  \
        Logger& logger = Logger::instance(); \
        logger.setLogLevel(LEVEL_INFO);   \
        char buf[1024] = {0};    \
        snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
        logger.log(buf, file, line); \
    } while (0)

#define LOG_ERROR_BASE(logmsgformat, file, line, ...) \
do \
    {  \
            Logger& logger = Logger::instance(); \
            logger.setLogLevel(LEVEL_ERROR);   \
            char buf[1024] = {0};    \
            snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
            logger.log(buf, file, line); \
    } while (0)

#define LOG_FATAL_BASE(logmsgformat, file, line, ...) \
do \
    {  \
            Logger& logger = Logger::instance(); \
            logger.setLogLevel(LEVEL_FATAL);   \
            char buf[1024] = {0};    \
            snprintf(buf, 1024, logmsgformat, ##__VA_ARGS__); \
            logger.log(buf, file, line); \
    } while (0)

#ifndef MUDEBUG
#define LOG_DEBUG_BASE(logmsgFormat, file, line, ...) \
do \
    { \
            Logger &logger = Logger::instance(); \
            logger.setLogLevel(LEVEL_DEBUG); \
            char buf[1024] = {0}; \
            snprintf(buf, 1024, logmsgFormat, ##__VA_ARGS__); \
            logger.log(buf, file, line); \
    } while(0)
#else
#define LOG_DEBUG(logmsgFormat, ...)
#endif

#define LOG_INFO(str) LOG_INFO_BASE(QString(str).toUtf8().data(), __FILE__, __LINE__)
#define LOG_ERROR(str) LOG_ERROR_BASE(QString(str).toUtf8().data(), __FILE__, __LINE__)
#define LOG_FATAL(str) LOG_FATAL_BASE(QString(str).toUtf8().data(), __FILE__, __LINE__)
#define LOG_DEBUG(str) LOG_DEBUG_BASE(QString(str).toUtf8().data(), __FILE__, __LINE__)

class Logger
{
public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static Logger& instance();

    void setLogLevel(LogLevel level);

    void log(QString str, const char* file, int line);

private:
    Logger() = default;

    LogLevel logLevel_;
};

#endif // LOGGER_H
