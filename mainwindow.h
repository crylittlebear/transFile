#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QVector>

#include "dragwidget.h"
#include "serverthread.h"

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
    void sltOpenRecvDir(QString fileName);

    void sltHandleRecvPiece(const FileMetaData& metaData);

    void on_tBtnClose_clicked();
    // void on_tBtnOpen_clicked();

private:
    Ui::MainWindow *ui;

    QTcpServer* server_;
};
#endif // MAINWINDOW_H
