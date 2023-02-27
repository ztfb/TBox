#ifndef INSTALLWIDGET_H
#define INSTALLWIDGET_H

#include <QWidget>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPluginLoader>
#include <QJsonDocument>
#include <QDir>
#include <QFile>
#include <QIcon>
#include <QDebug>
#include "PluginInterface.h"
#include "Element.h"

namespace Ui {
class InstallWidget;
}

class InstallWidget : public QWidget
{
    Q_OBJECT

public:
    explicit InstallWidget(QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    void showPlugin(QString dllPath,QString configPath);// 展示插件信息
    void getFileNamesFromDir(QString source);
    void installPlugin();// 安装插件
    void newPlugin();
    void deleteDir(QString source);
    void copyDir(QString source, QString target);
    ~InstallWidget();

private:
    Ui::InstallWidget *ui;
    QString theme;
    bool isSuccess;// 插件包是否识别成功
    QString IID;
    QString dllPath;
    QString configPath;
    QPixmap pixmap;
    QJsonObject data;
private:// 工具函数
    void clear(QString text,bool flag);
    void append(QJsonObject object,int start);
    void message(QString text);
    void setShow();
    void checkFile(QString path1,QString path2);// 检查拖入的项目是否正确
signals:
    void updateItem(QString ID,QJsonObject object);
    void addItem(QString ID,QJsonObject object);
};

#endif // INSTALLWIDGET_H
