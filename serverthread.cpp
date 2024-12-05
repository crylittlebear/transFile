#include "serverthread.h"
#include <QDir>
#include <QDebug>

ServerThread::ServerThread(QTcpSocket* socket, QObject *parent)
    : QThread{parent},
    socket_(socket)
{

}

void ServerThread::run()
{
    connect(socket_, &QTcpSocket::readyRead, this, [=]() {
        QDir dir;
        if (!dir.exists("RecvFiles")) {
            dir.mkdir("RecvFiles");
        }
        while (socket_->bytesAvailable() > sizeof(FileMetaData)) {
            if (isHeader_) {
                socket_->read((char*)&metaData_, sizeof(FileMetaData));
                isHeader_ = false;
            }
            if (socket_->bytesAvailable() < metaData_.dataSize) {
                break;
            }
            if (!isHeader_) {
                QString filePath = "RecvFiles/" + QString(metaData_.fileName);
                QFile file(filePath);
                if (!file.open(QIODevice::WriteOnly | QIODevice::Append)) {
                    qDebug() << "open file failed";
                }
                QByteArray data = socket_->read(metaData_.dataSize);
                file.write(data);
                file.close();
                isHeader_ = true;
                emit recvPiece(metaData_);
                if (metaData_.isLastPiece) {
                    emit recvLast();
                }
            }
        }
    });
}
