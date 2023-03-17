#ifndef SHOWWIDGET_H
#define SHOWWIDGET_H

#include <QWidget>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <myimage.h>

class ShowWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ShowWidget(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void draw(QImage& image);
    void paintEvent(QPaintEvent *event);
private:
    // 初始化的宽度和高度
    int Width;
    int Height;
    QImage image;
    bool isMove;//鼠标是否拖拽
    QPoint position;//鼠标的当前位置
signals:
    void positionChanged(QPoint p1,QPoint p2);
public slots:
};

#endif // SHOWWIDGET_H
