#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    logoPath="";
    this->logoInit();
    ui->IDLineEdit->setPlaceholderText("输入插件ID...");
    ui->nameLineEdit->setPlaceholderText("输入插件名...");
    ui->authorLineEdit->setPlaceholderText("输入插件作者...");
    ui->versionLineEdit->setPlaceholderText("输入插件版本...");
    ui->typeLineEdit->setPlaceholderText("输入插件类别...");
    ui->descriptionLineEdit->setPlaceholderText("输入插件描述...");
    ui->sizeLineEdit->setPlaceholderText("输入插件大小...");
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);
    connect(ui->findButton,&QPushButton::clicked,this,&Widget::findPlugin);
    connect(ui->selectButton,&QPushButton::clicked,this,[=](){
        QString fileName=QFileDialog::getOpenFileName(this,"选择图标","","图片(*png)");
        if(!fileName.isEmpty()){
            ui->logoButton->setIcon(QIcon(fileName));
            logoPath=fileName;
        }
    });
    connect(ui->pushButton,&QPushButton::clicked,this,&Widget::savePlugin);
    connect(ui->cancelButton,&QPushButton::clicked,this,&Widget::clear);
    connect(ui->newButton,&QPushButton::clicked,this,[=](){
        QString fileName=QFileDialog::getOpenFileName(this,"选择图标","","图片(*png)");
        if(!fileName.isEmpty()){
            QFileInfo fileinfo(fileName);
            if(QFile::exists("image/mylogo/"+fileinfo.fileName()))this->message("logo名已存在",false);
            else{
                QFile::copy(fileName,"image/mylogo/"+fileinfo.fileName());
                this->addItem(fileinfo.fileName().mid(0,fileinfo.fileName().size()-4));
                this->message("新增成功",false);
                QJsonObject object;
                object.insert("func","flushCombobox");
                this->callfunc(object);
            }
        }
    });
}
void Widget::findPlugin(){
    if(ui->IDLineEdit->text().isEmpty()){
        this->clear();
        this->message("请输入插件ID",false);
    }
    else if(!QFile::exists("json/"+ui->IDLineEdit->text()+".json")){
        this->clear();
        this->message("插件不存在",false);
    }
    else{// 如果插件存在
        QFile file("json/"+ui->IDLineEdit->text()+".json");
        file.open(QIODevice::ReadOnly);
        QJsonDocument document = QJsonDocument::fromJson(file.readAll());
        file.close();
        if (!document.isNull()){// 正确解析
            QJsonObject object=document.object();
            ui->nameLineEdit->setText(object.value("name").toString());
            ui->authorLineEdit->setText(object.value("author").toString());
            ui->versionLineEdit->setText(object.value("version").toString());
            ui->sizeLineEdit->setText(object.value("size").toString());
            ui->typeLineEdit->setText(object.value("type").toString());
            ui->descriptionLineEdit->setText(object.value("description").toString());
            ui->logoButton->setIcon(QIcon("json/"+ui->IDLineEdit->text()+".png"));
            logoPath="json/"+ui->IDLineEdit->text()+".png";
        }
    }
}
void Widget::savePlugin(){
    if(ui->IDLineEdit->text().isEmpty())this->message("插件ID为空",false);
    else if(!QFile::exists("json/"+ui->IDLineEdit->text()+".json"))this->message("插件不存在",false);
    else if(ui->nameLineEdit->text().isEmpty())this->message("插件名为空",false);
    else if(ui->authorLineEdit->text().isEmpty())this->message("作者为空",false);
    else if(ui->versionLineEdit->text().isEmpty())this->message("版本为空",false);
    else if(ui->sizeLineEdit->text().isEmpty())this->message("大小为空",false);
    else if(ui->typeLineEdit->text().isEmpty())this->message("类别为空",false);
    else if(ui->descriptionLineEdit->text().isEmpty())this->message("描述为空",false);
    else if(logoPath.isEmpty())this->message("未选择图标",false);
    else{// 保存插件信息
        if(this->message("确定保存？",true)){
            QJsonObject object;
            object.insert("name",ui->nameLineEdit->text());
            object.insert("author",ui->authorLineEdit->text());
            object.insert("version",ui->versionLineEdit->text());
            object.insert("size",ui->sizeLineEdit->text());
            object.insert("type",ui->typeLineEdit->text());
            object.insert("description",ui->descriptionLineEdit->text());
            QFile file("json/"+ui->IDLineEdit->text()+".json");
            file.open(QIODevice::WriteOnly);
            file.write(QJsonDocument(object).toJson());
            file.close();
            if(logoPath!="json/"+ui->IDLineEdit->text()+".png"){
                QFile::remove("json/"+ui->IDLineEdit->text()+".png");
                QFile::copy(logoPath,"json/"+ui->IDLineEdit->text()+".png");
            }
            this->message("保存成功",false);
            QJsonObject temp;
            temp.insert("func","flushPlugins");
            this->callfunc(temp);
        }
    }
}
void Widget::logoInit(){
    QDir dir("image/mylogo");
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        this->addItem(iter->fileName().mid(0,iter->fileName().size()-4));
    }
}
void Widget::addItem(QString name){
    // 添加item
    LogoItem *w = new LogoItem;
    w->setText(name);
    connect(w,&LogoItem::del,this,&Widget::delLogo);
    QListWidgetItem *item = new QListWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setSizeHint(QSize(450,50));
    ui->listWidget->addItem(item);
    ui->listWidget->setItemWidget(item,w);
    manageTable.insert(item,w);
}
void Widget::delLogo(QString name,LogoItem* value){
    if(this->message("确定删除["+name+"]？",true)){// 点击了确定按钮
        // 删除相应的文件
        QFile::remove("image/mylogo/"+name+".png");
        // 移除item
        QListWidgetItem* item=manageTable.key(value);
        ui->listWidget->removeItemWidget(item);
        manageTable.remove(item);
        delete item;delete value;
        // 弹出提示信息
        this->message("删除成功",false);
        QJsonObject object;
        object.insert("func","flushCombobox");
        this->callfunc(object);
    }
}
Widget::~Widget()
{
    delete ui;
    for(QHash<QListWidgetItem*,LogoItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
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
void Widget::clear(){
    QList<QLineEdit*> lines = ui->groupBox1->findChildren<QLineEdit*>();
    foreach (QLineEdit* line, lines) line->setText("");
    ui->logoButton->setIcon(QIcon(""));
    logoPath="";
}
