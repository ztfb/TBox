#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QComboBox>
#include <QFileDialog>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QFile>
#include <QSettings>
#include <QPluginLoader>
#include <QProcess>
#include <QMetaObject>
#include <QHash>
#include <QDebug>
#include "InstallWidget.h"
// 引入必要的动态链接库

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void systemTrayHandle(QSystemTrayIcon::ActivationReason reason);
    void basicStyle();// 基础设置
    void settingsInit();// 设置初始化
    void settingsSave();// 保存设置
    void setStyle();
    QString getStyleSheet(QString fileName);// 读取样式表文件
    void pluginInit(bool flag);// 插件初始化
    void addItem(QString ID, QJsonObject object);
    void delPlugin(QString ID,QString name,PluginItem* value);// 卸载插件
    void deleteDir(QString source);
    void updateItem(QString ID, QJsonObject object);
    void findPlugin();// 查询插件
    void runPlugin(QString ID,QString name);// 运行插件
    void selectPlugin(QString ID,Tab* value);// 选择插件
    void closePlugin(QString ID,Tab* value);// 关闭插件
    void closeAllPlugins();// 关闭所有插件

    ~Widget();

private:
    Ui::Widget *ui;
    bool isPress;//鼠标是否按下
    QPoint position;//鼠标的当前位置
    QSystemTrayIcon* systemTray;
    QMenu* menu;
    bool isMenu;
    QHash<QListWidgetItem*,PluginItem*> manageTable;
    QHash<QString,QPluginLoader*> loaders;// 插件加载器
    QHash<QString,Tab*> tabs;// 标签页
    QSize initSize;// 插件原始尺寸
    QProcess* process;// 启动TBox更新器
private:// 工具函数
    bool message(QString text,bool flag);
    void logoSwitch();// 切换logo
    void comboboxInit(QString filePath,QComboBox* combobox,bool flag);
    void aboutInit();// 关于界面初始化
private slots:
    void callfunc(QJsonObject object);// 调用函数
private:// 提供给插件调用的函数
    char funcname[50];// 函数名
    Q_INVOKABLE void flushPlugins();// 刷新插件列表
    Q_INVOKABLE void flushCombobox();// 刷新logo列表
signals:
    void send(QJsonObject message);
};
#endif // WIDGET_H
