#include "uploadlayout.h"

UploadLayout* UploadLayout::instance_ = new UploadLayout;


UploadLayout* UploadLayout::instance()
{
    return instance_;
}

void UploadLayout::setUploadLayout(QWidget *parent)
{
    verticalLayout_ = new QVBoxLayout;
    verticalLayout_->addStretch();
    parent->setLayout(verticalLayout_);
}

QVBoxLayout *UploadLayout::getUploadLayout()
{
    return verticalLayout_;
}
