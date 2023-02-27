#include "Element.h"
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

Help::~Help()
{
    delete ui;
}

Seed::Seed(int x,int y,int width,int height,int speed,int value,QWidget *parent) : QWidget(parent)
{
    state1=true;// 第一阶段
    state2=true;// 第一阶段
    opacity=0;
    scale=0.5+(value/100.0);
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        if(state2&&opacity<=100){// 第一阶段
            this->setWindowOpacity(opacity/100.0);opacity+=10;
            if(opacity==110){// 第一阶段结束
                timer->stop();
                QTimer::singleShot(2000,this,[=](){
                    state2=false;// 进入第二阶段
                    opacity=100;
                    timer->start(100);
                });
            }
        }else if(opacity>=0){
            this->setWindowOpacity(opacity/100.0);
            opacity-=10;
            if(opacity==-10){// 第二阶段结束
                timer->stop();
                this->close();
            }
        }
    });
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool|Qt::WindowStaysOnTopHint);// 设置窗口无边框，置顶，不在任务栏显示
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗口透明
    this->setFixedSize(qRound(64*scale),qRound(64*scale));//大小跟随系统
    //实现移动
    QPropertyAnimation* p=new QPropertyAnimation(this,"pos");
    int time=(100000/speed)*(height-y-this->height()-qRound(64*scale))/height;
    p->setDuration(time);
    p->setStartValue(QPoint(x+(width-this->width())/2,y));
    p->setEndValue(QPoint(x+(width-this->width())/2,height-this->height()-qRound(64*scale)));
    p->start();
    //移到到尽头时析构
    connect(p,&QPropertyAnimation::finished,this,[=](){
        state1=false;// 第二阶段
        this->setFixedSize(qRound(128*scale),qRound(128*scale));
        this->move(qRound(this->x()-32*scale),this->y());
        this->setWindowOpacity(0);
        this->update();
        timer->start(100);
    });//完成后自动关闭当前窗口
}
void Seed::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    if(state1)painter.drawPixmap(QRect(0,0,this->width(),this->height()),QPixmap(":/resource/seed.png"));
    else painter.drawPixmap(QRect(0,0,this->width(),this->height()),QPixmap(":/resource/heart.png"));
}

Ball::Ball(bool type,QWidget *parent) : QWidget(parent),type(type)
{
    this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);// 设置窗口无边框，不在任务栏显示
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗口透明
    //this->setFixedSize(64,64); //设置与素材相同大小
    p=new QPropertyAnimation(this,"pos");// 创建控制器
    p->setTargetObject(this);
    connect(p,&QPropertyAnimation::finished,this,&Ball::finished);
}
void Ball::showEvent(QShowEvent *event){
    this->setSize(value);
}
void Ball::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(QRect(0,0,this->width(),this->height()),QPixmap(":/resource/seed.png"));
}
void Ball::start(int x,int y,int r){
    this->setXY(x,y,r);
    this->beginMove();
}
void Ball::stop(){
    this->hide();
}
void Ball::setXY(int x, int y,int r){
    this->startX=x;
    this->startY=y;
    this->radius=r;
}
void Ball::beginMove(){
    int x=startX,y=startY,r=radius;
    int baseTime=500;
    QPainterPath path;
    path.moveTo(QPoint(x,y));// 移动到绘图点
    // 第一个参数是边界矩形，第二个参数是起始角度，第三个参数是角度偏移
    if(type){
        path.arcTo(QRect(x-r,y-r/2,r,r),0,180);
        path.arcTo(QRect(x-r,y-r,2*r,2*r),180,180);
        path.arcTo(QRect(x,y-r/2,r,r),0,180);
    }else{
        path.arcTo(QRect(x,y-r/2,r,r),180,-180);
        path.arcTo(QRect(x-r,y-r,2*r,2*r),0,-180);
        path.arcTo(QRect(x-r,y-r/2,r,r),180,-180);
    }
    p->setDuration(baseTime);
    p->setStartValue(QPoint(x,y));
    for(int i=0;i<=100;i++)
        p->setKeyValueAt(i/100.0,path.pointAtPercent(i/100.0));
    p->setEndValue(QPoint(x,y));
    p->start();
}
void Ball::setSize(int value){
    this->value=value;
    double scale=0.5+(value/100.0);
    this->setFixedSize(qRound(64*scale),qRound(64*scale));
}
void Ball::setisTop(bool flag,bool show){
    if(flag)this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);// 置顶
    else this->setWindowFlags(this->windowFlags()&~Qt::WindowStaysOnTopHint);// 取消置顶
    if(show)this->show();
}
Ball::~Ball(){
    p->deleteLater();
}

Toy::Toy(QWidget *parent) : QWidget(parent)
{
    isPress=false;
    type=QRandomGenerator::global()->bounded(3);// 生成一个0-2的整数
    this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗口透明
}
void Toy::showEvent(QShowEvent *event){
    this->currentPosition(this->x(),this->y());
    this->setSize(value);
}
void Toy::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        position=event->globalPos();//获取鼠标绝对位置
        isPress=true;
    }
}
void Toy::mouseMoveEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::OpenHandCursor);
        setCursor(cursor);
        QPoint p=event->globalPos();//获取当前位置
        this->move(this->pos().x()+p.x()-position.x(),this->pos().y()+p.y()-position.y());//调整窗口位置
        position=p;//更新位置
    }
}
void Toy::mouseReleaseEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::ArrowCursor);
        setCursor(cursor);
        isPress=false;
        this->currentPosition(this->x(),this->y());
    }
}
void Toy::draw(){
    type=QRandomGenerator::global()->bounded(3);// 生成一个0-2的整数
    this->update();
}
void Toy::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.drawPixmap(QRect(0,0,this->width(),this->height()),QPixmap(":/resource/toy-"+QString::number(type)+".png"));
}
void Toy::setSize(int value){
    this->value=value;
    double scale=0.5+(value/100.0);
    this->setFixedSize(qRound(200*scale),qRound(200*scale));
}

Door::Door(QWidget *parent) : QWidget(parent)
{
    isPress=false;
    type="fox";
    QPixmap pixmap("data/DesktopPet/fox-door.png");
    double scale=1.0*pixmap.width()/pixmap.height();
    this->setFixedSize(300,300);// 设置初始大小
    this->setFixedSize(qRound(this->height()*scale),this->height());// 适应图片大小
    this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗口透明
}
void Door::showEvent(QShowEvent *event){
    this->currentPosition(this->x(),this->y());
    this->setSize(value);
}
void Door::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        position=event->globalPos();//获取鼠标绝对位置
        isPress=true;
    }
}
void Door::mouseMoveEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::OpenHandCursor);
        setCursor(cursor);
        QPoint p=event->globalPos();//获取当前位置
        this->move(this->pos().x()+p.x()-position.x(),this->pos().y()+p.y()-position.y());//调整窗口位置
        position=p;//更新位置
        this->currentPosition(this->x(),this->y());
    }
}
void Door::mouseReleaseEvent(QMouseEvent *event)
{
    QCursor cursor(Qt::ArrowCursor);
    setCursor(cursor);
    isPress=false;
}
void Door::draw(QString type){
    this->type=type;
    QPixmap pixmap("data/DesktopPet/"+type+"-door.png");
    double scale=1.0*pixmap.width()/pixmap.height();
    this->setFixedSize(qRound(this->height()*scale),this->height());// 适应图片大小
    this->update();
}
void Door::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.drawPixmap(QRect(0,0,this->width(),this->height()),QPixmap("data/DesktopPet/"+type+"-door.png"));
}
void Door::setSize(int value){
    this->value=value;
    double scale=0.5+(value/100.0);
    QPixmap pixmap("data/DesktopPet/"+type+"-door.png");
    double s=1.0*pixmap.width()/pixmap.height();
    this->setFixedSize(qRound(300.0*s*scale),qRound(300.0*scale));
}
