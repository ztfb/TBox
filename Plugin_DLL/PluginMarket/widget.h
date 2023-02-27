#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QThread>
#include <QDir>
#include <QPluginLoader>
#include "ftp.h"
#include "Element.h"
#include "PluginInterface.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void basicStyle();
    void pluginInit(QJsonArray array);
    void pluginInstall(QString ID);
    ~Widget();

private:
    Ui::Widget *ui;
    QThread* thread;
    Ftp* ftp;// 负责请求插件信息
    QHash<QListWidgetItem*,PluginItem*> manageTable;
private:// 工具函数
    bool message(QString text,bool flag);
signals:
    void findPlugin(QString ip,ushort port,QString name);
    void downloadPlugin(QString ip, ushort port, QString ID);
    void callfunc(QJsonObject object);
};
#endif // WIDGET_H
