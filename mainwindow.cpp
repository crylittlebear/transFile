#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>

#include "downloaditemwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : DragWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init()
{
    setWindowFlag(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowIcon(QIcon(":/img/cloud.png"));
    setWindowTitle("服务器");
    ui->listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    server_ = new QTcpServer(this);

    quint16 port = 8086;
    server_->listen(QHostAddress::Any, port);

    connect(server_, &QTcpServer::newConnection, this, [=](){
        QTcpSocket* socket = server_->nextPendingConnection();
        if (!socket->isValid()) {
            qDebug() << "invalid socket!";
            return;
        }
        ServerThread* thread = new ServerThread(socket, this);
        connect(thread, &ServerThread::recvPiece, this, &MainWindow::sltHandleRecvPiece, Qt::QueuedConnection);
        thread->start();
    });
}

void MainWindow::sltOpenRecvDir(QString fileName)
{
    QString path = QDir::currentPath();
    path += "/RecvFiles/";
    QUrl url(path);
    QDesktopServices::openUrl(url);
}

void MainWindow::sltHandleRecvPiece(const FileMetaData &metaData)
{
    // 如果接收到第一块数据，创建一个ListWidgetItem
    QListWidgetItem* item;
    DownloadItemWidget* widget;
    if (metaData.offset == 0) {
        item = new QListWidgetItem;
        ui->listWidget->insertItem(0, item);
        item->setSizeHint(QSize(ui->listWidget->width(), 40));
        widget = new DownloadItemWidget;
        widget->setLabel("://img/picture.png");
        widget->setFileText(QString(metaData.fileName));
        widget->setProgressBarMax(metaData.totalSize);
        widget->setProgressBarValue(metaData.offset + metaData.dataSize);
        ui->listWidget->setItemWidget(item, widget);
        // 切记，信号槽只能连接一次，不然一个如果多次连接，可能会出现一次信号触发多个槽函数的情况
        connect(widget, &DownloadItemWidget::sigBtnOpenDir, this, &MainWindow::sltOpenRecvDir);
    } else {
        item = ui->listWidget->item(0);
        widget = static_cast<DownloadItemWidget*>(ui->listWidget->itemWidget(item));
        widget->setProgressBarValue(metaData.offset + metaData.dataSize);
    }
    if (metaData.offset + metaData.dataSize == metaData.totalSize) {
        widget->setToolButtonEnable();
    }

    /**
     *  像下面这样就不可以，这样会导致每收到一个一段来自客户端的数据就连接一次，那么当打开文件夹的时候就会出现
     *  一次性弹出好多个文件夹窗口
    */
    // connect(widget, &DownloadItemWidget::sigBtnOpenDir, this, &MainWindow::sltOpenRecvDir);
}

void MainWindow::on_tBtnClose_clicked()
{
    this->close();
}


