#ifndef FILEWIDGET_H
#define FILEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QMenu>

#include "common.h"
#include "logininfoinstance.h"
#include "networkdata.h"
#include "uploadtask.h"
#include "downloadtask.h"

namespace Ui {
class FileWidget;
}

class FileWidget : public QWidget
{
    Q_OBJECT

public:
    enum class FileDisplayMode {
        Normal,
        Asc,
        Desc
    };

    explicit FileWidget(QWidget *parent = nullptr);
    ~FileWidget();

    // 刷新文件列表
    void refreshFiles();

    // 切换用户
    void changeUser();

signals:
    void sigLoginAgain();

    void sigSwitchToTransfer(TransformStatus status);

private:
    void initListWidget();

    void addMenu();

    void addListWidgetItem(FileInfo* info);

    void buildMenuConnections();

    void dealFile(QString cmd);

    void shareFile(FileInfo* info);

    void deleteFile(FileInfo* info);

    void showProperty(FileInfo* info);

    void showFileItems();

    void uploadFile();

    void uploadFile(UploadFileInfo* fileInfo);

    void getMyFileCount(FileDisplayMode mode = FileDisplayMode::Normal);

    void getMyFileList(FileDisplayMode mode = FileDisplayMode::Normal);

    void refreshFileItems();

    void clearItems();

    void clearFileInfoList();

    void addUploadFiles();

    void addDownloadFiles();

    // 定时器相关函数
    void checkTaskList();

    void uploadFileAction();

    void downloadFileAction();

private slots:
    void sltRightMenu(const QPoint& pos);

private:
    Ui::FileWidget *ui;

    Common* common_;
    UploadTask* uploadTask_;
    DownloadTask* downloadTask_;
    LoginInfoInstance* loginInfo_;
    QNetworkAccessManager* networkManager_;

    int fileCount_;
    QList<FileInfo*> fileInfoList_;
    QStringList fileTypeList_;

    QTimer uploadTimer_;
    QTimer downloadTimer_;

    QMenu* menuItem_;
    QMenu* menuEmpty_;

    QAction* actionDownload_;
    QAction* actionShare_;
    QAction* actionDelete_;
    QAction* actionProperty_;
    QAction* actionDownloadAsc_;
    QAction* actionDownloadDesc_;
    QAction* actionRefresh_;
    QAction* actionUpload_;
};

#endif // FILEWIDGET_H
