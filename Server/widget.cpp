#include "widget.h"
#include "ui_widget.h"

TcpServer::TcpServer(QObject *parent) : QTcpServer(parent)
{

}
void TcpServer::incomingConnection(qintptr handle){
    this->newConnected(handle);
}

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->basicStyle();
    this->pluginInit(true);
}
void Widget::basicStyle(){
    // 样式
    this->setFixedSize(this->size());
    ui->stackedWidget->setCurrentIndex(0);
    ui->closeButton->setDisabled(true);
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
    ui->clearButton->setVisible(false);// 隐藏清除按钮
    ui->lineEdit->setPlaceholderText("搜索插件...");// 设置提示文本
    ui->serveButton->setDisabled(true);// 禁用按钮
    // 连接信号槽
    QHash<QString,int> indexs;
    indexs.insert("服务器",0);indexs.insert("包管理",1);
    QList<QPushButton*> buttons = ui->sideWidget->findChildren<QPushButton*>();
    foreach (QPushButton* btn, buttons) {// 按钮组
        connect(btn,&QPushButton::clicked,this,[=](){
            int index=indexs.value(btn->text());
            ui->stackedWidget->setCurrentIndex(index);
            foreach(QPushButton* temp, buttons)temp->setDisabled(false);
            btn->setDisabled(true);
        });
    }
    connect(ui->openButton,&QPushButton::clicked,this,&Widget::startServer);
    connect(ui->closeButton,&QPushButton::clicked,this,&Widget::closeServer);
    // 插件安装
    connect(ui->addButton,&QToolButton::clicked,this,[=](){
        InstallWidget installWidget(true);
        connect(&installWidget,&InstallWidget::addItem,this,[=](QString ID,QJsonObject object){
            this->addItem(ID,object);
            this->message("提交成功",false);
        });
        connect(&installWidget,&InstallWidget::updateItem,this,[=](QString ID,QJsonObject object){
            this->updateItem(ID,object);
            this->message("更新成功",false);
        });
        installWidget.move(this->x()+(this->width()-installWidget.width())/2,this->y()+(this->height()-installWidget.height())/2);
        installWidget.exec();
    });
    connect(ui->addButton_2,&QToolButton::clicked,this,[=](){
        InstallWidget installWidget(false);
        connect(&installWidget,&InstallWidget::commit,this,[=](){
            this->message("提交成功",false);
        });
        installWidget.move(this->x()+(this->width()-installWidget.width())/2,this->y()+(this->height()-installWidget.height())/2);
        installWidget.exec();
    });
    // 搜索框
    connect(ui->lineEdit,&QLineEdit::textChanged,this,&Widget::findPlugin);
    connect(ui->clearButton,&QToolButton::clicked,ui->lineEdit,&QLineEdit::clear);
}
void Widget::startServer(){
    if(!ui->portLineEdit->text().isEmpty()){
        ui->openButton->setDisabled(true);
        ui->closeButton->setDisabled(false);
        ui->portLineEdit->setReadOnly(true);
        tcpServer=new TcpServer(this);
        tcpServer->listen(QHostAddress::Any,ui->portLineEdit->text().toUShort());
        connect(tcpServer,&TcpServer::newConnected,this,[=](qintptr socket){
            QThread* thread=new QThread;// 创建子线程
            Ftp* ftp=new Ftp(socket);// 不能指定父对象，否则无法移动到子线程中
            ftp->moveToThread(thread);// 指定ftp的工作线程
            connect(ftp,&Ftp::finished,this,[=](){// 子线程工作完成后，进行对象的析构
                thread->quit();
                thread->wait();// 等到子线程任务全部完成后终止
                thread->deleteLater();
                ftp->deleteLater();
            });
            connect(ftp,&Ftp::addLog,ui->textEdit,&QTextEdit::append);
            ftp->log("接入");
            thread->start();// 启动子线程
        });
        this->message("服务器已开启",false);
    }else this->message("请配置监听端口",false);
}
void Widget::closeServer(){
    ui->openButton->setDisabled(false);
    ui->closeButton->setDisabled(true);
    ui->portLineEdit->setReadOnly(false);
    tcpServer->close();// 取消监听
    tcpServer->deleteLater();
    this->message("服务器已关闭",false);
}
void Widget::pluginInit(bool flag){
    QDir dir("json/");
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        if(iter->suffix()=="json"){
            QFile file("json/"+iter->fileName());
            file.open(QIODevice::ReadOnly);
            QJsonDocument document = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (!document.isNull()){// 正确解析
                QJsonObject object=document.object();
                if(flag||object.value("name").toString().contains(ui->lineEdit->text())){
                    QString ID=iter->fileName().mid(0,iter->fileName().size()-5);
                    this->addItem(ID,object);
                }
            }
        }
    }
}
void Widget::addItem(QString ID, QJsonObject object){
    // 添加item
    PluginItem *w = new PluginItem;
    w->setText(ID,object);
    connect(w,&PluginItem::del,this,&Widget::delPlugin);
    QListWidgetItem *item = new QListWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setSizeHint(QSize(750,60));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,w);
    manageTable.insert(item,w);
}
void Widget::delPlugin(QString ID,QString name,PluginItem* value){
    if(this->message("确定卸载插件["+name+"]？",true)){// 点击了确定按钮
        // 删除相应的文件
        QFile::remove("json/"+ID+".json");// 移除记录文件
        QFile::remove("json/"+ID+".png");// 移除图标
        QFile::remove("plugins/"+ID+".dll");// 移除动态链接库
        deleteDir("data/"+ID);// 移除文件夹
        // 移除item
        QListWidgetItem* item=manageTable.key(value);
        ui->listWidget->removeItemWidget(item);
        manageTable.remove(item);
        delete item;delete value;
        // 弹出提示信息
        this->message("卸载成功",false);
    }
}
void Widget::deleteDir(QString source){
    QDir dir(source);
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        if(iter->isFile())QFile::remove(iter->absoluteFilePath());
        else if(iter->isDir())deleteDir(iter->absoluteFilePath());
    }
    dir.rmdir(dir.absolutePath());
}
void Widget::updateItem(QString ID, QJsonObject object){
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        if(iter.value()->getID()==ID)iter.value()->setText(ID,object);
    }
}
void Widget::findPlugin(){
    // 根据文本是否为空决定是否显示清除按钮
    if(ui->lineEdit->text()=="")ui->clearButton->setVisible(false);
    else ui->clearButton->setVisible(true);
    // 先删除之前的所有窗口
    for(int i=0;i<ui->listWidget->count();i++)ui->listWidget->takeItem(i);
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        // 析构创建的项
        delete iter.key();
        delete iter.value();
    }
    manageTable.clear();
    // 查找符合条件的插件
    this->pluginInit(false);
}
Widget::~Widget()
{
    delete ui;
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        // 退出后析构创建的项
        delete iter.key();
        delete iter.value();
    }
}
bool Widget::message(QString text,bool flag){
        bool select=true;
        MessageBox m(text,flag,&select);
        m.move(this->x()+(this->width()-m.width())/2,this->y()+(this->height()-m.height())/2);
        m.exec();
        if(flag)return select;
        else return false;
}
