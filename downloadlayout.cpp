#include "downloadlayout.h"

DownloadLayout* DownloadLayout::instance_ = new DownloadLayout;


DownloadLayout* DownloadLayout::instance()
{
    return instance_;
}

void DownloadLayout::setDownloadLayout(QWidget *parent)
{
    verticalLayout_ = new QVBoxLayout;
    verticalLayout_->addStretch();
    parent->setLayout(verticalLayout_);
}

QVBoxLayout *DownloadLayout::getDownloadLayout()
{
    return verticalLayout_;
}
