#include "networkdata.h"
#include "logger.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <qjsonarray.h>

NetworkData::NetworkData(QObject *parent)
    : QObject{parent}
{

}

QString NetworkData::getCode(QByteArray json)
{
    QString code;
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        LOG_ERROR("Json解析失败");
    } else {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue value = jsonObj.value("code");
        code = value.toString();
    }
    return code;
}

int NetworkData::getCount(QByteArray json)
{
    int cnt;
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        LOG_ERROR("Json解析失败");
    } else {
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue value = jsonObj.value("count");
        cnt = value.toInt();
    }
    return cnt;
}

QStringList NetworkData::getFileCount(QByteArray json)
{
    QStringList list;
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        LOG_ERROR("Json解析失败");
    } else {
        QJsonObject rootObj = jsonDoc.object();
        QJsonValue codeValue = rootObj.value("code");
        if (codeValue.type() == QJsonValue::String) {
            list << codeValue.toString();
        }
        QJsonValue numValue = rootObj.value("num");
        if (numValue.type() == QJsonValue::String) {
            list << numValue.toString();
        }
    }
    return list;
}

QList<FileInfo *> NetworkData::getFileInfo(QByteArray json)
{
    QList<FileInfo*> list;
    QJsonParseError err;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        LOG_ERROR("Json解析失败");
    } else {
        QJsonObject rootObj = jsonDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray fileArr =  filesValue.toArray();
            for (int i = 0; i < fileArr.size(); ++i) {
                QJsonValue fileValue = fileArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {
                    FileInfo* info = new FileInfo;
                    QJsonObject jsonObj = fileValue.toObject();
                    QJsonValue userValue = jsonObj.value("user");
                    QJsonValue md5Value = jsonObj.value("md5");
                    QJsonValue createTimeValue = jsonObj.value("create_time");
                    QJsonValue fileNameValue = jsonObj.value("file_name");
                    QJsonValue shareStatusValue = jsonObj.value("share_status");
                    QJsonValue pvValue = jsonObj.value("pv");
                    QJsonValue urlValue = jsonObj.value("url");
                    QJsonValue sizeValue = jsonObj.value("size");
                    QJsonValue typeValue = jsonObj.value("type");

                    info->user = userValue.toString();
                    info->md5 = md5Value.toString();
                    info->createTime = createTimeValue.toString();
                    info->fileName = fileNameValue.toString();
                    info->shareStatus = shareStatusValue.toInt();
                    info->pv = pvValue.toInt();
                    info->url = urlValue.toString();
                    info->size = sizeValue.toInt();
                    info->fileType = typeValue.toString();

                    list.append(info);
                }
            }
        }
    }
    return list;
}

QList<FileDownload *> NetworkData::getFileDownload(QByteArray json)
{
    QList<FileDownload*> list;
    QJsonParseError err;
    //解析返回数据
    QJsonDocument rootDoc = QJsonDocument::fromJson(json, &err);
    if (err.error != QJsonParseError::NoError) {
        qDebug() << "Json格式错误";
    } else {
        //解析json
        QJsonObject rootObj = rootDoc.object();
        QJsonValue filesValue = rootObj.value("files");
        if (filesValue.type() == QJsonValue::Array) {
            QJsonArray filesArr = filesValue.toArray();

            for (int i=0;i<filesArr.count();i++) {
                QJsonValue fileValue = filesArr.at(i);
                if (fileValue.type() == QJsonValue::Object) {
                    FileDownload *fileDownload = new FileDownload;

                    QJsonObject fileObj = fileValue.toObject();
                    QJsonValue fileNameValue = fileObj.value("filename");
                    QJsonValue pvValue = fileObj.value("pv");

                    fileDownload->fileName = fileNameValue.toString();
                    fileDownload->pv = pvValue.toInt();

                    list.append(fileDownload);
                }
            }
        }
    }
    return list;
}
