#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    board = QGuiApplication::clipboard();
    transWidget=new TranslationWidget;
    transWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(this,&Widget::translation,transWidget,&TranslationWidget::translation);
    connect(transWidget,&TranslationWidget::getText,ui->textEdit,&QTextEdit::append);
    // 设置
    ui->topSwitch->setChecked(true);
    transWidget->setisTop(true);// 默认置顶
    QSettings settings("data/DocumentTranslation/settings.ini",QSettings::IniFormat);
    ui->urlLineEdit->setText(settings.value("baiduURL").toString());
    ui->urlLineEdit_2->setText(settings.value("youdaoURL").toString());
    ui->appidLineEdit->setText(settings.value("baiduID").toString());
    ui->appidLineEdit_2->setText(settings.value("youdaoID").toString());
    ui->keyLineEdit->setText(settings.value("baiduKEY").toString());
    ui->keyLineEdit_2->setText(settings.value("youdaoKEY").toString());
    transWidget->baiduURL=settings.value("baiduURL").toString();
    transWidget->youdaoURL=settings.value("youdaoURL").toString();
    transWidget->baiduID=settings.value("baiduID").toString();
    transWidget->youdaoID=settings.value("youdaoID").toString();
    transWidget->baiduKEY=settings.value("baiduKEY").toString();
    transWidget->youdaoKEY=settings.value("youdaoKEY").toString();
    // 信号槽
    connect(board,&QClipboard::dataChanged,this,[=](){
        if(ui->startSwitch->isChecked())this->translation(board->text());
        if(ui->startSwitch_2->isChecked())ui->textEdit_2->append(board->text());
    });
    // 翻译窗
    connect(ui->showSwitch,&SwitchButton::statusChanged,transWidget,&TranslationWidget::setisShow);
    connect(ui->topSwitch,&SwitchButton::statusChanged,transWidget,&TranslationWidget::setisTop);
    connect(ui->clearbutton,&QPushButton::clicked,this,[=](){
        if(this->message("确定清空？",true))ui->textEdit->clear();
    });
    connect(ui->savebutton,&QPushButton::clicked,this,[=](){
        if(!ui->textEdit->toPlainText().isEmpty()){
            QString fileName=QFileDialog::getSaveFileName(this,"选择保存位置");
            if(!fileName.isEmpty()){
                QFile file(fileName);
                file.open(QIODevice::WriteOnly);
                QTextStream stream(&file);
                stream<<ui->textEdit->toPlainText();
                file.close();
                this->message("保存成功",false);
            }
        }
    });
    connect(ui->helpbutton,&QPushButton::clicked,this,[=](){// 帮助文档
        Help h;h.settext("开启翻译悬浮窗后，所有复制的文字都会自动进行翻译，并显示在悬浮窗上。你可以自由选择翻译引擎，翻译方向 。"
                         "如果你想保存译文，请在得到翻译结果后点击[添加译文]，译文将会被添加到下方文本框中。"
                         "注意：只有获得许可证才能使用百度翻译，请在[命令行工具]插件中使用命令add -trans添加许可证");
        h.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-h.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-h.height())/2);
        h.exec();
    });
    // 加强剪切板
    connect(ui->clearbutton_2,&QPushButton::clicked,this,[=](){
        if(this->message("确定清空？",true))ui->textEdit_2->clear();
    });
    connect(ui->savebutton_2,&QPushButton::clicked,this,[=](){
        if(!ui->textEdit_2->toPlainText().isEmpty()){
            QString fileName=QFileDialog::getSaveFileName(this,"选择保存位置");
            if(!fileName.isEmpty()){
                QFile file(fileName);
                file.open(QIODevice::WriteOnly);
                QTextStream stream(&file);
                stream<<ui->textEdit_2->toPlainText();
                file.close();
                this->message("保存成功",false);
            }
        }
    });
    connect(ui->helpbutton_2,&QPushButton::clicked,this,[=](){// 帮助文档
        Help h;h.settext("开启加强剪切板后，复制的所有文本都会被添加到下方文本框内。在阅读完成后，可以点击[保存]按钮将所复制的文本保存为文件。加强剪切板旨在"
                         "实现沉浸式无打断阅读。");
        h.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-h.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-h.height())/2);
        h.exec();
    });
}
void Widget::closeEvent(QCloseEvent *event){
    transWidget->close();
}
Widget::~Widget()
{
    delete ui;
}
bool Widget::message(QString text,bool flag){// 封装好的对话框
    bool select=true;
    MessageBox m(text,flag,&select);
    m.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-m.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-m.height())/2);
    m.exec();
    if(flag)return select;
    else return false;
}
