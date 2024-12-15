#ifndef UPLOADLAYOUT_H
#define UPLOADLAYOUT_H

#include <QLayout>
#include <QWidget>

class UploadLayout
{
public:
    static UploadLayout* instance();

    void setUploadLayout(QWidget* parent);

    QVBoxLayout* getUploadLayout();

private:
    UploadLayout() = default;
    ~UploadLayout() = default;

    static UploadLayout* instance_;

    QVBoxLayout* verticalLayout_;
};

#endif // UPLOADLAYOUT_H
