#ifndef FILEINFO_H
#define FILEINFO_H

#include <QString>

struct FileInfo {
    // 用户名
    QString user;
    // 文件MD5值
    QString md5;
    // 文件的创建时间
    QString createTime;
    // 文件名
    QString fileName;
    // 分享状态
    int shareStatus;
    // 文件下载量，每下载一次加一
    int pv;
    // 文件的URL
    QString url;
    // 文件大小（以字节为单位）
    int size;
    // 文件类型
    QString fileType;
};

struct FileDownload {
    // 文件名
    QString fileName;
    // 文件下载量
    int pv;
};

#endif // FILEINFO_H
