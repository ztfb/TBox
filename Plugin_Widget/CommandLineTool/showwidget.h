#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>
#include <QInputMethodEvent>
#include <QKeyEvent>
#include <QTimer>
#include <QPainter>
#include <QDebug>
#include <QtMath>
#include <windows.h>

class ShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowWidget(QWidget *parent = nullptr);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void wheelEvent(QWheelEvent *event);
    void paintEvent(QPaintEvent *event);
    void inputMethodEvent(QInputMethodEvent *event);
    void output(QString outText,QColor outColor,bool isNew=false);// 输出文本
    void newLine();// 换行
    void clear();// 清屏
    // 样式设置
    void setFont(QFont f);
    void setBackground(QColor b);
    void setCommand(QColor c);
    void setIsVisible(bool is);
    void setEncode(int encode);
    int initH;// 窗口高度
private:
    int currentN;// 当前行数
    int usedW;// 已经被使用的宽度
    QFont font;// 字体
    QColor background;// 背景色
    QColor command;// 输入命令颜色
    int cursorP;// 当前光标位置
    bool isVisible;// 光标是否显示
    bool isShow;// 光标闪烁控制
    QTimer* timer;// 光标闪烁计时器
    QList<QList<QPair<QColor,QString>>> history;// 历史记录
    QList<QPair<QColor,QString>> outText;// 输出文本
    QString inText;// 输入文本
    bool control;// ctrl键是否按下
    int encode;// 加密模式
    bool bottom;// 置底信号
private:// 工具函数
    int getLen(QString text,int i);// 获取文字的长度
    int getMaxIndex(int availableW,QString text);// 获取剩余长度最多绘制的文字的数量
    void writeText(QPair<QColor,QString> text,int availableW,int currentN,int encode=0);// 递归地在屏幕上绘制文字
    void writeCursor(int currentN,int usedW);
signals:
    void reachBottom();// 置底
    void inputFinished(QString cmd);// 输入完成
public slots:
};

#endif // SHOWWIDGET_H
