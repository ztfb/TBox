#include "Element.h"
#include "ui_messagebox.h"
#include "ui_tab.h"
#include "ui_pluginitem.h"
// logo控件
Logo::Logo(QWidget *parent) : QWidget(parent)
{
    currAngle=0;
    label=new QToolButton(this);
    label->setAutoRaise(true);
    label->setStyleSheet("border:none;");
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        currAngle=(currAngle+angle)%360;
        QTransform transform;
        transform.rotate(currAngle);
        QPixmap temp=image.transformed(transform, Qt::SmoothTransformation);
        if(mode==1||mode==3)label->setIcon(temp.copy((temp.width()-image.width())/2,(temp.height()-image.height())/2,image.width(),image.height()));
        else if(mode==2||mode==4)label->setIcon(temp);
    });
    this->setFocusPolicy(Qt::StrongFocus);
}
void Logo::resizeEvent(QResizeEvent *event){
    label->resize(this->size());
    int len=qMin(this->width(),this->height())-5;
    label->setIconSize(QSize(len,len));
}
void Logo::enterEvent(QEvent *event){
    if(mode==1||mode==2)timer->start(50);
}
void Logo::leaveEvent(QEvent *event){
    if(mode==1||mode==2){
        timer->stop();
        currAngle=0;
        label->setIcon(image);
    }
}
void Logo::setMode(int mode){
    this->mode=mode;
    switch (mode) {
    case 0:case 1:case 2:
        if(timer->isActive())timer->stop();
        currAngle=0;
        label->setIcon(image);
        break;
    case 3:case 4:if(timer->isActive())timer->stop();timer->start(50);break;
    }
}
void Logo::setAngle(int angle){
    this->angle=angle;
}
void Logo::setImage(QString path){
    image.load(path);
    label->setIcon(image);
}
// 消息控件
MessageBox::MessageBox(QString text,bool isShow,bool* select,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MessageBox)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint);// 去除标题栏
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
// 标签控件
Tab::Tab(QString text,QString ID,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Tab)
{
    ui->setupUi(this);
    ui->label->setText(text);
    flag=true;// 开始时为禁用态
    isPressed=false;
    ui->label->setDisabled(flag);
    this->ID=ID;
    connect(ui->toolButton,&QToolButton::clicked,this,[=](){
        this->closePlugin(ID,this);
    });
}
void Tab::mousePressEvent(QMouseEvent *event){
    isPressed=true;
}
void Tab::mouseReleaseEvent(QMouseEvent *event){
    if(isPressed){
        isPressed=false;
        if(!flag){
            flag=!flag;
            ui->label->setDisabled(flag);
            this->selectPlugin(ID,this);
        }
    }
}
QString Tab::getID(){
    return ID;
}
void Tab::setFlag(bool flag){
    this->flag=flag;
    ui->label->setDisabled(this->flag);
}
bool Tab::getFlag(){
    return flag;
}
Tab::~Tab()
{
    delete ui;
}
// 插件项
PluginItem::PluginItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginItem)
{
    ui->setupUi(this);
    connect(ui->runButton,&QToolButton::clicked,this,[=](){
        this->run(ID,ui->nameLabel->text());
    });
    connect(ui->deleteButton,&QToolButton::clicked,this,[=](){
        this->del(ID,ui->nameLabel->text(),this);
    });
}
void PluginItem::setText(QString ID,QJsonObject object){
    this->ID=ID;
    ui->logoButton->setIcon(QIcon("json/"+ID+".png"));
    ui->nameLabel->setText(object.value("name").toString());
    ui->authorLabel->setText("作者："+object.value("author").toString());
    ui->versionLabel->setText("版本："+object.value("version").toString());
    ui->typeLabel->setText("#"+object.value("type").toString());
    ui->descriptionLabel->setText(object.value("description").toString());
    ui->sizeLabel->setText(object.value("size").toString());
}
QString PluginItem::getID(){
    return ID;
}
void PluginItem::mouseDoubleClickEvent(QMouseEvent *event){// 双击也可运行
    this->run(ID,ui->nameLabel->text());
}
PluginItem::~PluginItem()
{
    delete ui;
}
