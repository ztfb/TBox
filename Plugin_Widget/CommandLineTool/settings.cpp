#include "settings.h"
#include "ui_settings.h"

Settings::Settings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);// 去除标题栏
    ui->lineEdit->setPlaceholderText("不超过10个字");
    connect(ui->cancelButton,&QToolButton::clicked,this,&Settings::close);
    connect(ui->saveButton,&QToolButton::clicked,this,&Settings::saveStyle);
    connect(ui->recoverButton,&QToolButton::clicked,this,&Settings::recoverStyle);
    init();
}
void Settings::init(){
    connect(ui->selectButton,&QToolButton::clicked,this,[=](){
        bool ok;
        QFont font=QFontDialog::getFont(&ok);
        if(ok){// 用户点了确定按钮
            ui->label_3->setText(font.family());
            this->style.setFont(font.family());
        }
    });
    connect(ui->selectButton_2,&QToolButton::clicked,this,[=](){
        QColor color=QColorDialog::getColor(Qt::white, this,tr("颜色选择"),QColorDialog::ShowAlphaChannel);
        if(color.isValid()){
            ui->label_5->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
            this->style.background=color;
        }
    });
    connect(ui->selectButton_3,&QToolButton::clicked,this,[=](){
        QColor color=QColorDialog::getColor(Qt::white, this,tr("颜色选择"),QColorDialog::ShowAlphaChannel);
        if(color.isValid()){
            ui->label_7->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
            this->style.tipUser=color;
        }
    });
    connect(ui->selectButton_4,&QToolButton::clicked,this,[=](){
        QColor color=QColorDialog::getColor(Qt::white, this,tr("颜色选择"),QColorDialog::ShowAlphaChannel);
        if(color.isValid()){
            ui->label_9->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
            this->style.tipAdmin=color;
        }
    });
    connect(ui->selectButton_5,&QToolButton::clicked,this,[=](){
        QColor color=QColorDialog::getColor(Qt::white, this,tr("颜色选择"),QColorDialog::ShowAlphaChannel);
        if(color.isValid()){
           ui->label_11->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
            this->style.cmd=color;
        }
    });
    connect(ui->selectButton_6,&QToolButton::clicked,this,[=](){
        QColor color=QColorDialog::getColor(Qt::white, this,tr("颜色选择"),QColorDialog::ShowAlphaChannel);
        if(color.isValid()){
            ui->label_13->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(color.red())+","+QString::number(color.green())+","+QString::number(color.blue())+");");
            this->style.info=color;
        }
    });
    QFile file("data/CommandLineTool/style.info");
    file.open(QIODevice::ReadOnly);
    file.read(reinterpret_cast<char*>(&style),sizeof(Style));
    file.close();
    ui->lineEdit->setText(style.getUserName());
    ui->label_3->setText(style.getFont());
    ui->label_5->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.background.red())+","+QString::number(style.background.green())+","+QString::number(style.background.blue())+");");
    ui->label_7->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.tipUser.red())+","+QString::number(style.tipUser.green())+","+QString::number(style.tipUser.blue())+");");
    ui->label_9->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.tipAdmin.red())+","+QString::number(style.tipAdmin.green())+","+QString::number(style.tipAdmin.blue())+");");
    ui->label_11->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.cmd.red())+","+QString::number(style.cmd.green())+","+QString::number(style.cmd.blue())+");");
    ui->label_13->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.info.red())+","+QString::number(style.info.green())+","+QString::number(style.info.blue())+");");
}
void Settings::saveStyle(){
    if(ui->lineEdit->text()=="")QMessageBox::critical(this,"警告","用户名不能为空");
    else{
        if(QMessageBox::question(this,"提示","确定保存？")==QMessageBox::Yes){
            QFile file("data/CommandLineTool/style.info");
            file.open(QIODevice::WriteOnly);
            style.setUserName(ui->lineEdit->text());
            file.write(reinterpret_cast<const char*>(&style),sizeof(Style));
            file.close();
            QMessageBox::information(this,"提示","保存成功");
            this->styleChanged();
            this->close();
        }
    }
}
void Settings::recoverStyle(){
    if(QMessageBox::question(this,"提示","确定恢复？")==QMessageBox::Yes){
        QFile file("data/CommandLineTool/copy.info");
        file.open(QIODevice::ReadOnly);
        file.read(reinterpret_cast<char*>(&style),sizeof(Style));
        file.close();
        QFile file2("data/CommandLineTool/style.info");
        file2.open(QIODevice::WriteOnly);
        file2.write(reinterpret_cast<const char*>(&style),sizeof(Style));
        file2.close();
        ui->lineEdit->setText(style.getUserName());
        ui->label_3->setText(style.getFont());
        ui->label_5->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.background.red())+","+QString::number(style.background.green())+","+QString::number(style.background.blue())+");");
        ui->label_7->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.tipUser.red())+","+QString::number(style.tipUser.green())+","+QString::number(style.tipUser.blue())+");");
        ui->label_9->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.tipAdmin.red())+","+QString::number(style.tipAdmin.green())+","+QString::number(style.tipAdmin.blue())+");");
        ui->label_11->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.cmd.red())+","+QString::number(style.cmd.green())+","+QString::number(style.cmd.blue())+");");
        ui->label_13->setStyleSheet("border:1px solid rgb(0,0,0);background-color:rgb("+QString::number(style.info.red())+","+QString::number(style.info.green())+","+QString::number(style.info.blue())+");");
        QMessageBox::information(this,"提示","恢复成功");
        this->styleChanged();
        this->close();
    }
}
Settings::~Settings()
{
    delete ui;
}
