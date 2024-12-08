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
    ui->labelIcon->setPixmap(pixmap.scaled(ui->labelIcon->size()));
}

void DownloadItemWidget::setFileText(QString text)
{
    ui->labelText->setText(text);
}

void DownloadItemWidget::setToolButtonEnable()
{
    ui->tBtnOpenDir->setEnabled(true);
}

void DownloadItemWidget::setProgressBarMax(quint64 val)
{
    ProgressBar* bar = static_cast<ProgressBar*>(ui->progressBar);
    bar->setMaxValue(val);
}

void DownloadItemWidget::setProgressBarValue(quint64 val)
{
    ProgressBar* bar = static_cast<ProgressBar*>(ui->progressBar);
    bar->setCurValue(val);
}

void DownloadItemWidget::setProgressBarColor(QColor color)
{
    ProgressBar* bar = static_cast<ProgressBar*>(ui->progressBar);
    bar->setColor(color);

    connect(bar, &ProgressBar::sigPercent, this, &DownloadItemWidget::sltShowPercent);
}

QString DownloadItemWidget::fileName() const
{
    return ui->labelText->text();
}

void DownloadItemWidget::sltShowPercent(quint8 percent)
{
    ui->labelPercent->setText(QString("%1 %").arg(percent));
}

void DownloadItemWidget::on_tBtnOpenDir_clicked()
{
    emit sigBtnOpenDir(true);
}

