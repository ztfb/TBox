#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QJsonObject>

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

namespace Ui {
class PluginItem;
}

class PluginItem : public QWidget
{
    Q_OBJECT

public:
    explicit PluginItem(QWidget *parent = nullptr);
    void setText(QString ID,QJsonObject object);
    QString getID();
    ~PluginItem();
private:
    Ui::PluginItem *ui;
    QString ID;
signals:
    void del(QString ID,QString name,PluginItem* value);
};

#endif // ELEMENT_H
