#include "logger.h"
#include <QDateTime>
#include <QDebug>

Logger &Logger::instance()
{
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level)
{
    logLevel_ = level;
}

void Logger::log(QString str, const char* file, int line)
{
    QString logMsg;
    switch (logLevel_) {
    case LEVEL_INFO:
        logMsg += "[INFO]";
        break;
    case LEVEL_ERROR:
        logMsg += "[ERROR]";
        break;
    case LEVEL_FATAL:
        logMsg += "[FATAL]";
        break;
    case LEVEL_DEBUG:
        logMsg += "[DEBUG]";
        break;
    default:
        break;
    }
    QString filePath(file);
    QString fileName = filePath.right(filePath.size() - filePath.lastIndexOf('/') - 1);
    QString msg = QString("  [%1, line: %2] --%3").arg(fileName).arg(line).arg(str);
    logMsg += msg;
    qDebug() << logMsg.toUtf8().data();
}
