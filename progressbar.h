#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include <QWidget>
#include <QColor>

class ProgressBar : public QWidget
{
    Q_OBJECT
public:
    explicit ProgressBar(QWidget *parent = nullptr);

    void setMinValue(quint64 val);

    void setMaxValue(quint64 val);

    void setCurValue(quint64 val);

    void setColor(QColor color);

signals:
    void sigPercent(quint8 percent);

protected:
    void paintEvent(QPaintEvent* event) override;

    void resizeEvent(QResizeEvent* event) override;

private:
    QColor color_;

    QPoint startPoint_;
    quint32 width_;
    quint32 height_;

    quint64 minValue_ = 0;
    quint64 maxValue_ = 100;
    quint64 curValue_ = 0;
};

#endif // PROGRESSBAR_H
