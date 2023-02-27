#ifndef PET_H
#define PET_H

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QMovie>
#include <QTimer>
#include <QFile>
#include <QScreen>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QDateTime>
#include <QDebug>
#include "Element.h"

namespace Ui {
class Pet;
}

class Pet : public QWidget
{
    Q_OBJECT

public:
    explicit Pet(QWidget *parent = nullptr);
    void moveEvent(QMoveEvent *event);
    void closeEvent(QCloseEvent *event);
    void showEvent(QShowEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void setisShow(bool flag);// 设置是否可见
    void setisTop(bool flag);// 设置是否置顶
    void setSize(int value);// 设置大小
    void setOpacity(double opacity);// 设置透明度
    void settext(QString text);// 改变对话框文本
    void setisVisible(bool flag);// 设置对话框可见/不可见
    void setBackground(QColor color);// 设置背景颜色
    void setKey(int switchKey,int seedKey);// 设置快捷键
    void setSpeed(int speed);// 设置速度
    void setMode(int mode);// 设置模式
    void setMovieFile(QString path,QString type,QString dir);// 设置movie路径
    void setDoor1(int x,int y);// 传送门1
    void setDoor2(int x,int y);// 传送门2
    void traceCursor(int x,int y);// 跟随鼠标
    void objectSelect(int type);// 道具选择
    void traceToy(int x,int y);// 跟踪玩具
    void switchAppearance();// 切换皮肤
    void throwSeed();// 释放种子
    void petMove();// 随机移动
    ~Pet();

private:
    Ui::Pet *ui;
    bool isPress;//鼠标是否拖拽
    QPoint position;//鼠标的当前位置
    bool isShow;// 是否可见
    QSize initSize;// 初始大小
    QSize screenSize;// 屏幕大小
    int type;// 当前素材
    QStringList pets;// 素材列表
    int value;// 大小缩放值
    QMovie* movie;// 动图播放器
    QMovie* timeMovie;// 沙漏播放器
    int switchKey;// 换肤快捷键
    int seedKey;// 种子快捷键
    int speed;// 移动速度
    int mode;// 当前模式
    QPropertyAnimation* p;// 行走控制器
    QTimer* timer;
    int dir;// 当前移动方向
    // 0,1,2,3分别表示：上下左右
    Ball* ball1;// 跟随光球
    Ball* ball2;// 跟随光球
    QPoint doorp1;// 传送门1
    QPoint doorp2;// 传送门2
    int object;// 道具
    bool state;// 动图状态
signals:
    void Close();
    void appearanceChanged(QString type);
};

#endif // PET_H
