#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QMediaPlayer>
#include <QListWidgetItem>
#include <QTimer>
#include <QToolButton>

#include "dragwidget.h"
#include "serverthread.h"
#include "musiclistwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public DragWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void sigLogout();

private:
    void initMembers();

    void initConnections();

    void playMusic(QListWidgetItem* item);

    QString getRecvDir() const;

    void setToolButtonEnable(QToolButton* btn);

private slots:
    void sltBtnFileClicked();

    void sltBtnTransferClicked();

    void sltBtnMusicClicked();

    void sltBtnVideoClicked();

    void sltBtnLogoutClicked();

    void sltOpenRecvDir(bool flag);

    void sltHandleRecvPiece(const FileMetaData& metaData);

    void on_tBtnClose_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_tBtnMin_clicked();

    void on_tBtnMax_clicked();

    void on_listWidgetMusicList_itemDoubleClicked(QListWidgetItem *item);

    void on_tBtnPreMusic_clicked();

    void on_tBtnPlayPause_clicked();

    void on_tBtnNextMusic_clicked();

    void on_tBtnSearchMusic_clicked();

    void on_tBtnVolumn_clicked();

private:
    Ui::MainWindow *ui;

    QTcpServer* server_;

    // 播放器类
    QMediaPlayer* player_;
    // 所有音乐列表
    QStringList musicList_;
    // 音乐播放界面背景图列表
    QStringList backgroundList_;
    // 每隔一秒设置进度条前进一格
    QTimer* timer_;
    // 记录当前音乐播放进度
    quint32 musicSecondIndex_;
    // 记录当前音乐的总时长
    quint32 curMusicSecondLength_;
    // 当前播放的音乐条目
    QListWidgetItem* curPlayItem_;
};
#endif // MAINWINDOW_H
