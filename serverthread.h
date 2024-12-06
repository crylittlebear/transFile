#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include <QThread>
#include <QTcpSocket>

struct FileMetaData {
    char fileName[256];
    quint64 totalSize;
    quint64 dataSize;
    quint64 offset;
    bool isLastPiece;
};

class ServerThread : public QThread
{
    Q_OBJECT
public:
    explicit ServerThread(QTcpSocket* socket, QObject *parent = nullptr);

    void setSocket(QTcpSocket* socket);

    virtual void run() override;

signals:
    void recvPiece(const FileMetaData &metaData);

    void recvLast();

private:
    QTcpSocket* socket_;

    FileMetaData metaData_;

    bool isHeader_ = true;
};

#endif // SERVERTHREAD_H
