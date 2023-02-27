#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->basicStyle();
    thread=new QThread;
    ftp=new Ftp;
    connect(this,&Widget::checkUpdate,ftp,&Ftp::checkUpdate);
    connect(this,&Widget::TBoxUpdate,ftp,&Ftp::TBoxUpdate);
    connect(ftp,&Ftp::disconnection,this,[=](){this->message("连接断开，请检查网络",false);});
    connect(ftp,&Ftp::version,this,[=](QJsonObject object){
        ui->textEdit->append("当前TBox版本：");
        QSettings s("config/version.ini",QSettings::IniFormat);
        s.beginGroup("About");
        ui->textEdit->append(s.value("time").toString()+" v"+s.value("version").toString()+
                             (s.value("type").toString()=="0"?" 测试版":" 正式版"));
        s.endGroup();
        ui->textEdit->append("TBox最新版本：");
        ui->textEdit->append(object.value("time").toString()+" v"+object.value("version").toString()+
                             (object.value("type").toString()=="0"?" 测试版":" 正式版"));
        ui->textEdit->append("是否进行更新？");
        ui->updateButton->setText("更新");
    });
    connect(ftp,&Ftp::finished,this,&Widget::updateTBox);
    ftp->moveToThread(thread);
    thread->start();
}
void Widget::basicStyle(){
    ui->ipLineEdit->setPlaceholderText("服务器IP地址...");
    ui->portLineEdit->setPlaceholderText("服务器端口号...");
    ui->ipLineEdit->setVisible(false);
    ui->portLineEdit->setVisible(false);
    this->setMinimumSize(this->size());
    this->setMaximumSize(qRound(this->width()*1.2),qRound(this->height()*1.2));
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
    connect(ui->settingsButton,&QPushButton::clicked,this,[=](){
        ui->ipLineEdit->setVisible(!ui->ipLineEdit->isVisible());
        ui->portLineEdit->setVisible(!ui->portLineEdit->isVisible());
    });
    connect(ui->updateButton,&QPushButton::clicked,this,[=](){
        if(ui->ipLineEdit->text().isEmpty())this->message("请配置服务器IP地址",false);
        else if(ui->portLineEdit->text().isEmpty())this->message("请配置服务器端口号",false);
        else{
            if(ui->updateButton->text()=="检查更新")this->checkUpdate(ui->ipLineEdit->text(),ui->portLineEdit->text().toUShort());
            else if(ui->updateButton->text()=="更新"){
                if(this->message("确定进行更新？",true)){
                    ui->textEdit->setText("正在下载更新包...");
                    this->TBoxUpdate(ui->ipLineEdit->text(),ui->portLineEdit->text().toUShort());
                }
            }
        }
    });
}
void Widget::updateTBox(){
    QDir dir("temp");
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        if(iter->suffix()=="exe"){
            QFile::remove("TBox.exe");
            QFile::copy("temp/TBox.exe","TBox.exe");
        }else{
            QFile::remove("config/"+iter->fileName());
            QFile::copy("temp/"+iter->fileName(),"config/"+iter->fileName());
        }
        QFile::remove("temp/"+iter->fileName());
    }
    this->message("更新成功",false);
    QProcess* process=new QProcess;
    process->start("TBox.exe");
    this->close();
}
Widget::~Widget()
{
    delete ui;
    thread->quit();
    thread->wait();
    thread->deleteLater();
    ftp->deleteLater();
}
bool Widget::message(QString text,bool flag){
        bool select=true;
        MessageBox m(text,flag,&select);
        m.move(this->x()+(this->width()-m.width())/2,this->y()+(this->height()-m.height())/2);
        m.exec();
        if(flag)return select;
        else return false;
}
