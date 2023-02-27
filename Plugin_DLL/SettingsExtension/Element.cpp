#include "Element.h"
#include "ui_logoitem.h"
#include "ui_messagebox.h"

LogoItem::LogoItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LogoItem)
{
    ui->setupUi(this);
    connect(ui->deleteButton,&QToolButton::clicked,this,[=](){
        this->del(name,this);
    });
}
void LogoItem::setText(QString name){
    this->name=name;
    ui->nameLabel->setText(name);
    ui->logoButton->setIcon(QIcon("image/mylogo/"+name+".png"));
}
QString LogoItem::getName(){
    return name;
}
LogoItem::~LogoItem()
{
    delete ui;
}

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
