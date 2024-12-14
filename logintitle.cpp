#include "logintitle.h"

LoginTitle::LoginTitle(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::LoginTitleClass())
{
	ui->setupUi(this);

	connect(ui->btnConfig, &QToolButton::clicked, this, &LoginTitle::sltBtnConfigClicked);
	connect(ui->btnMin, &QToolButton::clicked, this, &LoginTitle::sltBtnMinClicked);
	connect(ui->btnClose, &QToolButton::clicked, this, &LoginTitle::sltBtnCloseClicked);
}

LoginTitle::~LoginTitle()
{
	delete ui;
}

void LoginTitle::sltBtnConfigClicked() 
{
	emit sigBtnConfigClicked();
}

void LoginTitle::sltBtnMinClicked()
{
	emit sigBtnMinClicked();
}

void LoginTitle::sltBtnCloseClicked()
{
	emit sigBtnCloseClicked();
}

