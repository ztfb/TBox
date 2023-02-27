#include "messagebox.h"
#include "ui_messagebox.h"

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
