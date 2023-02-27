#include "element.h"
#include "ui_messagebox.h"
#include "ui_help.h"

SwitchButton::SwitchButton(QWidget *parent) : QWidget(parent)
{
    space=2;
    checked=false;
    bgColorOn=QColor(0,180,250);
    bgColorOff=QColor(190, 190, 190);
    sliderColorOn=QColor(255, 255, 255);
    sliderColorOff=QColor(255, 255, 255);
    startX=0;
}
void SwitchButton::resizeEvent(QResizeEvent *event){
    if (checked)startX=width()-height();
    else startX = 0;
    this->update();
}
void SwitchButton::drawBackGround(QPainter *painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(checked?bgColorOn:bgColorOff);
    QRect rect(0,0,width(),height());
    int side=qMin(width(),height());
    //左侧半圆
    QPainterPath path1;
    path1.addEllipse(rect.x(),rect.y(),side,side);
    //右侧半圆
    QPainterPath path2;
    path2.addEllipse(rect.width()-side,rect.y(),side,side);
    //中间矩形
    QPainterPath path3;
    path3.addRect(rect.x()+side/2,rect.y(),rect.width()-side, height());
    QPainterPath path=path1+path2+path3;
    painter->drawPath(path);
}

void SwitchButton::drawSlider(QPainter *painter)
{
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(checked?sliderColorOn:sliderColorOff));
    int sliderWidth=qMin(width(),height())-space*2;// 计算滑块宽度
    painter->drawEllipse(QRect(space+startX, space, sliderWidth, sliderWidth));// 绘制滑块
}

void SwitchButton::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing);//启用反锯齿
    //绘制背景
    drawBackGround(&painter);
    //绘制滑块
    drawSlider(&painter);
}
void SwitchButton::mousePressEvent(QMouseEvent *ev)
{
    this->setChecked(!checked);
    this->statusChanged(checked);
    update();
}
void SwitchButton::setChecked(bool flag){
    checked=flag;
    //计算滑块X轴终点坐标
    if (checked)startX=width()-height();
    else startX = 0;
    this->update();
}
bool SwitchButton::isChecked(){
    return checked;
}

MessageBox::MessageBox(QString text,bool isShow,bool* select,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);// 任务栏不显示，去边框
    this->setAttribute(Qt::WA_TranslucentBackground);// 窗口透明
    ui->label->setText(text);// 设置文本
    if(!isShow){
        ui->widget->hide();
        this->setWindowFlags(this->windowFlags()|Qt::Popup);// 点击窗口之外关闭
    }else this->setWindowFlags(this->windowFlags()|Qt::Tool);
    connect(ui->ackButton,&QToolButton::clicked,this,[=](){// 点击确定按钮
        if(select!=nullptr)*select=true;
        this->close();
    });
    connect(ui->nakButton,&QToolButton::clicked,this,[=](){// 点击取消按钮
        if(select!=nullptr)*select=false;
        this->close();
    });
}
MessageBox::~MessageBox()
{
    delete ui;
}

Help::Help(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Help)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Popup);// 去除标题栏，点击窗口之外关闭
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗体透明
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
}
void Help::settext(QString text){
    ui->textEdit->setText(text);
}
Help::~Help()
{
    delete ui;
}
