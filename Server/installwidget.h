#ifndef INSTALLWIDGET_H
#define INSTALLWIDGET_H

#include <QDialog>
#include <QScrollBar>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QPluginLoader>
#include <QJsonDocument>
#include <QSettings>
#include <QDir>
#include <QDebug>
#include "Element.h"
#include "PluginInterface.h"

namespace Ui {
class InstallWidget;
}

class InstallWidget : public QDialog
{
    Q_OBJECT

public:
    explicit InstallWidget(bool flag,QWidget *parent = nullptr);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
    // 提交插件
    void showPlugin(QString dllPath,QString configPath);// 展示插件信息
    void getFileNamesFromDir(QString source);
    void installPlugin();// 安装插件
    void newPlugin();
    void deleteDir(QString source);
    void copyDir(QString source, QString target);
    // 提交TBox更新包
    void updateTBox();// 更新TBox
    void showTBox(QString path);
    ~InstallWidget();
private:
    Ui::InstallWidget *ui;
    bool flag;
    bool isSuccess;// 插件包是否识别成功
    QString IID;
    QString dllPath;
    QString configPath;
    QPixmap pixmap;
    QJsonObject data;
    QString path;
private:// 工具函数
    void clear(QString text);
    void append(QJsonObject object,int start);
    void message(QString text);
    void checkFile(QString path1,QString path2);// 检查拖入的项目是否正确
signals:
    void updateItem(QString ID,QJsonObject object);
    void addItem(QString ID,QJsonObject object);
    void commit();
};

#endif // INSTALLWIDGET_H
