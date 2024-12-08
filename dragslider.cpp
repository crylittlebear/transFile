#include "dragslider.h"

#include <QPainter>
#include <qevent.h>

DragSlider::DragSlider(QWidget *parent)
    : QWidget{parent}
    , sliderColor_(0, 62, 91)
    , sliderBlockColor_(255, 255, 255)
    , isDragging_(false)
{

}

void DragSlider::setSliderColor(QColor color)
{
    sliderColor_ = color;
}

void DragSlider::setSliderBlockColor(QColor color)
{
    sliderBlockColor_ = color;
}

void DragSlider::setMinValue(quint64 value)
{
    minValue_ = value;
}

void DragSlider::setMaxValue(quint64 value)
{
    maxValue_ = value;
}

void DragSlider::setCurValue(quint64 value)
{
    curValue_ = value;
}

quint64 DragSlider::curValue() const
{
    return curValue_;
}

quint64 DragSlider::maxValue() const
{
    return maxValue_;
}

quint64 DragSlider::minValue() const
{
    return minValue_;
}

bool DragSlider::inDragArea(QPoint pos)
{
    int dx = pos.x() - curPos_.x();
    int dy = pos.y() - curPos_.y();
    if ((dx * dx + dy * dy) < sliderHeight_ * sliderHeight_) {
        return true;
    }
    return false;
}

/*!
 * @brief 绘制可拖动Slider
*/
void DragSlider::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    // 绘制圆角矩形拖动条
    QRect rect(2 * sliderHeight_, 2 * sliderHeight_, sliderWidth_, sliderHeight_);
    painter.setBrush(QBrush(sliderColor_));
    painter.drawRoundedRect(rect, 5, 5);
    // 绘制拖动滑块
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(206, 206, 206));
    painter.setBrush(sliderBlockColor_);
    painter.setPen(pen);
    painter.drawEllipse(QPoint(curPos_.x(), height() / 2),
                        sliderHeight_, sliderHeight_);
}

void DragSlider::resizeEvent(QResizeEvent *event)
{
    int widgetWidth = event->size().width();
    int widgetHeight = event->size().height();

    sliderHeight_ = widgetHeight / 5;
    sliderWidth_ = widgetWidth - 4 * sliderHeight_;
    curPos_.setX(2 * sliderHeight_);
    curPos_.setY(2 * sliderHeight_);

    // 触发重绘
    update();
    QWidget::resizeEvent(event);
}

void DragSlider::mousePressEvent(QMouseEvent *event)
{
    if (inDragArea(event->pos()) && (event->button() == Qt::LeftButton)) {
        qDebug() << "click slider bolck!!!!!!!";
        isDragging_ = true;
    }
    QWidget::mousePressEvent(event);
}

void DragSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging_ && (event->buttons() & Qt::LeftButton)) {
        qDebug() << event->pos().x() << "left border = " << 2 * sliderHeight_;
        if (event->pos().x() > sliderWidth_ + 2 * sliderHeight_) {
            curPos_.setX(sliderWidth_ + 2 * sliderHeight_);
        } else if (event->pos().x() < 2 * sliderHeight_) {
            curPos_.setX(2 * sliderHeight_);
        } else {
            curPos_.setX(event->pos().x());
        }
        update();
    }
    QWidget::mouseMoveEvent(event);
}

void DragSlider::mouseReleaseEvent(QMouseEvent *event)
{
    if (isDragging_ && event->button() == Qt::LeftButton) {
        isDragging_ = false;
    }
    QWidget::mouseReleaseEvent(event);
}


