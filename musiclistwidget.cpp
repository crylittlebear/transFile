#include "musiclistwidget.h"
#include "ui_musiclistwidget.h"

MusicListWidget::MusicListWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MusicListWidget)
{
    ui->setupUi(this);
}

MusicListWidget::~MusicListWidget()
{
    delete ui;
}

void MusicListWidget::setIcon(QString iconPath)
{
    QPixmap pixmap(iconPath);
    ui->labelIcon->setPixmap(pixmap);
    ui->labelIcon->setScaledContents(true);
}

void MusicListWidget::setName(QString name)
{
    ui->labelName->setText(name);
}
