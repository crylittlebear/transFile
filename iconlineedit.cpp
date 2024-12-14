#include "iconlineedit.h"
#include <QPainter>
#include <QPixmap>

IconLineEdit::IconLineEdit(QWidget *parent)
	: QLineEdit(parent)
{
    label_ = new Label(this);
    int editHeight = this->height();
    label_->setFixedSize(editHeight * 3 / 5, editHeight * 3 / 5);
	label_->setVisible(false);

    // connect(this, &QLineEdit::selectionChanged, this, [=]() {
    //     label_->setVisible(false);
    // });

    // connect(this, &QLineEdit::editingFinished, this, [=]() {
    //     label_->setVisible(true);
    // });
}

IconLineEdit::~IconLineEdit()
{}

void IconLineEdit::setIconPath(QString path)
{
    label_->setImgPath(path);
	label_->setVisible(true);
    label_->setGeometry(height() / 5, height() / 5, label_->height(), label_->height());
    this->setTextMargins(this->height(), 1, 1, 1);
}

Label::Label(QWidget *parent) : QLabel(parent)
{

}

Label::~Label()
{

}

void Label::setImgPath(QString filePath)
{
    imgPath_ = filePath;
}

void Label::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    QPixmap pixmap(imgPath_);
    painter.drawPixmap(0, 0, width(), height(), pixmap);
}
