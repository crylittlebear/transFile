#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QDesktopServices>
#include <QAudioOutput>

#include "downloaditemwidget.h"
#include "dragslider.h"

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
    ui->listWidgetMusicList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->listWidgetMusicList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
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

    connect(ui->tBtnMusic, &QToolButton::clicked, this, &MainWindow::sltBtnMusicClicked);
    connect(ui->tBtnVideo, &QToolButton::clicked, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_video);
    });
    connect(ui->tBtnUpload, &QToolButton::clicked, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_upload);
    });
    connect(ui->tBtnOpen, &QToolButton::clicked, this, &MainWindow::sltOpenRecvDir);

    ui->stackedWidget->setCurrentWidget(ui->page_upload);

    player_ = new QMediaPlayer(this);
    QAudioOutput* audioOutput = new QAudioOutput(this);
    player_->setAudioOutput(audioOutput);

    // connect(player_, &QMediaPlayer::playbackStateChanged, [=] (QMediaPlayer::PlaybackState state) {
    //     if (state == QMediaPlayer::StoppedState) {
    //         // 如果一首歌播放完毕，切换到下一首歌
    //         emit ui->tBtnNextMusic->clicked();
    //     }
    // });

    connect(player_, &QMediaPlayer::mediaStatusChanged, [=](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            emit ui->tBtnNextMusic->clicked();
        }
    });

    static int index = 0;
    timer_ = new QTimer(this);
    timer_->setInterval(200);
    connect(timer_, &QTimer::timeout, [&](){
        ++index;
        ui->dragSlider->setCurValue(index);
        if (index == 100) {
            timer_->stop();
        }
    });
    timer_->start();
}

void MainWindow::playMusic(QListWidgetItem *item)
{
    MusicListWidget* widget = static_cast<MusicListWidget*>(ui->listWidgetMusicList->itemWidget(item));
    QString fileName = widget->fileName();
    player_->setSource(QUrl(getRecvDir() + fileName));
    ui->tBtnPlayPause->setIcon(QIcon("://img/pause.png"));
    ui->tBtnPlayPause->setText("暂停");
    player_->play();
}

QString MainWindow::getRecvDir() const
{
    return QCoreApplication::applicationDirPath() + "/RecvFiles/";
}

void MainWindow::sltBtnMusicClicked()
{
    ui->stackedWidget->setCurrentWidget(ui->page_music);
    // 打开Recv文件夹下的音频文件
    QDir dir(getRecvDir());
    QStringList entryList = dir.entryList(QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot);
    for (int i = 0; i < entryList.size(); ++i) {
        if (!musicList_.contains(entryList.at(i))) {
            QString filePath = entryList.at(i);
            QString suffix = filePath.right(filePath.size() - filePath.lastIndexOf('.') - 1);
            qDebug() << suffix;
            if (suffix == "flac" || suffix == "mp3" || suffix == "wav") {
                QListWidgetItem* item = new QListWidgetItem;
                ui->listWidgetMusicList->insertItem(0, item);
                item->setSizeHint(QSize(ui->listWidgetMusicList->width(), 40));
                MusicListWidget* widget = new MusicListWidget;
                widget->setIcon("://img/music.png");
                widget->setName(filePath);
                ui->listWidgetMusicList->setItemWidget(item, widget);
                musicList_.append(filePath);
            }
        }
    }
}

void MainWindow::sltOpenRecvDir(bool flag)
{
    Q_UNUSED(flag);
    QDesktopServices::openUrl(QUrl(getRecvDir()));
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
        widget->setProgressBarColor(QColor(0, 62, 91));
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

// 双击ListWIdget中的项，如果是视频直接播放
void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    // playMusic(item);
}

void MainWindow::on_tBtnMin_clicked()
{
    this->showMinimized();
}

void MainWindow::on_tBtnMax_clicked()
{
    if (this->isMaximized()) {
        this->showNormal();
    } else {
        this->showMaximized();
    }
}

void MainWindow::on_listWidgetMusicList_itemDoubleClicked(QListWidgetItem *item)
{
    playMusic(item);
}

void MainWindow::on_tBtnPreMusic_clicked()
{
    int row = ui->listWidgetMusicList->currentRow();
    qDebug() << "current row = " << row;
    if (row == -1 || row == 0) {
        return;
    }
    row = row - 1;
    ui->listWidgetMusicList->setCurrentRow(row);
    playMusic(ui->listWidgetMusicList->currentItem());
}

void MainWindow::on_tBtnPlayPause_clicked()
{
    if (player_->playbackState() == QMediaPlayer::PlayingState) {
        ui->tBtnPlayPause->setIcon(QIcon("://img/play.png"));
        ui->tBtnPlayPause->setText("播放");
        player_->pause();
    } else {
        ui->tBtnPlayPause->setIcon(QIcon("://img/pause.png"));
        ui->tBtnPlayPause->setText("暂停");
        player_->play();
    }
}

void MainWindow::on_tBtnNextMusic_clicked()
{
    int row = ui->listWidgetMusicList->currentRow();
    // qDebug() << "current row = " << row;
    if (row == -1 || row == ui->listWidgetMusicList->count() - 1) {
        return;
    }
    row = row + 1;
    ui->listWidgetMusicList->setCurrentRow(row);
    playMusic(ui->listWidgetMusicList->currentItem());
}


void MainWindow::on_tBtnSearchMusic_clicked()
{
    QString searchName = ui->lineEditSearchMusic->text();
    ui->lineEditSearchMusic->clear();
    for (int i = 0; i < ui->listWidgetMusicList->count(); ++i) {
        QListWidgetItem* item = ui->listWidgetMusicList->item(i);
        MusicListWidget* widget = static_cast<MusicListWidget*>(ui->listWidgetMusicList->itemWidget(item));
        QString fileName = widget->fileName();
        if (fileName.contains(searchName)) {
            ui->listWidgetMusicList->setCurrentRow(i);
            return;
        }
    }
}

