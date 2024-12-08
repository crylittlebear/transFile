#include "progressbar.h"

#include <QPainter>
#include <QDebug>
#include <QResizeEvent>

ProgressBar::ProgressBar(QWidget *parent)
    : QWidget{parent}
{

}

void ProgressBar::setMinValue(quint64 val)
{
    minValue_ = val;
}

void ProgressBar::setMaxValue(quint64 val)
{
    maxValue_ = val;
}

void ProgressBar::setCurValue(quint64 val)
{
    curValue_ = val;
    update();
}

void ProgressBar::setColor(QColor color)
{
    color_ = color;
}

void ProgressBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawRoundedRect(QRect(startPoint_.x(), startPoint_.y(), width_, height_),
                            height_ / 2, height_ / 2);
    painter.setBrush(QBrush(color_));
    quint64 val = curValue_ * 100 / maxValue_;
    quint64 mod = (curValue_ * 100) % maxValue_;
    if (mod != 0) {
        val += 1;
    }
    emit sigPercent(val);
    quint64 wid = (width_ / 100.0) * val;
    if (wid > 0) {
        painter.drawRoundedRect(QRect(startPoint_.x(), startPoint_.y(), wid, height_),
                                height_ / 2, height_ / 2);
    }
}

void ProgressBar::resizeEvent(QResizeEvent *event)
{
    // 动态计算进度条的尺寸和位置
    int widgetHeight = event->size().height();
    int widgetWidth = event->size().width();

    height_ = widgetHeight * 1 / 3;
    width_ = widgetWidth - widgetHeight * 1 / 3;
    startPoint_.setX(widgetHeight / 3);
    startPoint_.setY(widgetHeight / 3);

    // 触发重绘
    update();
    QWidget::resizeEvent(event);
}
