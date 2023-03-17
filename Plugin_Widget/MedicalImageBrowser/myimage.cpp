#include "myimage.h"

MyImage::MyImage()
{
    this->w=this->h=0;
}
void MyImage::copy(MyImage *image){
    if(this->w!=0&&this->h!=0){
        for(unsigned int i=0;i<this->w;i++)delete[] this->pixels[i];
        delete[] this->pixels;
    }
    this->w=static_cast<unsigned int>(image->width());
    this->h=static_cast<unsigned int>(image->height());
    this->pixels=new unsigned short*[this->w];
    for(unsigned int i=0;i<this->w;i++)this->pixels[i]=new unsigned short[this->h];
    for(unsigned int i=0;i<this->w;i++)
        for(unsigned int j=0;j<this->h;j++)
            pixels[i][j]=image->getPixel(i,j);
}
void MyImage::load(QString path){
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    file.read(reinterpret_cast<char*>(&(this->w)),4);
    file.read(reinterpret_cast<char*>(&(this->h)),4);
    this->pixels=new unsigned short*[this->w];
    for(unsigned int i=0;i<this->w;i++)this->pixels[i]=new unsigned short[this->h];
    // 由于数组寻址方式和坐标系寻址方式的差异，这里应该这样遍历！！
    for(unsigned int j=0;j<this->h;j++)
        for(unsigned int i=0;i<this->w;i++)
            file.read(reinterpret_cast<char*>(&(this->pixels[i][j])),2);
    file.close();
}
int MyImage::width(){
    return static_cast<int>(this->w);
}
int MyImage::height(){
    return static_cast<int>(this->h);
}
void MyImage::setPixel(int x,int y,unsigned short value){
    this->pixels[x][y]=value;
}
unsigned short MyImage::getPixel(int x,int y){
    return this->pixels[x][y];
}
void MyImage::save(QString path){
    QFile file(path);
    file.open(QIODevice::WriteOnly);
    file.write(reinterpret_cast<const char*>(&(this->w)),4);
    file.write(reinterpret_cast<const char*>(&(this->h)),4);
    // 由于数组寻址方式和坐标系寻址方式的差异，这里应该这样遍历！！
    for(unsigned int j=0;j<this->h;j++)
        for(unsigned int i=0;i<this->w;i++)
            file.write(reinterpret_cast<const char*>(&(this->pixels[i][j])),2);
    file.close();
}
MyImage::~MyImage(){
    for(unsigned int i=0;i<this->w;i++)delete[] this->pixels[i];
    delete[] this->pixels;
}
