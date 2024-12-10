#ifndef BACKLABEL_H
#define BACKLABEL_H

#include <QLabel>

class BackLabel : public QLabel
{
    Q_OBJECT
public:
    explicit BackLabel(QWidget *parent = nullptr);

    void setBackPath(QString path);

protected:
    virtual void paintEvent(QPaintEvent* event) override;

    virtual void resizeEvent(QResizeEvent* event) override;

private:
    QString imgPath_;
};

#endif // BACKLABEL_H
