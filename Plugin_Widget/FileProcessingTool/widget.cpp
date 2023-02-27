#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 开启子线程
    tool=new Tool;
    thread = new QThread;// 创建子线程
    tool->moveToThread(thread);
    thread->start();
    // 工作函数
    connect(this,&Widget::crypt,tool,&Tool::crypt);
    connect(tool,&Tool::cryptFinished,this,[=](){
        ui->crypyButton->setDisabled(false);
        this->message("加密/解密完成",false);
    });
    connect(this,&Widget::generateHash,tool,&Tool::generateHash);
    connect(tool,&Tool::hashValue,this,[=](QString value){
        ui->hashvaluelineEdit->setText(value);
        ui->generateButton->setDisabled(false);
    });
    connect(this,&Widget::rename,tool,&Tool::rename);
    connect(tool,&Tool::renameFinished,this,[=](){
        ui->renameButton->setDisabled(false);
        this->message("重命名完成",false);
    });
    // 样式设置
    ui->filepathlineEdit_2->setPlaceholderText("选择待处理文件...");
    ui->keylineEdit->setPlaceholderText("输入加密/解密密钥...");
    ui->filepathlineEdit->setPlaceholderText("选择待处理文件...");
    ui->hashvaluelineEdit->setPlaceholderText("正在生成文件哈希值...");
    ui->prefixlineEdit->setPlaceholderText("输入前缀固定字段...");
    ui->suffixlineEdit->setPlaceholderText("输入后缀固定字段...");
    ui->suffixlineEdit_2->setPlaceholderText("输入后缀名...");
    ui->helpButton->setToolTip("哈希值可以作为文件的唯一标识（不考虑碰撞）\n通过对比两个文件的哈希值可以确定两个文件是否完全相同，\n以此确定文件是否被篡改");
    // 连接信号槽
    connect(ui->selectfileButton_2,&QPushButton::clicked,this,[=](){
        QString fileName=QFileDialog::getOpenFileName(this,"选择文件");
        if(!fileName.isEmpty())ui->filepathlineEdit_2->setText(fileName);
    });
    connect(ui->crypyButton,&QPushButton::clicked,this,[=](){
        if(ui->filepathlineEdit_2->text().isEmpty())this->message("未选择文件",false);
        else if(ui->keylineEdit->text().isEmpty())this->message("未输入密钥",false);
        else{
            QString fileName=QFileDialog::getSaveFileName(this,"输入文件名");
            if(!fileName.isEmpty()){
                ui->crypyButton->setDisabled(true);
                this->crypt(ui->filepathlineEdit_2->text(),fileName,ui->keylineEdit->text().toUtf8());
            }
        }
    });
    connect(ui->selectfileButton,&QPushButton::clicked,this,[=](){
        QString fileName=QFileDialog::getOpenFileName(this,"选择文件");
        if(!fileName.isEmpty())ui->filepathlineEdit->setText(fileName);
    });
    connect(ui->generateButton,&QPushButton::clicked,this,[=](){
        if(ui->filepathlineEdit->text().isEmpty())this->message("未选择文件",false);
        else{
            ui->generateButton->setDisabled(true);
            ui->hashvaluelineEdit->setText("");
            this->generateHash(ui->filepathlineEdit->text(),ui->algorithmcomboBox->currentIndex());
        }
    });
    connect(ui->copyButton,&QPushButton::clicked,this,[=](){
        if(!ui->hashvaluelineEdit->text().isEmpty()){
            QClipboard *clipboard = QGuiApplication::clipboard();
            clipboard->setText(ui->hashvaluelineEdit->text());
            this->message("复制成功",false);
        }
    });
    connect(ui->selectfileButton_3,&QPushButton::clicked,this,[=](){
        fileNames.clear();
        fileNames=QFileDialog::getOpenFileNames(this,"批量选择文件");
        if(!fileNames.isEmpty())ui->filelabel->setText("已选择"+QString::number(fileNames.size())+"个文件");
        else ui->filelabel->setText("已选择...个文件");
    });
    connect(ui->renameButton,&QPushButton::clicked,this,[=](){
        if(fileNames.isEmpty())this->message("未选择文件",false);
        else{
            ui->renameButton->setDisabled(true);
            qRegisterMetaType<QList<QString>>("QList<QString>");
            this->rename(fileNames,ui->prefixlineEdit->text(),ui->spinBox->value(),ui->spinBox_2->value(),ui->suffixlineEdit->text(),ui->suffixlineEdit_2->text());
        }
    });
}
Widget::~Widget()
{
    delete ui;
    thread->quit();
    thread->wait();
    thread->deleteLater();
    tool->deleteLater();
}
bool Widget::message(QString text,bool flag){
        bool select=true;
        MessageBox m(text,flag,&select);
        m.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-m.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-m.height())/2);
        m.exec();
        if(flag)return select;
        else return false;
}
