#include "downloadtask.h"
#include "logger.h"
#include "downloadlayout.h"

DownloadTask* DownloadTask::task_ = new DownloadTask;

DownloadTask *DownloadTask::instance()
{
    return task_;
}

bool DownloadTask::isEmpty()
{
    return fileInfoList_.isEmpty();
}

DownloadFileInfo *DownloadTask::takeTask()
{
    DownloadFileInfo* task = nullptr;
    if (fileInfoList_.size() > 0) {
        task = fileInfoList_.at(0);
    }
    return task;
}

int DownloadTask::addTask(FileInfo *info, QString filePath, bool isShared)
{
    QFile* file = new QFile(filePath);
    if (!file->open(QIODevice::WriteOnly)) {
        LOG_ERROR("文件打开失败");
        delete file;
        file = nullptr;
        return -2;
    } else {
        DownloadFileInfo* downloadInfo = new DownloadFileInfo;
        downloadInfo->user = info->user;
        downloadInfo->fileName =info->fileName;
        downloadInfo->filePath = filePath;
        downloadInfo->md5 = info->md5;
        downloadInfo->url = info->url;
        downloadInfo->isShared = isShared;

        FileTransferProcess* progress = new FileTransferProcess;
        progress->setFileName(downloadInfo->fileName);
        downloadInfo->progress = progress;

        QVBoxLayout* layout = DownloadLayout::instance()->getDownloadLayout();
        layout->insertWidget(0, progress);

        fileInfoList_.append(downloadInfo);
        return 0;
    }
}

void DownloadTask::deleteTask()
{
    while (fileInfoList_.size() > 0) {
        DownloadFileInfo* downInfo = fileInfoList_.takeAt(0);
        if (downInfo != nullptr) {
            QVBoxLayout* layout = DownloadLayout::instance()->getDownloadLayout();
            layout->removeWidget(downInfo->progress);
        }
        delete downInfo->progress;
        QFile* file = downInfo->file;
        if (file->isOpen()) {
            file->close();
        }
        delete file;
        delete downInfo;
    }
}

void DownloadTask::clearTaskList()
{
    while (fileInfoList_.size() > 0) {
        DownloadFileInfo* downInfo = fileInfoList_.takeAt(0);
        if (downInfo != nullptr) {
            QVBoxLayout* layout = DownloadLayout::instance()->getDownloadLayout();
            layout->removeWidget(downInfo->progress);
        }
        delete downInfo->progress;
        delete downInfo;
    }
}


