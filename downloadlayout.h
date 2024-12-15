#ifndef DOWNLOADLAYOUT_H
#define DOWNLOADLAYOUT_H

#include <QLayout>
#include <QWidget>

class DownloadLayout
{
public:
    static DownloadLayout* instance();

    void setDownloadLayout(QWidget* parent);

    QVBoxLayout* getDownloadLayout();

private:
    DownloadLayout() = default;
    ~DownloadLayout() = default;

    static DownloadLayout* instance_;

    QVBoxLayout* verticalLayout_;
};

#endif // DOWNLOADLAYOUT_H
