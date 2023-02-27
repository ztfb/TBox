#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QThread>
#include <QTcpServer>
#include "ftp.h"
#include "installwidget.h"

class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = nullptr);
protected:
    void incomingConnection(qintptr handle);
signals:
    void newConnected(qintptr socket);
public slots:
};

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void basicStyle();
    void startServer();// 开启服务器
    void closeServer();// 关闭服务器
    void pluginInit(bool flag);// 插件初始化
    void addItem(QString ID, QJsonObject object);
    void delPlugin(QString ID,QString name,PluginItem* value);// 卸载插件
    void deleteDir(QString source);
    void updateItem(QString ID, QJsonObject object);
    void findPlugin();// 查询插件
    ~Widget();

private:
    Ui::Widget *ui;
    TcpServer* tcpServer;
    QHash<QListWidgetItem*,PluginItem*> manageTable;
private:// 工具函数
    bool message(QString text,bool flag);
};
#endif // WIDGET_H
