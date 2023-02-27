#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QScrollBar>
#include <QDateTime>
#include <QDesktopServices>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkInterface>
#include <QWindow>
#include <QDir>
#include <QDebug>
#include "showwidget.h"
#include "style.h"
#include "settings.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void analysis(QString cmd);// 命令解析函数
    // 功能函数
    void help();// 帮助文档
    void about();// 产品信息
    void ip();// 网络信息
    void time();// 当前时间
    void settings();// 样式设置
    void swallow();// 吞噬窗口
    void admin();// 管理员登录
    void exit();// 退出管理员
    void open(QString url);// 打开url
    void add();// 为url添加别名
    void del();// 删除url的别名
    void ls(QString title);// 查看所有url的别名
    void addLove();// 添加许可
    void delLove();// 删除许可
    void addTrans();// 添加许可
    void delTrans();// 删除许可
    ~Widget();

private:
    Ui::Widget *ui;
    ShowWidget* showWidget;
    Style style;// shell样式
    bool state;// 当前状态
    int mode;// 模式
    // 0 分析模式   1 输入模式  2 处理模式
    QEventLoop* loop;
    QString input;// 用户输入
    QWindow* window;// 吸入窗口
    QWidget* widget;// 吸入窗口
private:// 工具函数
    void tip();// 命令提示
    QString getInput(QString info, int encode=0);
    QJsonArray readAll(QString fileName);// 按行读取文件中所有数据
    bool find(QString fileName,QString target);// 判断文件中是否有target
    bool del(QString fileName,QString target);// 删除文件中的target，并返回结果
    void append(QString fileName,QString alias,QString path,QString description);// 添加别名
signals:
    void inputFinished();
};
#endif // WIDGET_H
