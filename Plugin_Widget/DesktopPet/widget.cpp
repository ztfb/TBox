#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    HOOK::install();// 安装钩子
    connect(HOOK::instance(), &HOOK::mouseEvent, this, &Widget::mouseEvent);
    Pet *pet=new Pet;
    pet->setAttribute(Qt::WA_DeleteOnClose);
    pets.push_back(pet);
    toy=new Toy;
    toy->setAttribute(Qt::WA_DeleteOnClose);
    connect(toy,&Toy::currentPosition,this,&Widget::currentPosition);
    door1=new Door;door2=new Door;
    door1->setAttribute(Qt::WA_DeleteOnClose);door2->setAttribute(Qt::WA_DeleteOnClose);
    connect(door1,&Door::currentPosition,this,&Widget::currentPositionDoor1);
    connect(door2,&Door::currentPosition,this,&Widget::currentPositionDoor2);
    this->basicStyle();
}
void Widget::mouseEvent(QMouseEvent *event){
    if(event->type()==QMouseEvent::MouseButtonPress){
        QString but="";
        if(event->button()==Qt::LeftButton)but = "左键";
        else if(event->button()==Qt::RightButton)but = "右键";
        but+="按下";
    }else if(event->type()==QMouseEvent::MouseButtonRelease){
        QString but="";
        if(event->button()==Qt::LeftButton)but = "左键";
        else if(event->button()==Qt::RightButton)but = "右键";
        but+="释放";
    }else if(event->type()==QMouseEvent::MouseMove){
        if(ui->modecomboBox->currentIndex()==3)this->cursorMove(event->x(),event->y());// 发送鼠标位置
    }
    delete event;
}
void Widget::closeEvent(QCloseEvent *event){
    foreach (Pet* pet, pets) {
        pet->close();
    }
    toy->close();
    door1->close();
    door2->close();
}
void Widget::showEvent(QShowEvent *event){
    static bool first=true;
    if(first){// 窗口展示后再进行设置，避免SwitchButton错误渲染
        this->settingsInit();
        first=false;
    }
}
void Widget::basicStyle(){
    ui->lineEdit->setPlaceholderText("提示框文本...");
    connect(ui->savebutton,&QPushButton::clicked,this,&Widget::settingsSave);// 保存设置
    connect(ui->helpButton,&QPushButton::clicked,this,[=](){// 帮助文档
        Help h;
        h.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-h.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-h.height())/2);
        h.exec();
    });
    connect(ui->switchButton,&SwitchButton::statusChanged,this,[=](bool cheched){// 开关按钮
        foreach (Pet* pet, pets) {
            if(cheched)pet->setisShow(true);
            else pet->setisShow(false);
        }
    });
    connect(ui->topButton,&SwitchButton::statusChanged,this,[=](bool cheched){// 置顶按钮
        foreach (Pet* pet, pets) {
            if(cheched)pet->setisTop(true);
            else pet->setisTop(false);
        }
    });
    connect(ui->sizeSlider,&QSlider::valueChanged,this,[=](int value){// 设置大小
        foreach (Pet* pet, pets) {
            pet->setSize(value);
        }
        toy->setSize(value);
        door1->setSize(value);
        door2->setSize(value);
    });
    connect(ui->opacitySlider,&QSlider::valueChanged,this,[=](int value){// 设置透明度
        foreach (Pet* pet, pets) {
            pet->setOpacity(value/100.0);
        }
        toy->setWindowOpacity(value/100.0);
        door1->setWindowOpacity(value/100.0);
        door2->setWindowOpacity(value/100.0);
    });
    connect(ui->speedSlider,&QSlider::valueChanged,this,[=](int value){// 调整速度
        foreach (Pet* pet, pets) {
            pet->setSpeed(value);
        }
    });
    void(QComboBox:: *indexChanged)(int)=&QComboBox::currentIndexChanged;
    connect(ui->switchcomboBox,indexChanged,this,[=](){
        for(int i=0;i<ui->seedcomboBox->count();i++)ui->seedcomboBox->setItemData(i,-1,Qt::UserRole-1);
        ui->seedcomboBox->setItemData(ui->switchcomboBox->currentIndex(),0,Qt::UserRole-1);
        foreach(Pet* pet,pets){
            pet->setKey(ui->switchcomboBox->currentIndex(),ui->seedcomboBox->currentIndex());
        }
    });
    connect(ui->seedcomboBox,indexChanged,this,[=](){
        for(int i=0;i<ui->switchcomboBox->count();i++)ui->switchcomboBox->setItemData(i,-1,Qt::UserRole-1);
        ui->switchcomboBox->setItemData(ui->seedcomboBox->currentIndex(),0,Qt::UserRole-1);
        foreach(Pet* pet,pets){
            pet->setKey(ui->switchcomboBox->currentIndex(),ui->seedcomboBox->currentIndex());
        }
    });
    connect(ui->delButton,&QPushButton::clicked,this,[=](){// 关闭pet
        if(pets.size()>1){
            Pet* pet=pets.last();
            pets.pop_back();
            pet->close();
        }
    });
    connect(ui->delButton_2,&QPushButton::clicked,this,[=](){// 关闭pet
        while(pets.size()>1){
            Pet* pet=pets.last();
            pets.pop_back();
            pet->close();
        }
    });
    connect(ui->newButton,&QPushButton::clicked,this,[=](){// 创建新的pet
        Pet *pet=new Pet;
        pet->setAttribute(Qt::WA_DeleteOnClose);
        this->petInit(pet);
        pets.push_back(pet);
    });
    connect(ui->showButton,&QPushButton::clicked,this,[=](){// 隐藏/显示对话框
        foreach (Pet* pet, pets) {
            if(ui->showButton->text()=="隐藏")pet->setisVisible(false);
            else if(ui->showButton->text()=="显示")pet->setisVisible(true);
        }
        if(ui->showButton->text()=="隐藏")ui->showButton->setText("显示");
        else if(ui->showButton->text()=="显示")ui->showButton->setText("隐藏");
    });
    connect(ui->colorButton,&QPushButton::clicked,this,[=](){// 改变对话框颜色
        QColor color=QColorDialog::getColor(Qt::white,this,"选择背景颜色");
        if(color.isValid()){
            foreach (Pet* pet, pets) {
                pet->setBackground(color);
            }
        }
    });
    connect(ui->objectcomboBox,indexChanged,this,[=](){// 道具选择
        toy->hide();door1->hide();door2->hide();
        if(ui->objectcomboBox->currentIndex()==1){
            toy->draw();toy->show();
        }else if(ui->objectcomboBox->currentIndex()==2){
            door1->show();door2->show();
        }
        this->objectSelect(ui->objectcomboBox->currentIndex());
    });
    connect(ui->modecomboBox,indexChanged,this,[=](){// 设置当前模式
        if(ui->modecomboBox->currentIndex()!=2){
            ui->objectcomboBox->setItemData(1,0,Qt::UserRole-1);
            ui->objectcomboBox->setItemData(2,0,Qt::UserRole-1);
            ui->objectcomboBox->setCurrentIndex(0);// 关闭道具
        }else{
            ui->objectcomboBox->setItemData(1,-1,Qt::UserRole-1);
            ui->objectcomboBox->setItemData(2,-1,Qt::UserRole-1);
        }
        foreach(Pet* pet,pets){
            pet->setMode(ui->modecomboBox->currentIndex());
        }
    });
}
void Widget::settingsInit(){
    QFile file("data/DesktopPet/settings.json");
    file.open(QIODevice::ReadOnly);
    QJsonDocument document=QJsonDocument::fromJson(file.readAll());
    file.close();
    if(!document.isNull()){
        QJsonObject object=document.object();
        ui->switchButton->setChecked((object.value("switch").toString()=="true"?true:false));
        ui->topButton->setChecked((object.value("topShow").toString()=="true"?true:false));
        ui->sizeSlider->setValue(object.value("size").toInt());
        ui->opacitySlider->setValue(object.value("opacity").toInt());
        ui->speedSlider->setValue(object.value("speed").toInt());
        ui->switchcomboBox->setCurrentIndex(object.value("switchKey").toInt());
        ui->seedcomboBox->setCurrentIndex(object.value("seedKey").toInt());
        ui->switchcomboBox->setItemData(ui->seedcomboBox->currentIndex(),0,Qt::UserRole-1);// 禁用
        ui->seedcomboBox->setItemData(ui->switchcomboBox->currentIndex(),0,Qt::UserRole-1);// 禁用
        ui->lineEdit->setText(object.value("text").toString());
        ui->showButton->setText((object.value("show").toString()=="true"?"隐藏":"显示"));
        ui->modecomboBox->setCurrentIndex(object.value("mode").toInt());
        if(!QFile::exists("data/DesktopPet/license.zf")){
            ui->modecomboBox->setItemData(4,0,Qt::UserRole-1);
            if(ui->modecomboBox->currentIndex()==4)ui->modecomboBox->setCurrentIndex(0);
        }
        ui->objectcomboBox->setCurrentIndex(object.value("object").toInt());
        if(ui->modecomboBox->currentIndex()!=2){
            ui->objectcomboBox->setItemData(1,0,Qt::UserRole-1);
            ui->objectcomboBox->setItemData(2,0,Qt::UserRole-1);
        }
        // 设置一些参数
        this->petInit(pets[0]);// 开始时只有一个窗口
        if(ui->objectcomboBox->currentIndex()==1)toy->show();
        else if(ui->objectcomboBox->currentIndex()==2){
            door1->show();door2->show();
        }
        toy->setSize(ui->sizeSlider->value());
        door1->setSize(ui->sizeSlider->value());
        door2->setSize(ui->sizeSlider->value());
        toy->setWindowOpacity(ui->opacitySlider->value()/100.0);
        door1->setWindowOpacity(ui->opacitySlider->value()/100.0);
        door2->setWindowOpacity(ui->opacitySlider->value()/100.0);
    }
}
void Widget::settingsSave(){
    if(this->message("确定保存？",true)){
        QJsonObject object;
        object.insert("switch",(ui->switchButton->isChecked()?"true":"false"));
        object.insert("topShow",(ui->topButton->isChecked()?"true":"false"));
        object.insert("size",ui->sizeSlider->value());
        object.insert("opacity",ui->opacitySlider->value());
        object.insert("speed",ui->speedSlider->value());
        object.insert("switchKey",ui->switchcomboBox->currentIndex());
        object.insert("seedKey",ui->seedcomboBox->currentIndex());
        object.insert("text",ui->lineEdit->text());
        object.insert("show",(ui->showButton->text()=="隐藏"?"true":"false"));
        object.insert("mode",ui->modecomboBox->currentIndex());
        object.insert("object",ui->objectcomboBox->currentIndex());
        QFile file("data/DesktopPet/settings.json");
        file.open(QIODevice::WriteOnly);
        file.write(QJsonDocument(object).toJson());
        file.close();
        this->message("保存成功",false);
    }
}
Widget::~Widget()
{
    delete ui;
    HOOK::uninstall();
}
void Widget::petInit(Pet *pet){
    // 检查是否显示pet
    if(ui->switchButton->isChecked())pet->setisShow(true);
    else pet->setisShow(false);
    // 检查pet是否置顶
    if(ui->topButton->isChecked())pet->setisTop(true);
    else pet->setisTop(false);
    // 调整大小
    pet->setSize(ui->sizeSlider->value());
    // 调整透明度
    pet->setOpacity(ui->opacitySlider->value()/100.0);
    // 调整速度
    pet->setSpeed(ui->speedSlider->value());
    // 设置快捷键
    pet->setKey(ui->switchcomboBox->currentIndex(),ui->seedcomboBox->currentIndex());
    // 设置对话框文本
    pet->settext(ui->lineEdit->text());
    connect(ui->lineEdit,&QLineEdit::textChanged,pet,&Pet::settext);// 改变对话框内容
    // 检查是否显示对话框
    if(ui->showButton->text()=="隐藏")pet->setisVisible(true);
    else if(ui->showButton->text()=="显示")pet->setisVisible(false);
    // 设置当前模式
    pet->setMode(ui->modecomboBox->currentIndex());
    connect(this,&Widget::cursorMove,pet,&Pet::traceCursor);
    // 传递道具选择
    connect(this,&Widget::objectSelect,pet,&Pet::objectSelect);
    connect(this,&Widget::currentPosition,pet,&Pet::traceToy);
    connect(this,&Widget::currentPositionDoor1,pet,&Pet::setDoor1);
    connect(this,&Widget::currentPositionDoor2,pet,&Pet::setDoor2);
    // 传递外观切换
    connect(pet,&Pet::appearanceChanged,this,[=](QString type){
        door1->draw(type);
        door2->draw(type);
    });
}
bool Widget::message(QString text,bool flag){// 封装好的对话框
    bool select=true;
    MessageBox m(text,flag,&select);
    m.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-m.width())/2,this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-m.height())/2);
    m.exec();
    if(flag)return select;
    else return false;
}
