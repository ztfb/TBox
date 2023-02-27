#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QColor>
#include <QPainter>
#include <QThread>
#include <QPropertyAnimation>
#include <QPaintEvent>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QRandomGenerator>
#include <QDebug>

class SwitchButton : public QWidget
{
    Q_OBJECT

public:
    explicit SwitchButton(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    void drawBackGround(QPainter *painter);
    void drawSlider(QPainter *painter);
    void setChecked(bool flag);
    bool isChecked();
    ~SwitchButton(){}
private:
    int space;//滑块距离边界距离
    bool checked;//是否选中

    QColor bgColorOn;         //打开时候的背景色
    QColor bgColorOff;        //关闭时候的背景色
    QColor sliderColorOn;     //打开时候滑块颜色
    QColor sliderColorOff;    //关闭时候滑块颜色

    int startX;//滑块开始X轴坐标
signals:
    void statusChanged(bool checked);
};

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
class Help;
}

class Help : public QDialog
{
    Q_OBJECT

public:
    explicit Help(QWidget *parent = nullptr);
    ~Help();

private:
    Ui::Help *ui;
};

class Seed : public QWidget
{
    Q_OBJECT
public:
    explicit Seed(int x,int y,int width,int height,int speed,int value,QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *event);
private:
    bool state1;// 阶段
    bool state2;// 阶段
    QTimer* timer;
    int opacity;
    double scale;
signals:

public slots:
};

class Ball : public QWidget
{
    Q_OBJECT
public:
    explicit Ball(bool type,QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
    void paintEvent(QPaintEvent *event);
    void start(int x,int y,int r);
    void stop();
    void setXY(int x,int y,int r);
    void beginMove();
    void setSize(int value);
    void setisTop(bool flag,bool show);// 设置是否置顶
    ~Ball();
private:
    bool type;
    int startX;
    int startY;// 开始位置
    int radius;// 半径
    QPropertyAnimation* p;// 行走控制器
    int value;// 大小缩放比例
signals:
    void finished();
public slots:
};

class Toy : public QWidget
{
    Q_OBJECT
public:
    explicit Toy(QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void draw();
    void setSize(int value);
private:
    bool isPress;//鼠标是否拖拽
    QPoint position;//鼠标的当前位置
    int type;
    int value;// 大小缩放比例
signals:
    void currentPosition(int x,int y);// 当前位置
public slots:
};

class Door : public QWidget
{
    Q_OBJECT
public:
    explicit Door(QWidget *parent = nullptr);
    void showEvent(QShowEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void paintEvent(QPaintEvent *event);
    void draw(QString type);
    void setSize(int value);
private:
    bool isPress;//鼠标是否拖拽
    QPoint position;//鼠标的当前位置
    QString type;
    int value;// 大小缩放比例
signals:
    void currentPosition(int x,int y);// 当前位置
public slots:
};

#endif // ELEMENT_H
