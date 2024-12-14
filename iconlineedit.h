#pragma once

#include <QLineEdit>
#include <QLabel>
#include <QPaintEvent>

class Label : public QLabel {
public:
    Label(QWidget* parent = nullptr);
    ~Label();

    void setImgPath(QString filePath);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QString imgPath_;
};

class IconLineEdit  : public QLineEdit
{
public:
	IconLineEdit(QWidget *parent);
	~IconLineEdit();

	void setIconPath(QString path);

private:
    Label* label_;
};
