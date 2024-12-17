#include "filewidget.h"
#include "ui_filewidget.h"
#include "logger.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>

FileWidget::FileWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileWidget)
{
    ui->setupUi(this);

    initListWidget();

    addMenu();

    networkManager_ = common_->getNetworkAccessManager();
    uploadTask_ = UploadTask::instance();
    downloadTask_ = DownloadTask::instance();

    checkTaskList();
}

FileWidget::~FileWidget()
{
    delete ui;
}

void FileWidget::refreshFiles()
{
    getMyFileCount(FileDisplayMode::Normal);
}

void FileWidget::initListWidget()
{
    common_ = Common::instance();
    loginInfo_ = LoginInfoInstance::instance();

    ui->listWidget->setViewMode(QListView::IconMode);
    ui->listWidget->setIconSize(QSize(80, 80));
    ui->listWidget->setGridSize(QSize(100, 120));

    ui->listWidget->setResizeMode(QListView::Adjust);
    ui->listWidget->setMovement(QListView::Static);

    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->listWidget, &QListWidget::customContextMenuRequested,
            this, &FileWidget::sltRightMenu);
}

void FileWidget::addMenu()
{
    menuItem_ = new QMenu(this);

    actionDownload_ = new QAction("下载", this);
    actionShare_ = new QAction("分享", this);
    actionDelete_ = new QAction("删除", this);
    actionProperty_ = new QAction("属性", this);

    menuItem_->addAction(actionDownload_);
    menuItem_->addAction(actionShare_);
    menuItem_->addAction(actionDelete_);
    menuItem_->addAction(actionProperty_);

    menuEmpty_ = new QMenu(this);
    actionDownloadAsc_ = new QAction("按下载量升序", this);
    actionDownloadDesc_ = new QAction("按下载量降序", this);
    actionRefresh_ = new QAction("刷新", this);
    actionUpload_ = new QAction("上传", this);

    menuEmpty_->addAction(actionDownloadAsc_);
    menuEmpty_->addAction(actionDownloadDesc_);
    menuEmpty_->addAction(actionRefresh_);
    menuEmpty_->addAction(actionUpload_);

    buildMenuConnections();
}

void FileWidget::addListWidgetItem(FileInfo *info)
{
    QString fileInfoName = QString("%1.png").arg(info->fileType);
    QString fileName = common_->getFileType(fileInfoName);
    QString filePath = QString("%1/%2").arg(FILE_TYPE_DIR).arg(fileName);
    LOG_INFO(QString("fileName: %1").arg(fileName));

    ui->listWidget->addItem(new QListWidgetItem(QIcon(filePath), info->fileName));
}

void FileWidget::buildMenuConnections()
{
    connect(actionDownload_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 下载触发");
        addDownloadFiles();
    });

    connect(actionShare_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 分享触发");
        dealFile("download");
    });

    connect(actionDelete_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 删除触发");
        dealFile("delete");
    });

    connect(actionProperty_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 属性触发");
        dealFile("property");
    });

    connect(actionDownloadAsc_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 按下载量升序");
        getMyFileCount(FileDisplayMode::Asc);
    });

    connect(actionDownloadDesc_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 按下载量降序");
        getMyFileCount(FileDisplayMode::Desc);
    });

    connect(actionRefresh_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 刷新");
        getMyFileCount(FileDisplayMode::Normal);
    });

    connect(actionUpload_, &QAction::triggered, this, [=]() {
        LOG_INFO("action 上传");
        addUploadFiles();
    });
}

void FileWidget::dealFile(QString cmd)
{
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (item == nullptr) {
        LOG_INFO("select nothing");
        return;
    }
    for (int i = 0; i < fileInfoList_.size(); ++i) {
        FileInfo* info = fileInfoList_.at(i);
        if (info->fileName == item->text()) {
            if (cmd == "share") {
                shareFile(info);
            } else if (cmd == "delete") {
                deleteFile(info);
            } else if (cmd == "property") {
                showProperty(info);
            }
            break;
        }
    }
}

void FileWidget::shareFile(FileInfo *info)
{
    QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");

    QString url = QString("http://%1:%2/dealfile?cmd=share").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramObj;
    paramObj.insert("user", loginInfo_->user());
    paramObj.insert("token", loginInfo_->token());
    paramObj.insert("md5", info->md5);
    paramObj.insert("filename", info->fileName);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply* reply = networkManager_->post(request, data);

    // 读取服务器返回的数据
    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        QByteArray data = reply->readAll();
        LOG_INFO(QString("服务器返回数据: %1").arg(QString(data)));
        QString code = NetworkData::getCode(data);

        if ("010" == code) {
            // 分享成功
            info->shareStatus = 1;
            QMessageBox::information(this, "分享成功", QString("【%1】分享成功").arg(info->fileName));
        } else if ("011" == code) {
            QMessageBox::warning(this, "分享失败", QString("【%1】分享失败").arg(info->fileName));
        } else if ("012" == code) {
            QMessageBox::warning(this, "分享失败", QString("【%1】文件已被分享").arg(info->fileName));
        } else if ("013" == code) {
            QMessageBox::critical(this, "账号异常", "请重新登录");
            emit sigLoginAgain();
            return;
        }
    });
}

void FileWidget::deleteFile(FileInfo *info)
{
    QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
    QString url = QString("http://%1/%2/dealfile?cmd=delete").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramObj;
    paramObj.insert("user", loginInfo_->user());
    paramObj.insert("token", loginInfo_->token());
    paramObj.insert("md5", info->md5);
    paramObj.insert("filename", info->fileName);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply* reply = networkManager_->post(request, data);

    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        QByteArray data = reply->readAll();
        LOG_INFO(QString("receive data: %1").arg(QString(data)));
        QString code = NetworkData::getCode(data);

        if ("013" == code) {
            QMessageBox::information(this, "删除成功", QString("【%1】文件删除成功").arg(info->fileName));
            for (int i = 0; i < fileInfoList_.size(); ++i) {
                if (fileInfoList_.at(i)->fileName == info->fileName) {
                    for (int j = 0; j < ui->listWidget->count(); ++j) {
                        QListWidgetItem* item = ui->listWidget->item(j);
                        if (item->text() == info->fileName) {
                            ui->listWidget->removeItemWidget(item);
                            delete item;
                            break;
                        }
                    }
                    fileInfoList_.removeAt(i);
                }
            }
        } else if ("014" == code) {
            QMessageBox::warning(this, "删除失败", QString("【%1】文件删除失败").arg(info->fileName));
        }
    });
}

void FileWidget::showProperty(FileInfo *info)
{

}

void FileWidget::showFileItems()
{
    int size = fileInfoList_.size();
    for (int i = 0; i < size; ++i) {
        FileInfo* info = fileInfoList_.at(i);
        addListWidgetItem(info);
    }
}

void FileWidget::uploadFile()
{
    QString filePath = QFileDialog::getOpenFileName();
    LOG_INFO(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR("文件打开失败");
        return;
    }
    int pos = filePath.lastIndexOf("/") + 1;
    QString fileName = filePath.mid(pos);

    QString boundary = common_->getBoundary();
    QByteArray data;
    data.append(boundary.toUtf8());
    data.append("\r\n");

    data.append("Content-Disposition: form-data;");
    data.append(QString("user = \"%1\" fileName = \"%2\" md5 = \"%3\" size = %4")
                    .arg(loginInfo_->user())
                    .arg(fileName)
                    .arg(common_->getFileMd5(filePath))
                    .arg(file.size()).toUtf8());
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");

    // 上传文件
    data.append(file.readAll());
    data.append("\r\n");
    data.append(boundary.toUtf8());

    if (file.isOpen()) {
        file.close();
    }
    QString url = QString("http://%1:%2/upload").arg(loginInfo_->ip())
                      .arg(loginInfo_->port());
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    // 发送http请求
    QNetworkReply* reply = networkManager_->post(request, data);
    if (reply == nullptr) {
        LOG_ERROR("HTTP请求失败");
        return;
    }

    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 文件上传成功中
        if (reply->error() != QNetworkReply::NoError) {
            LOG_ERROR(reply->errorString());
        } else {
            QByteArray json = reply->readAll();
            LOG_INFO(QString("array: %1").arg(QString(json)));
            QString code = NetworkData::getCode(json);
            if ("008" == code) {
                LOG_INFO("上传成功");
                getMyFileCount();
            } else if ("009" == code) {
                LOG_INFO("上传失败");
            }
        }
        reply->deleteLater();
    });
}

void FileWidget::uploadFile(UploadFileInfo *fileInfo)
{
    QFile file(fileInfo->filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOG_ERROR("文件打开失败");
        return;
    }
    QString boundary = common_->getBoundary();
    QByteArray data;
    data.append(boundary.toUtf8());
    data.append("\r\n");

    data.append("Content-Disposition: form-data;");
    data.append(QString("user = \"%1\" fileName = \"%2\" md5 = \"%3\" size = %4")
                    .arg(loginInfo_->user())
                    .arg(fileInfo->fileName)
                    .arg(fileInfo->md5)
                    .arg(fileInfo->size).toUtf8());
    data.append("\r\n");
    data.append("Content-Type: application/octet-stream");
    data.append("\r\n");
    data.append("\r\n");

    // 上传文件
    data.append(file.readAll());
    data.append("\r\n");
    data.append(boundary.toUtf8());

    if (file.isOpen()) {
        file.close();
    }
    QString url = QString("http://%1:%2/upload").arg(loginInfo_->ip())
                                                .arg(loginInfo_->port());
    QNetworkRequest request;
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
    // 发送http请求
    QNetworkReply* reply = networkManager_->post(request, data);
    if (reply == nullptr) {
        LOG_ERROR("HTTP请求失败");
        return;
    }

    connect(reply, &QNetworkReply::uploadProgress, this, [=](quint64 bytesSent, quint64 bytesTotal) {
        if (bytesTotal != 0) {
            fileInfo->progress->setProgressValue(bytesSent / 1024, bytesTotal / 1024);
        }
    });

    connect(reply, &QNetworkReply::finished, this, [=]() {
        // 文件上传成功中
        if (reply->error() != QNetworkReply::NoError) {
            LOG_ERROR(reply->errorString());
        } else {
            QByteArray json = reply->readAll();
            LOG_INFO(QString("array: %1").arg(QString(json)));
            QString code = NetworkData::getCode(json);
            if ("008" == code) {
                LOG_INFO("上传成功");
                fileInfo->uploadStatus = UPLOAD_FINISHED;
                common_->writeRecord(loginInfo_->user(), fileInfo->fileName, code);
                getMyFileCount();
            } else if ("009" == code) {
                LOG_INFO("上传失败");
                fileInfo->uploadStatus = UPLOAD_FAILED;
                common_->writeRecord(loginInfo_->user(), fileInfo->fileName, code);
            }
            uploadTask_->deleteTask();
        }
        reply->deleteLater();
    });
}

void FileWidget::getMyFileCount(FileDisplayMode mode)
{
    fileCount_ = 0;

    QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
    QString url = QString("http://%1:%2/myfiles?cmd=count").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramObj;
    paramObj.insert("user", loginInfo_->user());
    paramObj.insert("token", loginInfo_->token());
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply* reply = networkManager_->post(request, data);

    connect(reply, &QNetworkReply::readyRead, [=]() {
        QByteArray data = reply->readAll();
        LOG_INFO(data);
        QStringList list = NetworkData::getFileCount(data);
        if (!list.empty()) {
            QString code = list.at(0);
            if ("110" == code) {
                fileCount_ = list.at(1).toInt();
            } else if ("111" == code) {
                QMessageBox::warning(this, "账号异常", "请重新登录");
                emit sigLoginAgain();
                return;
            }
        }
        if (fileCount_ > 0) {
            getMyFileList(mode);
        } else {
            refreshFileItems();
        }
        reply->deleteLater();
    });
}

void FileWidget::getMyFileList(FileDisplayMode mode)
{
    QString strCmd;
    if (mode == FileDisplayMode::Normal) {
        strCmd = "normal";
    } else if (mode == FileDisplayMode::Asc) {
        strCmd = "pvasc";
    } else if (mode == FileDisplayMode::Desc) {
        strCmd = "pvdesc";
    }

    QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
    QString url = QString("http://%1:%2/myfiles?cmd=%3").arg(ip).arg(port).arg(strCmd);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramObj;
    paramObj.insert("user", loginInfo_->user());
    paramObj.insert("token", loginInfo_->token());
    paramObj.insert("start", 0);
    paramObj.insert("count", fileCount_);
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply* reply = networkManager_->post(request, data);

    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        QByteArray data = reply->readAll();
        LOG_INFO(QString("接收到数据: %1").arg(QString(data)));

        clearFileInfoList();

        QList<FileInfo*> fileList = NetworkData::getFileInfo(data);

        reply->deleteLater();

        clearItems();

        showFileItems();
    });
}

void FileWidget::refreshFileItems()
{
    // 首先情况原先的显示条目
    clearItems();

    // 如果文件列表不空，则显示文件列表
    if (!fileInfoList_.empty()) {
        for (int i = 0; i < fileInfoList_.size(); ++i) {
            FileInfo* info = fileInfoList_.at(i);

            // 添加到显示列表中
            addListWidgetItem(info);
        }
    }
}

void FileWidget::clearItems()
{
    int size = ui->listWidget->count();
    for (int i = 0; i < size; ++i) {
        QListWidgetItem* item = ui->listWidget->takeItem(0);
        delete item;
    }
}

void FileWidget::addUploadFiles()
{
    // 切换到下载界面
    emit sigSwitchToTransfer(TransformStatus::StatusDownload);
    QStringList filePathList = QFileDialog::getOpenFileNames();
    for (int i = 0; i < filePathList.size(); ++i) {
        QString filePath = filePathList.at(i);
        int res = uploadTask_->addTask(filePath);
        if (res == -1) {
            QMessageBox::warning(this, "警告", "上传文件不能超过30M");
        }
    }

}

void FileWidget::addDownloadFiles()
{
    // 切换到上传界面
    emit sigSwitchToTransfer(TransformStatus::StatusUpload);
    // 当前选中的ListWidgetItem
    QListWidgetItem* item = ui->listWidget->currentItem();
    if (item == nullptr) {
        LOG_ERROR("item 为空");
        return;
    }
    FileInfo* fileInfo = nullptr;
    int size = fileInfoList_.size();
    for (int i = 0; i < size; ++i) {
        fileInfo = fileInfoList_.at(i);
        if (fileInfo != nullptr) {
            if (fileInfo->fileName == item->text()) {
                QString filePath = QFileDialog::getSaveFileName(this, "请选择保存路径",
                                                                item->text());
                LOG_INFO(filePath);
                if (filePath.isEmpty()) {
                    return;
                }

                int res = downloadTask_->addTask(fileInfo, filePath);
                if (res == -2) {
                    common_->writeRecord(loginInfo_->user(), fileInfo->fileName, "091");
                }
            }
        }
    }
}

void FileWidget::checkTaskList()
{
    connect(&uploadTimer_, &QTimer::timeout, this, [=]() {
        uploadFileAction();
    });
    uploadTimer_.start(500);
    connect(&downloadTimer_, &QTimer::timeout, this, [=]() {
        downloadFileAction();
    });
    downloadTimer_.start(500);
}

void FileWidget::uploadFileAction()
{
    if (uploadTask_->isEmpty()) {
        return;
    }
    UploadFileInfo* uploadinfo = uploadTask_->takeTask();
    if (uploadinfo == nullptr) {
        return;
    }
    QNetworkRequest request;
    QString ip = common_->getConfigValue("web_server", "ip");
    QString port = common_->getConfigValue("web_server", "port");
    QString url = QString("http://%1:%2/md5").arg(ip).arg(port);
    request.setUrl(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));

    QJsonObject paramObj;
    paramObj.insert("token", loginInfo_->token());
    paramObj.insert("md5", uploadinfo->md5);
    paramObj.insert("fileName", uploadinfo->fileName);
    paramObj.insert("user", loginInfo_->user());
    QJsonDocument doc(paramObj);

    QByteArray data = doc.toJson();
    QNetworkReply* reply = networkManager_->post(request, data);

    connect(reply, &QNetworkReply::readyRead, this, [=]() {
        QByteArray data = reply->readAll();
        LOG_INFO(data);
        QString code = NetworkData::getCode(data);
        if ("005" == code) {
            uploadinfo->uploadStatus = UPLOAD_FILE_EXISTE;
            uploadTask_->deleteTask();
            common_->writeRecord(loginInfo_->user(), uploadinfo->fileName, code);
        } else if ("006" == code) {
            uploadTask_->deleteTask();
            common_->writeRecord(loginInfo_->user(), uploadinfo->fileName, code);
        } else if ("007" == code) {
            common_->writeRecord(loginInfo_->user(), uploadinfo->fileName, code);
            // 服务器没有此文件，上传文件
            uploadFile(uploadinfo);
        } else if ("111" == code) {
            QMessageBox::critical(this, "账号异常", "请重新登录");
            emit sigLoginAgain();
            return;
        }
        reply->deleteLater();
    });
}

void FileWidget::downloadFileAction()
{
    if (downloadTask_->isEmpty()) {
        return;
    }
    DownloadFileInfo* fileInfo = downloadTask_->takeTask();
    if (fileInfo == nullptr) {
        return;
    }
    QFile* file = fileInfo->file;
    QUrl url = QUrl(fileInfo->url);
    QNetworkRequest request;
    request.setUrl(url);

    QNetworkReply* replay = networkManager_->get(request);
    if (replay == nullptr) {
        downloadTask_->deleteTask();
        return;
    }
    connect(replay, &QNetworkReply::readyRead, [=]() {
        if (file != nullptr) {
            file->write(replay->readAll());
        }
    });
    connect(replay, &QNetworkReply::finished, [=]() {
        replay->deleteLater();
        common_->writeRecord(loginInfo_->user(), fileInfo->fileName, "090");
        common_->sleep(3000);
        downloadTask_->deleteTask();
    });
    connect(replay, &QNetworkReply::downloadProgress, this, [=](quint64 bytesSent, quint64 bytesTotal) {
        fileInfo->progress->setProgressValue(bytesSent / 1024, bytesTotal / 1024);
    });
}

void FileWidget::sltRightMenu(const QPoint& pos)
{
    QListWidgetItem* item = ui->listWidget->itemAt(pos);
    if (item == nullptr) {
        menuEmpty_->exec(QCursor::pos());
    } else {
        menuItem_->exec(QCursor::pos());
    }
}

void FileWidget::clearFileInfoList()
{
    int size = fileInfoList_.size();
    for (int i = 0; i < size; ++i) {
        FileInfo* info = fileInfoList_.takeFirst();
        if (info != nullptr) {
            delete info;
        }
    }
}
