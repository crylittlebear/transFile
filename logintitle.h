#pragma once

#include <QWidget>
#include "ui_logintitle.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginTitleClass; };
QT_END_NAMESPACE

class LoginTitle : public QWidget
{
	Q_OBJECT

public:
	LoginTitle(QWidget *parent = nullptr);
	~LoginTitle();

signals:
	void sigBtnConfigClicked();

	void sigBtnMinClicked();

	void sigBtnCloseClicked();

private slots:
	void sltBtnConfigClicked();

	void sltBtnMinClicked();

	void sltBtnCloseClicked();

private:
	Ui::LoginTitleClass *ui;
};
