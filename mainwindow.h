#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>
#include <QMediaPlayer>
#include <QListWidgetItem>

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
    void init();

private slots:
    void sltBtnMusicClicked();

    void sltOpenRecvDir(QString fileName);

    void sltHandleRecvPiece(const FileMetaData& metaData);

    void on_tBtnClose_clicked();
    // void on_tBtnOpen_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_tBtnMin_clicked();

    void on_tBtnMax_clicked();

private:
    Ui::MainWindow *ui;

    QTcpServer* server_;

    QMediaPlayer* player_ = nullptr;
};
#endif // MAINWINDOW_H
