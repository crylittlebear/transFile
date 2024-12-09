#include "dragslider.h"

#include <QPainter>
#include <qevent.h>

DragSlider::DragSlider(QWidget *parent)
    : QWidget{parent}
    , sliderColor_(0, 144, 211)
    , sliderBlockColor_(255, 255, 255)
    , isDragging_(false)
    , minValue_(0)
    , maxValue_(100)
    , curValue_(0)
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
    emit curValueChanged();

    // 每次更新curValue的时候都需要重新绘制进度条
    update();
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
    // 绘制进度条前半部分
    painter.setBrush(sliderColor_);
    QRect rectFont(2 * sliderHeight_, 2 * sliderHeight_, curPos_.x() - 2 * sliderHeight_, sliderHeight_);
    painter.drawRoundedRect(rectFont, 5, 5);
    // 绘制进度条后半部分
    painter.setBrush(Qt::white);
    QRect rectBack(curPos_.x(), 2 * sliderHeight_, sliderWidth_ - (curPos_.x() - 2 * sliderHeight_), sliderHeight_);
    painter.drawRoundedRect(rectBack, 5, 5);
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
    setCurValue(curValue_);
    // curPos_.setX(2 * sliderHeight_);
    // curPos_.setY(2 * sliderHeight_);

    qDebug() << "sliderHeight = " << sliderHeight_;
    qDebug() << "sliderWidth = " << sliderWidth_;
    qDebug() << "curValue = " << curValue_;
    QWidget::resizeEvent(event);
}

void DragSlider::mousePressEvent(QMouseEvent *event)
{
    // qDebug() << "mosue pressed";
    // qDebug() << "slider height = " << sliderHeight_;
    // qDebug() << "mouse pos x = " <<event->pos().x();
    // qDebug() << "curPos x = " << curPos_.x();
    // qDebug() << "mouse pos y = " <<event->pos().y();
    // qDebug() << "curPos y = " << curPos_.y();
    // if (inDragArea(event->pos())) {
    //     qDebug() << "true";
    // } else {
    //     qDebug() << "false";
    // }
    if (inDragArea(event->pos()) && (event->button() == Qt::LeftButton)) {
        // qDebug() << "slider block clicked!";
        isDragging_ = true;
    }
    QWidget::mousePressEvent(event);
}

void DragSlider::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging_ && (event->buttons() & Qt::LeftButton)) {
        if (event->pos().x() > sliderWidth_ + 2 * sliderHeight_) {
            curPos_.setX(sliderWidth_ + 2 * sliderHeight_);
        } else if (event->pos().x() < 2 * sliderHeight_) {
            curPos_.setX(2 * sliderHeight_);
        } else {
            curPos_.setX(event->pos().x());
        }
        // emit sliderBlockMoved(event->pos().x());
        qint32 moveDis = curPos_.x() - 2 * sliderHeight_;
        curValue_ = (moveDis * maxValue_) / sliderWidth_;
        update();
        qDebug() << "curValue = " << curValue_;
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

void DragSlider::curValueChanged()
{
    curPos_.setX(2 * sliderHeight_ + curValue_ * sliderWidth_ / maxValue_);
    curPos_.setY(this->height() / 2);
}

