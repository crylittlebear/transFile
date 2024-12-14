#pragma once

#include "ui_logindialog.h"
#include "common.h"
#include "mainwindow.h"

#include <QMouseEvent>
#include <QRegExp>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialogClass; };
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
	Q_OBJECT

public:
	LoginDialog(QWidget *parent = nullptr);
	~LoginDialog();

signals:
    void sigLoginSuccess(QString userName);

protected:
	void mousePressEvent(QMouseEvent* event) override;

	void mouseMoveEvent(QMouseEvent* event) override;

	void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
	void sltBtnLoginClicked();

	void sltBtnRegClicked();

    void slgBtnRegBacktoLoginClicked();

	void sltBtnServerSetClicked();

    void slgBtnRegFontClicked();

private:
    void init();

    void buildConnections();

	void readConfigFile();

    void saveLoginInfoData(QString userName, QString token, QString ip, QString port);

private:
	Ui::LoginDialogClass *ui;

    QPoint position_;   // 记录鼠标当前位置，用于实现屏幕拖动
	bool isMoving_;

    Common* common_;    // common类，提供写入和读取配置Json文档函数
    QNetworkAccessManager* networkManager_;   // 网络接口类，用于实现Http请求与接收操作

    MainWindow* mainWindow_;
};
