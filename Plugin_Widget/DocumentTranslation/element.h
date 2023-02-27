#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QPainter>

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
    void settext(QString text);
    ~Help();

private:
    Ui::Help *ui;
};

#endif // ELEMENT_H
