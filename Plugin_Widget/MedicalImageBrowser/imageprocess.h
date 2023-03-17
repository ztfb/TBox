#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include <QObject>
#include <QImage>
#include <QtMath>
#include <QVector>
#include <myimage.h>

class ImageProcess : public QObject
{
    Q_OBJECT
public:
    explicit ImageProcess(QObject *parent = nullptr);
    void grayReverse(MyImage* targetImage);// 灰度反转
    void grayWindow(int start,int end,MyImage* targetImage,QImage* showImage);// 灰度窗
    void enhance(double a,double c,MyImage* image);// 图像增强
    void grayShowReverse(QImage* image);
    void scale(double times,QImage* image);// 图像缩放
    void reverse(QImage* image);// 左右翻转
    void route(double angle,QImage* image);// 旋转
private:
    double cf(int len, double c1);
    double ef(unsigned short p1, unsigned short p2, double c2);
signals:
    void endGrayWindow();
    void endGrayReverse();
    void endReverse();
    void endGrayShowReverse();
    void endScale();
    void endEnhance();
public slots:
};

#endif // IMAGEPROCESS_H
