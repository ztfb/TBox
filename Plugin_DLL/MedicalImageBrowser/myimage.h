#ifndef MYIMAGE_H
#define MYIMAGE_H
#include <QString>
#include <QFile>
class MyImage
{
public:
    MyImage();
    void copy(MyImage* image);
    void load(QString path);// 载入图像
    int width();
    int height();
    void setPixel(int x,int y,unsigned short value);
    unsigned short getPixel(int x,int y);
    void save(QString path);
    ~MyImage();
private:
    unsigned int w;
    unsigned int h;
    unsigned short** pixels;
};

#endif // MYIMAGE_H
