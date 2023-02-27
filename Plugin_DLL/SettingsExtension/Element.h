#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QDebug>

namespace Ui {
class LogoItem;
}

class LogoItem : public QWidget
{
    Q_OBJECT

public:
    explicit LogoItem(QWidget *parent = nullptr);
    void setText(QString name);
    QString getName();
    ~LogoItem();
private:
    Ui::LogoItem *ui;
    QString name;
signals:
    void del(QString name,LogoItem* value);
};

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit MessageBox(QString text="",bool isShow=true,bool* select=nullptr,QWidget *parent = nullptr);
    ~MessageBox();

private:
    Ui::MessageBox *ui;
};

#endif // ELEMENT_H
