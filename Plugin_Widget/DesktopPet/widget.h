#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QColorDialog>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileInfo>
#include <QUrl>
#include <QTimer>
#include <QDebug>
#include "Element.h"
#include "pet.h"
#include "hook.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void mouseEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
    void settingsInit();// 设置初始化
    void settingsSave();// 保存设置
    void basicStyle();
    ~Widget();
private:
    void petInit(Pet* pet);
    bool message(QString text,bool flag);
private:
    Ui::Widget *ui;
    QList<Pet*>pets;
    Toy* toy;// 玩具
    Door* door1;// 传送门1
    Door* door2;// 传送门2
signals:
    void cursorMove(int x,int y);
    void objectSelect(int type);// 道具选择
    void currentPosition(int x,int y);// 玩具当前位置
    void currentPositionDoor1(int x,int y);// 传送门1当前位置
    void currentPositionDoor2(int x,int y);// 传送门2当前位置
};
#endif // WIDGET_H
