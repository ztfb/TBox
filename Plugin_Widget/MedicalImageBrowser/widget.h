#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaType>
#include <QScrollBar>
#include <imageprocess.h>
#include <myimage.h>
#include <showwidget.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void loadImage();// 图像导入
    void saveImage();// 图像导出
    void grawWindow();// 灰度窗变换
    void grayReverse();// 灰度反转
    void reverse();// 左右翻转
    void grayShowReverse();// 灰度反转
    void scale();// 缩放
    void enhance();// 图像增强
    ~Widget();

private:
    Ui::Widget *ui;
    ImageProcess* tools;// 图像处理工具
    QThread* sub;// 子线程
    ShowWidget* showWidget;
    MyImage sourceImage;// 源图像，不可修改
    MyImage targetImage;// 目标图像
    QImage showImage;// 展示
    // 标志位
    bool isLoad;
    bool isLoadShow;
private:// 工具函数
    void draw();
signals:
    void startGrayReverse(MyImage* targetImage);
    void startGrayWindow(int start,int end,MyImage* targetImage,QImage* showImage);
    void startReverse(QImage* showImage);
    void startGrayShowReverse(QImage* showImage);
    void startScale(double times,QImage* showImage);
    void startEnhance(double a,double c,MyImage* targetImage);
};
#endif // WIDGET_H
