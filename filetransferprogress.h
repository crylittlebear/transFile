#ifndef FILETRANSFERPROGRESS_H
#define FILETRANSFERPROGRESS_H

#include <QWidget>

namespace Ui {
class FileTransferProcess;
}

class FileTransferProcess : public QWidget
{
    Q_OBJECT

public:
    explicit FileTransferProcess(QWidget *parent = nullptr);
    ~FileTransferProcess();

    void setFileName(QString fileName);

    void setProgressValue(quint64 value, quint64 maxValue);

private:
    Ui::FileTransferProcess *ui;
};

#endif // FILETRANSFERPROGRESS_H
