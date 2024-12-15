#ifndef NETWORKDATA_H
#define NETWORKDATA_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QStringList>
#include "fileinfo.h"

class NetworkData : public QObject
{
    Q_OBJECT
public:
    explicit NetworkData(QObject *parent = nullptr);

    static QString getCode(QByteArray json);

    static int getCount(QByteArray json);

    static QStringList getFileCount(QByteArray json);

    static QList<FileInfo*> getFileInfo(QByteArray json);

    static QList<FileDownload*> getFileDownload(QByteArray json);
};

#endif // NETWORKDATA_H
