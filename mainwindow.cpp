#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>

#include "serverthread.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/cloud.png"));
    setWindowTitle("服务器");
    server_ = new QTcpServer(this);

    ui->lineEdit->setText("6666");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnListen_clicked()
{
    quint16 port = ui->lineEdit->text().toUInt();
    server_->listen(QHostAddress::Any, port);
    ui->plainTextEdit->appendPlainText(QString("start listen on port: %1").arg(port));

    connect(server_, &QTcpServer::newConnection, this, [=](){
        QTcpSocket* socket = server_->nextPendingConnection();
        if (!socket->isValid()) {
            qDebug() << "invalid socket!";
        }
        ServerThread* thread = new ServerThread(socket, this);
        connect(thread, &ServerThread::recvPiece, [=](FileMetaData metaData) {
            ui->progressBar->setMaximum(metaData.totalSize);
            ui->progressBar->setValue(metaData.offset + metaData.dataSize);
            ui->labelFileName->setText(QString(metaData.fileName));
            ui->plainTextEdit->appendPlainText(QString("recv piece at: %1").arg(metaData.offset));
        });
        connect(thread, &ServerThread::recvLast, [=]() {
            ui->plainTextEdit->appendPlainText(QString("recv last piece of file!!!!!!!"));
        });
        thread->start();
    });
}


void MainWindow::on_btnOpen_clicked()
{
    QString path = QDir::currentPath();
    qDebug() << "path = " << path;
    path += "/RecvFiles";
    QUrl url(path);
    QDesktopServices::openUrl(url);
}

