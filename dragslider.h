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

    quint64 getPostionFromValue();

signals:
    void sliderBlockMoved(quint64 posX);

protected:
    void paintEvent(QPaintEvent *event) override;

    void resizeEvent(QResizeEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;

    void mouseMoveEvent(QMouseEvent* event) override;

    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void curValueChanged();

private:
    // 拖动进度条的高度
    qint32 sliderHeight_;
    // 拖动进度条的宽度
    qint32 sliderWidth_;
    // 进度条的最小值
    quint64 minValue_;
    // 进度条的最大值
    quint64 maxValue_;
    // 进度条的当前值
    quint32 curValue_;
    // 拖动滑块的当前位置
    QPoint curPos_;
    // 进度条的颜色
    QColor sliderColor_;
    // 拖动滑块的颜色
    QColor sliderBlockColor_;
    // 是否正处于拖动过程中
    bool isDragging_;
};

#endif // DRAGSLIDER_H
