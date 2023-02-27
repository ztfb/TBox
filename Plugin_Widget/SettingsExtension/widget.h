#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QFileDialog>
#include <QFile>
#include "Element.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void findPlugin();
    void savePlugin();
    void logoInit();
    void addItem(QString name);
    void delLogo(QString name,LogoItem* value);
    ~Widget();

private:
    Ui::Widget *ui;
    QString logoPath;
    QHash<QListWidgetItem*,LogoItem*> manageTable;
private:
    bool message(QString text,bool flag);
    void clear();
signals:
    void callfunc(QJsonObject object);
};
#endif // WIDGET_H
