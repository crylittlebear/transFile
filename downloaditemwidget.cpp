#include "downloaditemwidget.h"
#include "ui_downloaditemwidget.h"

DownloadItemWidget::DownloadItemWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::DownloadItemWidget)
{
    ui->setupUi(this);

    ui->tBtnOpenDir->setEnabled(false);
}

DownloadItemWidget::~DownloadItemWidget()
{
    delete ui;
}

void DownloadItemWidget::setLabel(QString labelPath)
{
    QPixmap pixmap(labelPath);
    // qDebug() << "labelsize = " << ui->labelIcon->width() << ", " << ui->labelIcon->height();
    ui->labelIcon->setPixmap(pixmap.scaled(ui->labelIcon->size()));
}

void DownloadItemWidget::setFileText(QString text)
{
    ui->labelText->setText(text);
}

void DownloadItemWidget::setProgressBarMax(quint64 value)
{
    ui->progressBar->setMaximum(value);
}

void DownloadItemWidget::setProgressBarValue(quint64 value)
{
    ui->progressBar->setValue(value);
}

void DownloadItemWidget::setToolButtonEnable()
{
    ui->tBtnOpenDir->setEnabled(true);
}

void DownloadItemWidget::on_tBtnOpenDir_clicked()
{
    emit sigBtnOpenDir(ui->labelText->text());
}

