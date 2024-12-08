#ifndef DRAGSLIDER_H
#define DRAGSLIDER_H

#include <QWidget>

class DragSlider : public QWidget
{
    Q_OBJECT
public:
    explicit DragSlider(QWidget *parent = nullptr);

    void setSliderColor(QColor color);

    void setSliderBlockColor(QColor color);

    void setMinValue(quint64 value);

    void setMaxValue(quint64 value);

    void setCurValue(quint64 value);

    quint64 curValue() const;

    quint64 maxValue() const;

    quint64 minValue() const;

    bool inDragArea(QPoint pos);

protected:
    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

private:
    qint32 sliderHeight_;
    qint32 sliderWidth_;

    quint64 minValue_;
    quint64 maxValue_;
    quint32 curValue_;

    QPoint curPos_;

    QColor sliderColor_;
    QColor sliderBlockColor_;

    bool isDragging_;
};

#endif // DRAGSLIDER_H
