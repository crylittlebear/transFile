#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H

#include <QString>
#include <QList>
#include <QFile>

#include "filetransferprogress.h"
#include "fileinfo.h"

struct DownloadFileInfo {
    // 文件
    QFile* file;
    // 下载用户
    QString user;
    // 文件的MD5值
    QString md5;
    // 文件名称
    QString fileName;
    // 文件路径
    QString filePath;
    // 下载地址
    QString url;
    // 是否共享文件
    bool isShared;
    // 进度条
    FileTransferProcess* progress;
};

// 下载任务列表
class DownloadTask
{
public:
    // 获取单例对象
    static DownloadTask* instance();

    // 判断任务列表是否为空
    bool isEmpty();

    // 取出任务
    DownloadFileInfo* takeTask();

    // 添加文件到上传任务列表
    int addTask(FileInfo* info, QString filePath, bool isShared = false);

    // 删除任务
    void deleteTask();

    // 清空任务列表
    void clearTaskList();

private:
    DownloadTask() = default;
    ~DownloadTask() = default;

    static DownloadTask* task_;

private:
    QList<DownloadFileInfo*> fileInfoList_;
};

#endif // DOWNLOADTASK_H
