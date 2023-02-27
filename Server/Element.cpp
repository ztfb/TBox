#include "Element.h"
#include "ui_messagebox.h"
#include "ui_pluginitem.h"

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

// 插件项
PluginItem::PluginItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginItem)
{
    ui->setupUi(this);
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
PluginItem::~PluginItem()
{
    delete ui;
}
