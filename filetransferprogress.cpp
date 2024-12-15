#include "filetransferprogress.h"
#include "ui_filetransferprogress.h"

FileTransferProcess::FileTransferProcess(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FileTransferProcess)
{
    ui->setupUi(this);
}

FileTransferProcess::~FileTransferProcess()
{
    delete ui;
}

void FileTransferProcess::setFileName(QString fileName)
{
    ui->label->setText(fileName);
}

void FileTransferProcess::setProgressValue(quint64 value, quint64 maxValue)
{
    ui->progressBar->setValue(value);
    ui->progressBar->setMaximum(maxValue);
}
