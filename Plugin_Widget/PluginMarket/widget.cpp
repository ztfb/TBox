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
    connect(this,&Widget::findPlugin,ftp,&Ftp::findPlugin);
    connect(this,&Widget::downloadPlugin,ftp,&Ftp::downloadPlugin);
    connect(ftp,&Ftp::disconnection,this,[=](){this->message("连接断开，请检查网络",false);});
    connect(ftp,&Ftp::pluginInit,this,&Widget::pluginInit);
    connect(ftp,&Ftp::pluginInstall,this,&Widget::pluginInstall);
    ftp->moveToThread(thread);
    thread->start();
}
void Widget::basicStyle(){
    ui->ipLineEdit->setPlaceholderText("服务器IP地址...");
    ui->portLineEdit->setPlaceholderText("服务器端口号...");
    ui->lineEdit->setPlaceholderText("搜索插件...");
    ui->ipLineEdit->setVisible(false);
    ui->portLineEdit->setVisible(false);
    connect(ui->settingsButton,&QPushButton::clicked,this,[=](){
        ui->ipLineEdit->setVisible(!ui->ipLineEdit->isVisible());
        ui->portLineEdit->setVisible(!ui->portLineEdit->isVisible());
    });
    connect(ui->findButton,&QPushButton::clicked,this,[=](){
        if(ui->ipLineEdit->text().isEmpty())this->message("请配置服务器IP地址",false);
        else if(ui->portLineEdit->text().isEmpty())this->message("请配置服务器端口号",false);
        else this->findPlugin(ui->ipLineEdit->text(),ui->portLineEdit->text().toUShort(),ui->lineEdit->text());
    });
}
void Widget::pluginInit(QJsonArray array){
    // 先删除之前的所有窗口
    for(int i=0;i<ui->listWidget->count();i++)ui->listWidget->takeItem(i);
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        // 析构创建的项
        delete iter.key();
        delete iter.value();
    }
    manageTable.clear();
    // 添加item
    foreach (QJsonValue value, array) {
        QJsonObject object=value.toObject();
        PluginItem *w = new PluginItem;
        w->setText(object);
        connect(w,&PluginItem::download,this,[=](bool isExisting,QString ID){
            if(ui->ipLineEdit->text().isEmpty())this->message("请配置服务器IP地址",false);
            else if(ui->portLineEdit->text().isEmpty())this->message("请配置服务器端口号",false);
            else if(isExisting){
               if(this->message("是否进行版本更新？",true)) this->downloadPlugin(ui->ipLineEdit->text(),ui->portLineEdit->text().toUShort(),ID);
            }else this->downloadPlugin(ui->ipLineEdit->text(),ui->portLineEdit->text().toUShort(),ID);
        });
        QListWidgetItem *item = new QListWidgetItem;
        item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
        item->setSizeHint(QSize(750,60));
        ui->listWidget->addItem(item);
        ui->listWidget->setItemWidget(item,w);
        manageTable.insert(item,w);
    }
}
void Widget::pluginInstall(QString ID){
    QDir tempDir;if(!tempDir.exists("data/"+ID))tempDir.mkdir("data/"+ID);
    QString path="data/PluginMarket/"+ID;
    QDir dir(path);
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        if(iter->suffix()=="dll"){
            QFile::remove("plugins/"+iter->fileName());
            QFile::copy(path+"/"+iter->fileName(),"plugins/"+iter->fileName());
        }
        else{
            QFile::remove("data/"+ID+"/"+iter->fileName());
            QFile::copy(path+"/"+iter->fileName(),"data/"+ID+"/"+iter->fileName());
        }
        QFile::remove(path+"/"+iter->fileName());
    }
    dir.rmdir(dir.absolutePath());
    QPluginLoader *loader = new QPluginLoader("plugins/"+ID+".dll",this);
    if(loader->load()){
        PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if(!plugin){// 如果转换失败
            delete loader;loader = nullptr;
        }else{
            // 新建JSON文件
            QFile::remove("json/"+ID+".json");
            QJsonObject jsonObject=loader->metaData();
            QJsonObject data=jsonObject.value("MetaData").toObject();
            QFile file("json/"+ID+".json");
            file.open(QIODevice::WriteOnly);
            file.write(QJsonDocument(data).toJson());
            file.close();
            // 新建icon
            QFile::remove("json/"+ID+".png");
            QPixmap pixmap=plugin->getIcon();
            pixmap.save("json/"+ID+".png");
            if(loader->unload()){// 卸载插件
                delete loader;loader=nullptr;
            }
        }
    }else{
        delete loader;loader = nullptr;
    }
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        if(iter.value()->getID()==ID)iter.value()->setAvailable(false);
    }
    QJsonObject object;
    object.insert("func","flushPlugins");
    this->callfunc(object);
}
Widget::~Widget()
{
    delete ui;
    thread->quit();
    thread->wait();
    thread->deleteLater();
    ftp->deleteLater();
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        // 退出后析构创建的项
        delete iter.key();
        delete iter.value();
    }
}
bool Widget::message(QString text,bool flag){
        bool select=true;
        MessageBox m(text,flag,&select);
        m.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-m.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-m.height())/2);
        m.exec();
        if(flag)return select;
        else return false;
}
