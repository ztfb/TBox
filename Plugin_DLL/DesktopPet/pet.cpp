#include "pet.h"
#include "ui_pet.h"

Pet::Pet(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Pet)
{
    ui->setupUi(this);
    initSize=this->size();
    QScreen *screen = QGuiApplication::primaryScreen ();
    screenSize=screen->availableVirtualGeometry().size();
    isPress=false;
    type=-1;// 默认皮肤
    pets<<"fox"<<"totoro"<<"green"<<"kaguranana";
    movie=new QMovie(this);
    timeMovie=new QMovie(this);timeMovie->setFileName(":/resource/time.gif");
    ui->showWidget->setMovie(movie);
    ui->timelabel_2->setMovie(timeMovie);timeMovie->start();
    p=new QPropertyAnimation(this,"pos");// 创建控制器
    p->setTargetObject(this);
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Pet::petMove);
    dir=2;// 默认向左
    state=false;// 默认状态1
    ball1=new Ball(true,this);ball2=new Ball(false,this);//创建跟随光球
    ball1->setAttribute(Qt::WA_DeleteOnClose);ball2->setAttribute(Qt::WA_DeleteOnClose);//关闭时析构
    ball1->setAttribute(Qt::WA_ShowWithoutActivating);ball2->setAttribute(Qt::WA_ShowWithoutActivating);//新建的窗口不可抢占焦点
    connect(this,&Pet::Close,ball1,&Ball::close);connect(this,&Pet::Close,ball2,&Ball::close);//主窗口关闭时，从窗口也随之关闭
    connect(ball1,&Ball::finished,this,[=](){// 如果仍在隐藏模式，则开始新一轮循环
        if(mode==4)ball1->beginMove();
    });
    connect(ball2,&Ball::finished,this,[=](){// 如果仍在隐藏模式，则开始新一轮循环
        if(mode==4)ball2->beginMove();
    });
    this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);// 任务栏不显示，去边框
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗体透明
    this->setFocusPolicy(Qt::StrongFocus);// 获取强焦点
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
    QSizePolicy policy = ui->textEdit->sizePolicy();
    policy.setRetainSizeWhenHidden(true);
    ui->textEdit->setSizePolicy(policy);// 控件隐藏后保留占位
    ui->timelabel->setSizePolicy(policy);// 控件隐藏后保留占位
    QDateTime dateTime=QDateTime::currentDateTime();
    QDate date=dateTime.date();
    ui->timelabel->setText("2019.12.17-"+QString::number(date.year())+"."+QString::number(date.month())+"."+QString::number(date.day()));
    ui->timelabel_2->setSizePolicy(policy);// 控件隐藏后保留占位
    ui->timelabel->hide();
    ui->timelabel_2->hide();
    this->switchAppearance();
}
void Pet::moveEvent(QMoveEvent *event){
    if(mode==2&&object==2){// 互动，传送门
        // 如果碰到了传送门，则传送到另一侧
        int endX=0,endY=0;
        bool isMove=false;
        int cx=this->x(),cy=this->y()+ui->textEdit->height()+ui->showWidget->height()/2;
        if(cx>doorp1.x()&&cx<doorp1.x()+50
                &&cy>doorp1.y()+20&&cy<doorp1.y()+this->height()-20){
            isMove=true;
            endX=doorp2.x()-50;
            endY=doorp2.y()-ui->textEdit->height();
        }else if(cx>doorp2.x()&&cx<doorp2.x()+50
                 &&cy>doorp2.y()+20&&cy<doorp2.y()+this->height()-20){
             isMove=true;
             endX=doorp1.x()-50;
             endY=doorp1.y()-ui->textEdit->height();
         }
        if(isMove){// 传送
            timer->stop();
            p->stop();
            this->move(endX,endY);// 进行传送
            timer->start(10000);// 传送结束后重新行走
            this->petMove();
        }
    }
}
void Pet::closeEvent(QCloseEvent *event){
    this->Close();
}
void Pet::showEvent(QShowEvent *event){
    this->setSize(value);
}
void Pet::keyPressEvent(QKeyEvent *event){
    if(event->key()==Qt::Key_Space){
        if(switchKey==0)this->switchAppearance();
        else if(seedKey==0)this->throwSeed();
    }else if(event->key()>=Qt::Key_F1&&event->key()<=Qt::Key_F6){
        int index=event->key()-Qt::Key_F1+1;
        if(switchKey==index)this->switchAppearance();
        else if(seedKey==index)this->throwSeed();
    }else{
        int endX=0,endY=0;// 结束位置
        int len=this->width()/3;
        switch (event->key()) {
        case Qt::Key_Up:
            endX=this->x();endY=qMax(this->y()-len,0);break;
        case Qt::Key_Down:
            endX=this->x();endY=qMin(this->y()+len,screenSize.height()-this->height());break;
        case Qt::Key_Left:
            endY=this->y();endX=qMax(this->x()-len,0);break;
        case Qt::Key_Right:
            endY=this->y();endX=qMin(this->x()+len,screenSize.width()-this->width());break;
        }
        if(mode!=3)this->move(endX,endY);// 鼠标跟随模式下不能移动
        if(mode==1)this->move(this->x(),screenSize.height()-this->height());// 重力模式下只能在底层移动
        ball1->setXY(this->x()+this->width()/2,this->y(),this->height());
        ball2->setXY(this->x()+this->width()/2,this->y(),this->height());
    }
}
void Pet::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton&&event->y()>ui->textEdit->height()){
        position=event->globalPos();//获取鼠标绝对位置
        isPress=true;
    }
}
void Pet::mouseMoveEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::OpenHandCursor);
        setCursor(cursor);
        QPoint p=event->globalPos();//获取当前位置
        this->move(this->pos().x()+p.x()-position.x(),this->pos().y()+p.y()-position.y());//调整窗口位置
        position=p;//更新位置
        if(mode==1)this->move(this->x(),screenSize.height()-this->height());// 重力模式下只能在底层移动
        ball1->setXY(this->x()+this->width()/2,this->y(),this->height());
        ball2->setXY(this->x()+this->width()/2,this->y(),this->height());
    }
}
void Pet::mouseReleaseEvent(QMouseEvent *event)
{
    QCursor cursor(Qt::ArrowCursor);
    setCursor(cursor);
    isPress=false;
}
void Pet::mouseDoubleClickEvent(QMouseEvent *event){
    QString path="data/DesktopPet/"+pets[type]+"-";// 素材所在位置
    state=!state;
    if(QFile::exists(path+"2-left.gif")){// 存在第二形态
        this->setMovieFile(path,(state?"2":"1"),(dir==3?"right":"left"));
    }else movie->setPaused(state);// 不存在第二形态
}
void Pet::setisShow(bool flag){
    if(flag){
        this->show();
        if(mode==4){
            ball1->show();
            ball2->show();
        }
    }
    else{
        this->hide();
        if(mode==4){
            ball1->hide();
            ball2->hide();
        }
    }
    isShow=flag;
}
void Pet::setisTop(bool flag){
    if(flag)this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);// 置顶
    else this->setWindowFlags(this->windowFlags()&~Qt::WindowStaysOnTopHint);// 取消置顶
    if(isShow)this->show();
    ball1->setisTop(flag,isShow&&(mode==4));
    ball2->setisTop(flag,isShow&&(mode==4));
}
void Pet::setSize(int value){
    this->value=value;
    double scale=0.5+(value/100.0);
    ui->textEdit->setFont(QFont("楷体",qRound(24*scale)));
    ui->timelabel->setFont(QFont("楷体",qRound(16*scale)));
    this->setFixedSize(qRound(initSize.width()*scale),qRound(initSize.height()*scale));
    movie->setScaledSize(ui->showWidget->size());
    timeMovie->setScaledSize(QSize(ui->textEdit->height(),ui->textEdit->height()));
    if(mode==1)this->move(this->x(),screenSize.height()-this->height());// 重力模式下只能在底层移动
    ball1->setSize(value);
    ball2->setSize(value);
    ball1->setXY(this->x()+this->width()/2,this->y(),this->height());
    ball2->setXY(this->x()+this->width()/2,this->y(),this->height());
}
void Pet::setOpacity(double opacity){
    this->setWindowOpacity(opacity);
    ball1->setWindowOpacity(opacity);
    ball2->setWindowOpacity(opacity);
}
void Pet::settext(QString text){
    ui->textEdit->setText(text);
}
void Pet::setisVisible(bool flag){
    ui->textEdit->setVisible(flag);
}
void Pet::setBackground(QColor color){
    ui->textEdit->setStyleSheet("border:1px solid rgb(0,0,0);border-radius:5px;color: rgb(0, 0, 0);background-color:rgb("
                                +QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
    ui->timelabel->setStyleSheet("color:rgb("
                                 +QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
}
void Pet::setKey(int switchKey, int seedKey){
    this->switchKey=switchKey;
    this->seedKey=seedKey;
}
void Pet::setSpeed(int speed){
    this->speed=speed;
}
void Pet::setMode(int mode){
    this->mode=mode;
    if(mode!=2&&timer->isActive()){// 如果不是互动模式，就关闭动画
        timer->stop();// 停止动画
        p->stop();
    }
    else if(mode==2&&!timer->isActive()){// 如果是互动模式，就启动动画
        timer->start(10000);// 每10秒启动一次
        this->petMove();
    }
    if(mode==1)this->move(this->x(),screenSize.height()-this->height());// 重力模式，下沉
    if(mode==4){// 隐藏模式-启动窗口
        ui->timelabel->show();
        ui->timelabel_2->show();
        ball1->start(this->x()+this->width()/2,this->y(),this->height());
        ball2->start(this->x()+this->width()/2,this->y(),this->height());
        if(isShow){
            ball1->show();
            ball2->show();
        }
    }
    else{// 非隐藏模式-关闭窗口
        ui->timelabel->hide();
        ui->timelabel_2->hide();
        ball1->stop();
        ball2->stop();
    }
}
void Pet::setMovieFile(QString path,QString type,QString dir){
    movie->stop();
    if(QFile::exists(path+"2-left.gif")){// 存在第二形态
        movie->setFileName(path+type+"-"+dir+".gif");
        movie->start();
    }else{// 不存在第二形态
        movie->setFileName(path+"1-"+dir+".gif");
        movie->start();
        movie->setPaused(state);
    }
}
void Pet::setDoor1(int x, int y){
    doorp1.setX(x);
    doorp1.setY(y);
}
void Pet::setDoor2(int x, int y){
    doorp2.setX(x);
    doorp2.setY(y);
}
void Pet::traceCursor(int x, int y){
    this->move(x,y-ui->textEdit->height()-ui->showWidget->height()/2);
}
void Pet::objectSelect(int type){
    this->object=type;
    if((type==0||type==2)&&!timer->isActive()){// 道具关闭或者传送门打开，进入随机游走模式
        timer->start(10000);// 每10秒启动一次
        this->petMove();
    }else if(type==1&&timer->isActive()){// 玩具模式，关闭随机游走，开启玩具追踪
        timer->stop();// 停止动画
        p->stop();
    }
}
void Pet::traceToy(int x, int y){
    if(x<this->x())dir=2;// 切换方向
    else dir=3;
    this->setMovieFile("data/DesktopPet/"+pets[type]+"-",(state?"2":"1"),(dir==3?"right":"left"));
    p->stop();// 停止之前的跟踪
    int len=qRound(sqrt((x-this->x())*(x-this->x())+(y-this->y())*(y-this->y())));
    int time=qRound(1.0*len/(speed+50));
    p->setDuration(time*1000);
    p->setStartValue(QPoint(this->x(),this->y()));
    p->setEndValue(QPoint(x,y));
    p->start();
}
void Pet::switchAppearance(){
    type=(type+1)%pets.size();
    QString path="data/DesktopPet/"+pets[type]+"-";// 素材所在位置
    this->setMovieFile(path,(state?"2":"1"),(dir==3?"right":"left"));
    // 如果要清除movie，请使用：
    // ui->showWidget->setMovie(nullptr);
    this->appearanceChanged(pets[type]);// 发送切换信号
}
void Pet::throwSeed(){
    Seed* seed=new Seed(this->x(),this->y()+this->height(),this->width(),screenSize.height(),speed,value);//主窗口关闭时，该窗口随之关闭
    seed->setAttribute(Qt::WA_DeleteOnClose);//关闭时析构
    seed->setAttribute(Qt::WA_ShowWithoutActivating);//新建的窗口不可抢占焦点
    connect(this,&Pet::Close,seed,&Seed::close);//主窗口关闭时，从窗口也随之关闭
    seed->show();
}
void Pet::petMove(){// 随机移动
    int rand=QRandomGenerator::global()->bounded(8);// 生成一个0-7的整数，以此生成接下来移动的方向
    if(rand>=0&&rand<=1)dir=rand;
    else if(rand>=2&&rand<+4)dir=2;
    else dir=3;
    this->setMovieFile("data/DesktopPet/"+pets[type]+"-",(state?"2":"1"),(dir==3?"right":"left"));
    int time=QRandomGenerator::global()->bounded(8,10);// 随机获取移动所需时间（8-9秒）
    int len=(speed+50)*time;// 计算移动的路程
    p->setDuration(time*1000);// 设置移动时间
    p->setStartValue(QPoint(this->x(),this->y()));
    int endX=0,endY=0;// 结束位置
    switch (dir) {
    case 0:
        endX=this->x();endY=qMax(this->y()-len,0);break;
    case 1:
        endX=this->x();endY=qMin(this->y()+len,screenSize.height()-this->height());break;
    case 2:
        endY=this->y();endX=qMax(this->x()-len,0);break;
    case 3:
        endY=this->y();endX=qMin(this->x()+len,screenSize.width()-this->width());break;
    }
    p->setEndValue(QPoint(endX,endY));
    p->start();
}
Pet::~Pet()
{
    delete ui;
}
