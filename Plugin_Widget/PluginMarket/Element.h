#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QJsonObject>
#include <QJsonDocument>
#include <QDir>
#include <QDebug>

namespace Ui {
class PluginItem;
}

class PluginItem : public QWidget
{
    Q_OBJECT

public:
    explicit PluginItem(QWidget *parent = nullptr);
    void setText(QJsonObject object);
    QString getID();
    void setAvailable(bool flag);
    ~PluginItem();
private:
    Ui::PluginItem *ui;
    QString ID;
    bool isExisting;
signals:
    void download(bool isExisting,QString ID);
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
