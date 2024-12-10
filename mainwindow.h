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

private:
    void initMembers();

    void initConnections();

    void playMusic(QListWidgetItem* item);

    QString getRecvDir() const;

    void setToolButtonEnable(QToolButton* btn);

private slots:
    void sltBtnMusicClicked();

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

    QMediaPlayer* player_ = nullptr;

    QStringList musicList_;

    QStringList backgroundList_;

    QTimer* timer_;

    quint32 musicSecondIndex_;
    quint32 curMusicSecondLength_;
};
#endif // MAINWINDOW_H
