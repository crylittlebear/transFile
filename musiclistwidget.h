#ifndef MUSICLISTWIDGET_H
#define MUSICLISTWIDGET_H

#include <QWidget>

namespace Ui {
class MusicListWidget;
}

class MusicListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MusicListWidget(QWidget *parent = nullptr);
    ~MusicListWidget();

    void setIcon(QString iconPath);

    void setName(QString name);

    QString fileName() const;

private:
    Ui::MusicListWidget *ui;
};

#endif // MUSICLISTWIDGET_H
