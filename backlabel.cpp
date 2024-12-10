#include "backlabel.h"

#include <QPainter>

BackLabel::BackLabel(QWidget *parent)
    : QLabel{parent}
{

}

void BackLabel::setBackPath(QString path)
{
    imgPath_ = path;
    update();
}

void BackLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    QPen pen;
    pen.setColor(Qt::gray);
    pen.setWidth(2);
    painter.setPen(pen);
    QPixmap pixmap(imgPath_);
    painter.drawPixmap(0, 0, this->width(), this->height(), pixmap);
}

void BackLabel::resizeEvent(QResizeEvent *event)
{
    update();
}
