#ifndef DOWNLOADITEMWIDGET_H
#define DOWNLOADITEMWIDGET_H

#include <QWidget>
#include "progressbar.h"

namespace Ui {
class DownloadItemWidget;
}

class DownloadItemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DownloadItemWidget(QWidget *parent = nullptr);
    ~DownloadItemWidget();

    void setLabel(QString labelPath);

    void setFileText(QString text);

    void setToolButtonEnable();

    void setProgressBarMax(quint64 val);

    void setProgressBarValue(quint64 val);

    void setProgressBarColor(QColor color);

    QString fileName() const;

signals:
    void sigBtnOpenDir(QString fileName);

private slots:
    void sltShowPercent(quint8 percent);

    void on_tBtnOpenDir_clicked();

private:
    Ui::DownloadItemWidget *ui;
};

#endif // DOWNLOADITEMWIDGET_H
