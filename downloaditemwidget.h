#ifndef DOWNLOADITEMWIDGET_H
#define DOWNLOADITEMWIDGET_H

#include <QWidget>

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

    void setProgressBarMax(quint64  value);

    void setProgressBarValue(quint64 value);

    void setToolButtonEnable();

signals:
    void sigBtnOpenDir();

private slots:
    void on_tBtnOpenDir_clicked();

private:
    Ui::DownloadItemWidget *ui;
};

#endif // DOWNLOADITEMWIDGET_H
