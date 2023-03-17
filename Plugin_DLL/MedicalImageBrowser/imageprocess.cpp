#include "imageprocess.h"

ImageProcess::ImageProcess(QObject *parent) : QObject(parent)
{

}
void ImageProcess::grayReverse(MyImage* targetImage){
    for(int i=0;i<targetImage->width();i++)
        for(int j=0;j<targetImage->height();j++){
            unsigned short temp=targetImage->getPixel(i,j);
            targetImage->setPixel(i,j,4095-temp);
        }
    this->endGrayReverse();
}
void ImageProcess::grayWindow(int start,int end,MyImage* targetImage,QImage* showImage){
    QVector<QRgb> grayTable;
    for(int i=0;i<256;i++)grayTable.push_back(qRgb(i,i,i));
    QImage tempImage=QImage(targetImage->width(),targetImage->height(),QImage::Format_Indexed8);
    tempImage.setColorTable(grayTable);
    double k=255.0/(end-start);
    for(int i=0;i<targetImage->width();i++)
        for(int j=0;j<targetImage->height();j++){
            unsigned short temp=targetImage->getPixel(i,j);
            if(temp<start)temp=0;
            else if(temp>end)temp=255;
            else{
                temp=static_cast<unsigned short>(qRound((temp-start)*k));
            }
            tempImage.setPixel(i,j,temp);
        }
    *showImage=tempImage;
    this->endGrayWindow();// 发送处理完成信号
}
void ImageProcess::enhance(double a,double c,MyImage* image){
    short** d;
    d=new short*[image->width()];
    for(int i=0;i<image->width();i++)d[i]=new short[image->height()];
    if(a>0){
        // 使用拉普拉斯算子进行边界锐化
        //1.计算拉普拉斯算子
        double a2=-a;//（α=1+a）
        for(int i=0;i<image->width();i++){
            for(int j=0;j<image->height();j++){
                unsigned short temp[4]={};
                if(i-1<0)temp[0]=0;
                else temp[0]=image->getPixel(i-1,j);
                if(i+1>=image->width())temp[1]=0;
                else temp[1]=image->getPixel(i+1,j);
                if(j-1<0)temp[2]=0;
                else temp[2]=image->getPixel(i,j-1);
                if(j+1>=image->height())temp[3]=0;
                else temp[3]=image->getPixel(i,j+1);
                d[i][j]=static_cast<short>(temp[0]+temp[1]+temp[2]+temp[3]-4*image->getPixel(i,j));
            }
        }
        // 作用于图像
        for(int i=0;i<image->width();i++){
            for(int j=0;j<image->height();j++){
                short pixel=static_cast<short>(qRound(image->getPixel(i,j)+a2*d[i][j]));
                if(pixel<0)pixel=0;
                else if(pixel>4095)pixel=4095;
                image->setPixel(i,j,static_cast<unsigned short>(pixel));
            }
        }
    }
    if(c>0){
        // 再使用双边滤波器降低噪音，同时保护边缘
        //1.计算双边滤波器
        double c1=10*c,c2=150*c;
        for(int i=0;i<image->width();i++){
            for(int j=0;j<image->height();j++){
                double sum=0;
                double pixelSum=0;
                unsigned short cp=image->getPixel(i,j);
                sum+=1;pixelSum+=cp;
                if(i-1>=0&&j-1>=0){unsigned short p=image->getPixel(i-1,j-1);double temp=cf(2,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(j-1>=0){unsigned short p=image->getPixel(i,j-1);double temp=cf(1,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(i+1<image->width()&&j-1>=0){unsigned short p=image->getPixel(i+1,j-1);double temp=cf(2,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(i-1>=0){unsigned short p=image->getPixel(i-1,j);double temp=cf(1,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(i+1<image->width()){unsigned short p=image->getPixel(i+1,j);double temp=cf(1,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(i-1>=0&&j+1<image->height()){unsigned short p=image->getPixel(i-1,j+1);double temp=cf(2,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(j+1<image->height()){unsigned short p=image->getPixel(i,j+1);double temp=cf(1,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                if(i+1<image->width()&&j+1<image->height()){unsigned short p=image->getPixel(i+1,j+1);double temp=cf(2,c1)*ef(cp,p,c2);sum+=temp;pixelSum+=temp*p;}
                d[i][j]=static_cast<short>(qRound(pixelSum/sum));
            }
        }
        //2.作用于图像
        for(int i=0;i<image->width();i++){
            for(int j=0;j<image->height();j++){
                image->setPixel(i,j,static_cast<unsigned short>(d[i][j]));
            }
        }
    }
    for(int i=0;i<image->width();i++)delete[] d[i];
    delete[] d;
    this->endEnhance();
}
void ImageProcess::grayShowReverse(QImage* image){
    for(int i=0;i<image->width();i++)
        for(int j=0;j<image->height();j++){
            unsigned short temp=qRed(image->pixel(i,j));
            image->setPixel(i,j,255-temp);
        }
    this->endGrayShowReverse();
}
void ImageProcess::scale(double times,QImage* image){
    QVector<QRgb> grayTable;
    for(int i=0;i<256;i++)grayTable.push_back(qRgb(i,i,i));
    int w,h;// 缩放后的宽度和高度
    // 四舍五入的计算缩放后图像的宽高
    w=qRound(image->width()*times);
    h=qRound(image->height()*times);
    // 先初始化目标图片
    QImage tempImage=QImage(w,h,QImage::Format_Indexed8);
    tempImage.setColorTable(grayTable);
    // 使用反向映射计算目标图像中每一个点的像素值
    for(int i=0;i<tempImage.width();i++){
        for(int j=0;j<tempImage.height();j++){
            double x=i/times,y=j/times;
            // 使用双线性插值法计算(i,j)对应的像素值
            int x0=qFloor(x),y0=qFloor(y),x1=qCeil(x),y1=qCeil(y);
            if(x1>=image->width())x1=image->width()-1;
            if(y1>=image->height())y1=image->height()-1;
            double _x=x-x0,_y=y-y0;
            double f=qRed(image->pixel(x0,y0))*(1-_x)*(1-_y)+
                    qRed(image->pixel(x0,y1))*(1-_x)*_y+
                    qRed(image->pixel(x1,y0))*_x*(1-_y)+
                    qRed(image->pixel(x1,y1))*_x*_y;
            tempImage.setPixel(i,j,static_cast<unsigned int>(qRound(f)));
        }
    }
    *image=tempImage;
    this->endScale();
}
void ImageProcess::reverse(QImage* image){
    QImage temp=*image;
    for(int i=0;i<image->width();i++)
        for(int j=0;j<image->height();j++){
            image->setPixel(i,j,qRed(temp.pixel(image->width()-1-i,j)));
        }
    this->endReverse();
}
void ImageProcess::route(double angle, QImage *image){
    QVector<QRgb> grayTable;
    for(int i=0;i<256;i++)grayTable.push_back(qRgb(i,i,i));
    // 先初始化目标图片
    int cx=image->width()/2,cy=image->height()/2;
    double max=qSqrt((cx)*(cx)+(cy)*(cy));
    if(max<qSqrt((cx-image->width())*(cx-image->width())+(cy)*(cy)))max=qSqrt((cx-image->width())*(cx-image->width())+(cy)*(cy));
    if(max<qSqrt((cx)*(cx)+(cy-image->height())*(cy-image->height())))max=qSqrt((cx)*(cx)+(cy-image->height())*(cy-image->height()));
    if(max<qSqrt((cx-image->width())*(cx-image->width())+(cy-image->height())*(cy-image->height())))max=qSqrt((cx-image->width())*(cx-image->width())+(cy-image->height())*(cy-image->height()));

    QImage tempImage=QImage(cx+qRound(max),cy+qRound(max),QImage::Format_Indexed8);
    tempImage.setColorTable(grayTable);
    // 使用反向映射计算目标图像中每一个点的像素值
    double c=0;
    for(int i=0;i<tempImage.width();i++){
        for(int j=0;j<tempImage.height();j++){
            const double pi=3.1415926;
            double x=(i-cx)*qCos(angle*pi/180)+(j-cy)*qSin(angle*pi/180)+cx;
            double y=(j-cy)*qCos(angle*pi/180)-(i-cx)*qSin(angle*pi/180)+cy;
            // 使用双线性插值法计算(i,j)对应的像素值
            int x0=qFloor(x),y0=qFloor(y),x1=qCeil(x),y1=qCeil(y);
            if(x0>=0&&y0>=0&&x1<image->width()&&y1<image->height()){
                c++;
                double _x=x-x0,_y=y-y0;
                double f=qRed(image->pixel(x0,y0))*(1-_x)*(1-_y)+
                        qRed(image->pixel(x0,y1))*(1-_x)*_y+
                        qRed(image->pixel(x1,y0))*_x*(1-_y)+
                        qRed(image->pixel(x1,y1))*_x*_y;
                tempImage.setPixel(i,j,static_cast<unsigned int>(qRound(f)));
            }else{
                tempImage.setPixel(i,j,0);
            }
        }
    }
    *image=tempImage;
}
double ImageProcess::cf(int len, double c1){
    double temp=-1.0*len/(2*c1*c1);
    return qPow(2.71828,temp);
}
double ImageProcess::ef(unsigned short p1, unsigned short p2, double c2){
    unsigned short len=(p1-p2)*(p1-p2);
    double temp=-1.0*len/(2*c2*c2);
    return qPow(2.71828,temp);
}
