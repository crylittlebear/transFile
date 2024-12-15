#ifndef UPLOADTASK_H
#define UPLOADTASK_H

#include <QString>
#include <QList>

#include "common.h"
#include "filetransferprogress.h"

#define UPLOAD_NOT              0
#define UPLOADING               1
#define UPLOAD_FINISHED         2
#define UPLOAD_FAILED           3
#define UPLOAD_FILE_EXISTE      4

struct UploadFileInfo {
    // 文件的MD5值
    QString md5;
    // 文件名称
    QString fileName;
    // 文件路径
    QString filePath;
    // 文件大小
    quint64 size;
    // 进度条
    FileTransferProcess* progress;
    // 上传状态（0：未上传，1：正在上传，2：上传完毕）
    int uploadStatus;
};

class UploadTask
{
public:
    // 获取唯一实例对象
    static UploadTask* instance();

    // 添加上传任务
    int addTask(QString filePath);

    // 判断任务列表是否为空
    bool isEmpty();

    // 取出任务
    UploadFileInfo* takeTask();

    // 删除任务
    void deleteTask();

    // 清空列表
    void clearFileList();

private:
    UploadTask() = default;
    ~UploadTask() = default;

private:
    static UploadTask* instance_;
    QList<UploadFileInfo*> fileList_;
};

#endif // UPLOADTASK_H
