#include "uploadtask.h"
#include "logger.h"
#include "uploadlayout.h"

#include <QFileInfo>

UploadTask* UploadTask::instance_ = new UploadTask;

UploadTask *UploadTask::instance()
{
    return instance_;
}

int UploadTask::addTask(QString filePath)
{
    qint64 fileSize = QFileInfo(filePath).size();
    if (fileSize > 30 * 1024 * 1024) {
        LOG_ERROR("文件太大, 最大允许上传30M");
        return -1;
    }
    UploadFileInfo* uploadFile = new UploadFileInfo;
    int pos = filePath.lastIndexOf('/') + 1;
    uploadFile->fileName = filePath.right(filePath.size() - pos);
    uploadFile->filePath = filePath;
    uploadFile->md5 = Common::instance()->getFileMd5(filePath);
    uploadFile->uploadStatus = UPLOAD_NOT;

    FileTransferProcess* progress = new FileTransferProcess;
    progress->setFileName(uploadFile->fileName);
    uploadFile->progress = progress;

    QVBoxLayout* layout = UploadLayout::instance()->getUploadLayout();
    layout->insertWidget(0, progress);

    fileList_.append(uploadFile);
    return 0;
}

bool UploadTask::isEmpty()
{
    return fileList_.isEmpty();
}

UploadFileInfo *UploadTask::takeTask()
{
    UploadFileInfo* info = nullptr;
    if (fileList_.size() > 0) {
        info = fileList_.takeAt(0);
        info->uploadStatus = UPLOADING;
    }
    return info;
}

void UploadTask::deleteTask()
{
    for (int i = 0; i < fileList_.size(); ++i) {
        UploadFileInfo* info = fileList_.at(i);
        if (info->uploadStatus == UPLOAD_FINISHED || info->uploadStatus == UPLOAD_FAILED
                                                  || info->uploadStatus == UPLOAD_FILE_EXISTE) {
            info = fileList_.takeAt(0);
            if (info != nullptr) {
                QVBoxLayout* layout = UploadLayout::instance()->getUploadLayout();
                layout->removeWidget(info->progress);
            }
            delete info->progress;
            delete info;
        }
    }
}

void UploadTask::clearFileList()
{
    while (fileList_.size() > 0) {
        UploadFileInfo* info = fileList_.takeAt(0);
        if (info != nullptr) {
            QVBoxLayout* layout = UploadLayout::instance()->getUploadLayout();
            layout->removeWidget(info->progress);

            delete info->progress;
            delete info;
        }
    }
}
