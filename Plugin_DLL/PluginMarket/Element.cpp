#include "Element.h"
#include "ui_pluginitem.h"
#include "ui_messagebox.h"

PluginItem::PluginItem(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PluginItem)
{
    ui->setupUi(this);
    isExisting=false;
    connect(ui->downloadButton,&QToolButton::clicked,this,[=](){
        this->download(isExisting,ID);
    });
}
void PluginItem::setText(QJsonObject object){
    this->ID=object.value("ID").toString();
    ui->nameLabel->setText(object.value("name").toString());
    ui->authorLabel->setText("作者："+object.value("author").toString());
    ui->versionLabel->setText("版本："+object.value("version").toString());
    ui->typeLabel->setText("#"+object.value("type").toString());
    ui->descriptionLabel->setText(object.value("description").toString());
    ui->sizeLabel->setText(object.value("size").toString());
    if(QFile::exists("json/"+ID+".json")){
        isExisting=true;
        QFile file("json/"+ID+".json");
        file.open(QIODevice::ReadOnly);
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (!document.isNull()){// 正确解析
            QJsonObject tempObject=document.object();
            if(tempObject.value("version").toString()==object.value("version").toString())ui->downloadButton->setDisabled(true);
        }
    }
}
QString PluginItem::getID(){
    return ID;
}
void PluginItem::setAvailable(bool flag){
    ui->downloadButton->setDisabled(!flag);
}
PluginItem::~PluginItem()
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
