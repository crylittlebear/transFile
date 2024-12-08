#include "dragwidget.h"

#include "qevent.h"
#include "qdebug.h"

DragWidget::DragWidget(QWidget *parent)
    : QMainWindow(parent)
    , isMoving_(false)
    , isScaling_(false)
    , direction_(ScaleDirection::None)
    , dragHeight_(32)
{
    // 设置窗口无边框
    setWindowFlag(Qt::FramelessWindowHint);
    // 设置窗口的最小尺寸
    setMinimumSize(700, 480);
}

DragWidget::~DragWidget()
{}

/*!
 * @brief 设置可拖动区域的高度
 * @param height 可拖动区域的高度值
*/
void DragWidget::setDragAreaHeight(int height)
{
    dragHeight_ = height;
}

void DragWidget::mousePressEvent(QMouseEvent * event)
{
    if (event->button() == Qt::LeftButton) {
        // 记录鼠标位置
        point_ = event->pos();
        // 如果处于缩放区域内，则设置缩放标记为true
        if (isInScaleArea(event->pos())) {
            isScaling_ = true;
            if (isInCornerTopLeft(event->pos())) {
                direction_ = ScaleDirection::BottomRight;
            } else if (isInTopBottomEdge(event->pos())) {
                direction_ = ScaleDirection::Down;
            } else if (isInLeftRightEdge(event->pos())) {
                direction_ = ScaleDirection::Right;
            }
        } else if (isInDragArea(event->pos())) {
            isMoving_ = true;
        }
    }
    event->accept();
}

void DragWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        if (isScaling_) {
            int tempWidth = this->geometry().width();
            int tempHeight = this->geometry().height();
            switch (direction_) {
            case ScaleDirection::BottomRight:
                tempWidth = event->globalPosition().toPoint().x() - this->geometry().x();
                tempHeight = event->globalPosition().toPoint().y() - this->geometry().y();
                break;
            case ScaleDirection::Right:
                tempWidth = event->globalPosition().toPoint().x() - this->geometry().x();
                break;
            case ScaleDirection::Down:
                tempHeight = event->globalPosition().toPoint().y() - this->geometry().y();
                break;
            default:
                break;
            }
            this->resize(tempWidth, tempHeight);
        } else if (isMoving_) {

            this->move(event->globalPosition().toPoint() - point_);
        }
    }
    event->accept();
}

void DragWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (isMoving_) {
        isMoving_ = false;
    }
    if (isScaling_) {
        isScaling_ = false;
    }
}

bool DragWidget::event(QEvent* event)
{
    if (event->type() == QEvent::HoverMove) {
        QMouseEvent* mEvent = static_cast<QMouseEvent*>(event);
        if (isInCornerTopLeft(mEvent->pos())) {
            this->setCursor(Qt::SizeFDiagCursor);
        } else if (isInCornerTopRight(mEvent->pos())) {
            this->setCursor(Qt::SizeBDiagCursor);
        } else if (isInLeftRightEdge(mEvent->pos())) {
            this->setCursor(Qt::SizeHorCursor);
        } else if (isInTopBottomEdge(mEvent->pos())) {
            this->setCursor(Qt::SizeVerCursor);
        } else {
            this->setCursor(Qt::ArrowCursor);
        }
    }
    return QWidget::event(event);
}

bool DragWidget::isInDragArea(QPoint p)
{
    if ((p.x() >= dragHeight_) && (p.x() <= width() - dragHeight_)
        && (p.y() < dragHeight_)) {
        return true;
    }
    return false;
}

bool DragWidget::isInScaleArea(QPoint p)
{
    return isInLeftRightEdge(p) || isInTopBottomEdge(p);
}

bool DragWidget::isInLeftRightEdge(QPoint p)
{
    if (p.x() < scaleDistance_ || p.x() > this->width() - scaleDistance_) {
        return true;
    }
    return false;
}

bool DragWidget::isInTopBottomEdge(QPoint p)
{
    if (p.y() < scaleDistance_ || p.y() > this->height() - scaleDistance_) {
        return true;
    }
    return false;
}

bool DragWidget::isInCornerTopLeft(QPoint p)
{
    if (p.x() < scaleDistance_ && p.y() < scaleDistance_
        || (p.x() > width() - scaleDistance_) && (p.y() > height() - scaleDistance_)) {
        return true;
    }
    return false;
}

bool DragWidget::isInCornerTopRight(QPoint p)
{
    if ((p.x() > width() - scaleDistance_) && (p.y() < scaleDistance_)
        || (p.x() < scaleDistance_) && (p.y() > height() - scaleDistance_)) {
        return true;
    }
    return false;
}
