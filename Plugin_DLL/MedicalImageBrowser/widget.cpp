#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 初始化变量
    isLoad=false;
    isLoadShow=false;
    showWidget=new ShowWidget;
    ui->scrollArea_2->setWidget(showWidget);
    connect(showWidget,&ShowWidget::positionChanged,this,[=](QPoint oldP,QPoint newP){
        int xChanged=newP.x()-oldP.x(),yChanged=newP.y()-oldP.y();
        ui->scrollArea_2->horizontalScrollBar()->setValue(ui->scrollArea_2->horizontalScrollBar()->value()-xChanged);
        ui->scrollArea_2->verticalScrollBar()->setValue(ui->scrollArea_2->verticalScrollBar()->value()-yChanged);
    });
    // 开启子线程
    this->tools=new ImageProcess;
    this->sub = new QThread;// 创建子线程
    this->tools->moveToThread(sub);
    this->sub->start();
    // 连接信号槽
    connect(ui->loadButton_2,&QToolButton::clicked,this,&Widget::loadImage);
    connect(ui->saveButton_2,&QToolButton::clicked,this,&Widget::saveImage);
    connect(ui->undoButton,&QToolButton::clicked,this,[=](){
        if(!isLoad)QMessageBox::critical(this,"错误","未导入任何图像");
        else{
            this->targetImage.copy(&(this->sourceImage));
            this->grawWindow();
        }
    });
    connect(ui->windowButton,&QToolButton::clicked,this,&Widget::grawWindow);
    connect(ui->grayreverseButton,&QToolButton::clicked,this,&Widget::grayReverse);
    connect(ui->dirreverseButton,&QToolButton::clicked,this,&Widget::reverse);
    connect(ui->showreverseButton,&QToolButton::clicked,this,&Widget::grayShowReverse);
    connect(ui->scaleButton_2,&QToolButton::clicked,this,&Widget::scale);
    connect(ui->enhanceButton,&QToolButton::clicked,this,&Widget::enhance);
    // 图像处理
    connect(this,&Widget::startGrayWindow,tools,&ImageProcess::grayWindow);
    connect(this,&Widget::startGrayReverse,tools,&ImageProcess::grayReverse);
    connect(this,&Widget::startReverse,tools,&ImageProcess::reverse);
    connect(this,&Widget::startGrayShowReverse,tools,&ImageProcess::grayShowReverse);
    connect(this,&Widget::startScale,tools,&ImageProcess::scale);
    connect(this,&Widget::startEnhance,tools,&ImageProcess::enhance);
    // 处理完成
    connect(tools,&ImageProcess::endGrayWindow,this,&Widget::draw);
    connect(tools,&ImageProcess::endGrayReverse,this,&Widget::grawWindow);
    connect(tools,&ImageProcess::endReverse,this,&Widget::draw);
    connect(tools,&ImageProcess::endGrayShowReverse,this,&Widget::draw);
    connect(tools,&ImageProcess::endScale,this,&Widget::draw);
    connect(tools,&ImageProcess::endEnhance,this,[=](){
        QMessageBox::information(this,"提示","增强完成");
        this->grawWindow();
    });
}

Widget::~Widget()
{
    delete ui;
    showWidget->deleteLater();
    sub->quit();
    sub->wait();
    sub->deleteLater();
    tools->deleteLater();
}
// 工具函数
void Widget::draw(){
    isLoadShow=true;
    this->showWidget->draw(this->showImage);
}
void Widget::loadImage(){
    // 点击后打开文件对话框
    QString fileName = QFileDialog::getOpenFileName(this,tr("选择图像"),"data/MedicalImageBrowser",tr("自定义图像(*raw);;标准图像(*bmp)"));// 只能选择一些指定的文件类型
    if(fileName!=""){// 只有当输入的文件路径不为空时，才进行下一步
        QString type=fileName.mid(fileName.size()-3,3);
        if(type=="bmp"){
            // 将导入的图像存储下来，并绘制在原图像窗口
            this->showImage.load(fileName);// 加载图像
            QVector<QRgb> grayTable;
            for(int i=0;i<256;i++)grayTable.push_back(qRgb(i,i,i));
            showImage.setColorTable(grayTable);
            ui->label_9->setText("已导入   宽*高："+QString::number(this->showImage.width())+"*"+QString::number(this->showImage.height()));
            ui->widthSpinBox->setDisabled(true);
            ui->positionSpinBox->setDisabled(true);
            ui->sharkSpinBox->setDisabled(true);
            ui->reduceSpinBox->setDisabled(true);
            ui->enhanceButton->setDisabled(true);
            ui->windowButton->setDisabled(true);
            ui->grayreverseButton->setDisabled(true);
            isLoadShow=true;
            this->draw();
        }else if(type=="raw"){
            // 将导入的图像存储下来，并绘制在原图像窗口
            this->sourceImage.load(fileName);// 加载图像
            this->targetImage.copy(&(this->sourceImage));
            ui->label_9->setText("已导入   宽*高："+QString::number(this->sourceImage.width())+"*"+QString::number(this->sourceImage.height()));
            this->isLoad=true;
        }
    }
}
void Widget::saveImage(){
    if(!this->isLoad||!this->isLoadShow)QMessageBox::critical(this,"错误","未导入任何图像");
    else {// 导出图像
        QString fileName=QFileDialog::getSaveFileName(this,tr("保存图像"),"",tr("自定义图像(*raw);;标准图像(*bmp)"));
        if(fileName!=""){
            QString type=fileName.mid(fileName.size()-3,3);
            if(type=="bmp"){
                this->showImage.save(fileName);
            }else if(type=="raw"){
                this->targetImage.save(fileName);
            }
        }
    }
}
void Widget::grawWindow(){
    if(!this->isLoad)QMessageBox::critical(this,"错误","未导入任何图像");
    else {
        const unsigned short maxPixelV=4095;
        int w=ui->widthSpinBox->value();
        int p=ui->positionSpinBox->value();
        // 计算开始位置和结束位置
        int start=p-(w-1)/2;
        int end=p+(w-1)/2+(w%2==0?1:0);
        qRegisterMetaType<MyImage>("MyImage");
        if(start>=0&&end<=maxPixelV)this->startGrayWindow(start,end,&(this->targetImage),&(this->showImage));
        else QMessageBox::critical(this,"错误","灰度窗设置错误");
    }
}
void Widget::grayReverse(){
    if(!this->isLoad)QMessageBox::critical(this,"错误","未导入任何图像");
    else {
        qRegisterMetaType<MyImage>("MyImage");
        this->startGrayReverse(&(this->targetImage));
    }
}
void Widget::reverse(){
    if(!this->isLoadShow)QMessageBox::critical(this,"错误","未产生展示图像");
    else {
        this->startReverse(&(this->showImage));
    }
}
void Widget::grayShowReverse(){
    if(!this->isLoadShow)QMessageBox::critical(this,"错误","未产生展示图像");
    else {
        this->startGrayShowReverse(&(this->showImage));
    }
}
void Widget::scale(){
    if(!this->isLoadShow)QMessageBox::critical(this,"错误","未产生展示图像");
    else {
        if(ui->scaleSpinBox->value()>0){
            this->startScale(ui->scaleSpinBox->value(),&(this->showImage));
        }
    }
}
void Widget::enhance(){
    if(!this->isLoad)QMessageBox::critical(this,"错误","未导入任何图像");
    else {
        this->startEnhance(ui->sharkSpinBox->value(),ui->reduceSpinBox->value(),&(this->targetImage));
    }
}

