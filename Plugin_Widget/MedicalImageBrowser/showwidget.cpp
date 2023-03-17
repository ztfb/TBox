#include "showwidget.h"

ShowWidget::ShowWidget(QWidget *parent) : QWidget(parent)
{
    Width=this->width();
    Height=this->height();
}
void ShowWidget::draw(QImage &image)
{
    this->image=image;
    // 窗口大小重置
    this->setMinimumSize(Width,Height);
    this->resize(Width,Height);
    // 根据图像和窗口大小决定显示
    this->setMinimumWidth((this->width()>image.width()?this->width():image.width()));
    this->setMinimumHeight((this->height()>image.height()?this->height():image.height()));
    this->update();// 主动调用绘图事件
}
void ShowWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setBrush(QBrush(QColor(0,0,0)));
    painter.drawRect(QRect(0,0,this->width(),this->height()));
    painter.drawImage(QRect(0,0,image.width(),image.height()),this->image);
}
void ShowWidget::mousePressEvent(QMouseEvent *event){
    if(event->button()==Qt::LeftButton){
        position=event->pos();//获取鼠标相对位置
        isMove=true;
    }
}
void ShowWidget::mouseMoveEvent(QMouseEvent *event){
    if(isMove){//当鼠标按下时才能拖拽
        QPoint p=event->pos();//获取临时位置
        // 根据p和position调整滚动条的位置
        this->positionChanged(position,p);// 发送位置改变信号
        position=p;//更新位置
    }
}
void ShowWidget::mouseReleaseEvent(QMouseEvent *event){
    isMove=false;
}
