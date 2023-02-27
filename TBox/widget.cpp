#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    isPress=false;// 初始无按压
    this->basicStyle();
    this->settingsInit();
    this->pluginInit(true);
}
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        position=event->globalPos();//获取鼠标绝对位置
        isPress=true;
    }
}
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::OpenHandCursor);
        setCursor(cursor);
        QPoint p=event->globalPos();//获取当前位置
        this->move(this->pos().x()+p.x()-position.x(),this->pos().y()+p.y()-position.y());//调整窗口位置
        position=p;//更新位置
    }
}
void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    QCursor cursor(Qt::ArrowCursor);
    setCursor(cursor);
    isPress=false;
}
void Widget::closeEvent(QCloseEvent *event){
    if(!isMenu&&ui->trayComboBox->currentIndex()==1){// 关闭事件来自窗口，且系统托盘打开
        event->ignore();
        this->hide();
    }
}
void Widget::systemTrayHandle(QSystemTrayIcon::ActivationReason reason){
    switch (reason) {
    case QSystemTrayIcon::Context:menu->show();break;
    case QSystemTrayIcon::DoubleClick:this->show();break;
    default:
        break;
    }
}
void Widget::basicStyle(){
    // 系统托盘
    systemTray = new QSystemTrayIcon(this);
    systemTray->setIcon(QIcon(":/resource/logo.png"));
    systemTray->setToolTip("TBox");
    //  托盘菜单
    menu = new QMenu(this);
    QAction* action1 = new QAction("关于",menu);
    QAction* action2 = new QAction("退出",menu);
    connect(action1,&QAction::triggered,this,[=](){
        systemTray->showMessage("关于","应用名：TBox\n开发者：twz\n一个集成了大量工具的箱子",QSystemTrayIcon::MessageIcon::Information,5000);
    });
    connect(action2,&QAction::triggered,this,[=](){
        isMenu=true;
        this->close();
    });
    menu->addAction(action1);
    menu->addAction(action2);
    systemTray->setContextMenu(menu);
    connect(systemTray,&QSystemTrayIcon::activated,this,&Widget::systemTrayHandle);
    // 控件初状态
    isMenu=false;
    this->setMinimumSize(this->size());// 设置最小尺寸
    ui->mainWidget->setCurrentIndex(1);// 默认在首页启动
    ui->stackedWidget->setCurrentIndex(0);
    ui->aboutTextEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
    ui->clearButton->setVisible(false);// 隐藏清除按钮
    ui->findLineEdit->setPlaceholderText("搜索插件...");// 设置提示文本
    ui->firstButton->setDisabled(true);// 禁用按钮
    // 控件初始化
    this->comboboxInit("style",ui->themeComboBox,true);
    this->comboboxInit("image/logo",ui->logoPictureComboBox,false);
    this->comboboxInit("image/mylogo",ui->myLogoComboBox,false);
    this->aboutInit();
    // 连接信号槽
    QHash<QString,int> indexs;
    indexs.insert("首页",0);indexs.insert("插件管理",1);indexs.insert("偏好设置",2);indexs.insert("关于TBox",3);
    QList<QPushButton*> buttons = ui->sideWidget->findChildren<QPushButton*>();
    foreach (QPushButton* btn, buttons) {// 按钮组
        connect(btn,&QPushButton::clicked,this,[=](){
            int index=indexs.value(btn->text());
            ui->stackedWidget->setCurrentIndex(index);
            foreach(QPushButton* temp, buttons)temp->setDisabled(false);
            btn->setDisabled(true);
        });
    }
    // 插件安装
    connect(ui->installWidget,&InstallWidget::addItem,this,[=](QString ID,QJsonObject object){
        this->addItem(ID,object);this->message("安装成功",false);
    });
    connect(ui->installWidget,&InstallWidget::updateItem,this,[=](QString ID,QJsonObject object){
        this->updateItem(ID,object);this->message("更新成功",false);
    });
    // 返回按钮
    connect(ui->returnButton,&QToolButton::clicked,this,[=](){
        ui->mainWidget->setCurrentIndex(1);
        QList<Tab*> tabs = ui->tabWidget->findChildren<Tab*>();
        foreach(Tab* tab, tabs)tab->setFlag(false);
    });
    // 屏幕适应选择
    void(QComboBox:: *indexChanged)(int)=&QComboBox::currentIndexChanged;
    connect(ui->screenComboBox,indexChanged,this,[=](){
        if(ui->screenComboBox->currentIndex()==0)ui->pluginWidget->widget()->setMinimumSize(initSize);
        else if(ui->screenComboBox->currentIndex()==1)ui->pluginWidget->widget()->setMinimumSize(0,0);
    });
    // 关闭所有插件
    connect(ui->closeButton,&QToolButton::clicked,this,&Widget::closeAllPlugins);
    // 搜索框
    connect(ui->findLineEdit,&QLineEdit::textChanged,this,&Widget::findPlugin);
    connect(ui->findButton,&QToolButton::clicked,this,&Widget::findPlugin);
    connect(ui->clearButton,&QToolButton::clicked,ui->findLineEdit,&QLineEdit::clear);
    // 主题切换
    connect(ui->themeComboBox,indexChanged,this,&Widget::setStyle);
    // 透明度
    connect(ui->windowOpacitySlider,&QSlider::valueChanged,this,[=](){
        this->setWindowOpacity(ui->windowOpacitySlider->value()/100.0);
    });
    // logo切换
    connect(ui->logoPictureComboBox,indexChanged,this,&Widget::logoSwitch);
    connect(ui->myLogoComboBox,indexChanged,this,&Widget::logoSwitch);
    // 下载接口
    connect(ui->downloadButton,&QPushButton::clicked,this,[=](){
        QString path=QFileDialog::getExistingDirectory(this,"选择保存位置");
        if(path!=""){
            QFile::copy("config/PluginInterface.h",path+"/PluginInterface.h");
            this->message("下载完成",false);
        }
    });
    connect(ui->versionUpdateButton,&QPushButton::clicked,this,[=](){// 启动外部更新程序
        if(this->message("确定启动更新程序？",true)){
            process=new QProcess;
            process->start("TBoxUpdate.exe");
            isMenu=true;this->close();// 关闭当前窗口
        }
    });
    // 系统托盘
    connect(ui->trayComboBox,indexChanged,this,[=](int index){
        if(index==0)systemTray->hide();
        else systemTray->show();
    });
    // logo特效
    connect(ui->speedSlider,&QSlider::valueChanged,this,[=](){
        ui->logoWidget->setAngle(ui->speedSlider->value());
        ui->logoWidget_2->setAngle(ui->speedSlider->value());
    });
    connect(ui->logoModeComboBox,indexChanged,this,[=](int index){
        ui->logoWidget->setMode(index);
        ui->logoWidget_2->setMode(index);
    });
    // 保存设置
    connect(ui->saveButton,&QToolButton::clicked,this,&Widget::settingsSave);
}
void Widget::settingsInit(){
    QSettings settings("config/settings.ini",QSettings::IniFormat);
    settings.beginGroup("Settings");
    ui->themeComboBox->setCurrentIndex(settings.value("theme").toInt());
    ui->windowOpacitySlider->setValue(settings.value("windowOpacity").toInt());
    ui->myLogoComboBox->setCurrentIndex(settings.value("myLogo").toInt());
    ui->trayComboBox->setCurrentIndex(settings.value("tray").toInt());
    ui->logoModeComboBox->setCurrentIndex(settings.value("logoMode").toInt());
    ui->logoPictureComboBox->setCurrentIndex(settings.value("logoPicture").toInt());
    ui->speedSlider->setValue(settings.value("speed").toInt());
    settings.endGroup();
    this->setStyle();
    this->setWindowOpacity(ui->windowOpacitySlider->value()/100.0);
    this->logoSwitch();
    if(ui->trayComboBox->currentIndex()==1)systemTray->show();
    ui->logoWidget->setMode(ui->logoModeComboBox->currentIndex());
    ui->logoWidget_2->setMode(ui->logoModeComboBox->currentIndex());
    ui->logoWidget->setAngle(ui->speedSlider->value());
    ui->logoWidget_2->setAngle(ui->speedSlider->value());
}
void Widget::settingsSave(){
    if(this->message("确定保存？",true)){// 点击了确定按钮
        QSettings settings("config/settings.ini",QSettings::IniFormat);
        settings.beginGroup("Settings");
        settings.setValue("theme",ui->themeComboBox->currentIndex());
        settings.setValue("windowOpacity",ui->windowOpacitySlider->value());
        settings.setValue("myLogo",ui->myLogoComboBox->currentIndex());
        settings.setValue("tray",ui->trayComboBox->currentIndex());
        settings.setValue("logoMode",ui->logoModeComboBox->currentIndex());
        settings.setValue("logoPicture",ui->logoPictureComboBox->currentIndex());
        settings.setValue("speed",ui->speedSlider->value());
        settings.endGroup();
        this->message("保存成功",false);
    }
}
void Widget::setStyle(){
    this->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/Widget.qss"));
    ui->installWidget->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/InstallWidget.qss"));
    QList<PluginItem*> ws=ui->listWidget->findChildren<PluginItem*>();
    foreach(PluginItem* w,ws)w->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/PluginItem.qss"));
    QList<Tab*> tabs = ui->tabWidget->findChildren<Tab*>();
    foreach(Tab* tab, tabs)tab->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/Tab.qss"));
}
QString Widget::getStyleSheet(QString fileName){// 读取样式表文件
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QString styleSheet=file.readAll();
    file.close();
    return styleSheet;
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
                bool isContain=false;
                switch (ui->findComboBox->currentIndex()) {
                case 0:if(object.value("name").toString().contains(ui->findLineEdit->text()))isContain=true;break;
                case 1:if(object.value("author").toString().contains(ui->findLineEdit->text()))isContain=true;break;
                case 2:if(object.value("type").toString().contains(ui->findLineEdit->text()))isContain=true;break;
                case 3:if(object.value("description").toString().contains(ui->findLineEdit->text()))isContain=true;break;
                }
                if(flag||isContain){
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
    w->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/PluginItem.qss"));
    connect(w,&PluginItem::run,this,&Widget::runPlugin);
    connect(w,&PluginItem::del,this,&Widget::delPlugin);
    QListWidgetItem *item = new QListWidgetItem;
    item->setFlags(item->flags() & ~Qt::ItemIsSelectable);
    item->setSizeHint(QSize(800,60));
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
        // 关闭相应的插件
        Tab* tab=tabs.value(ID);
        this->closePlugin(ID,tab);
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
    if(ui->findLineEdit->text()=="")ui->clearButton->setVisible(false);
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
void Widget::runPlugin(QString ID, QString name){
    if(tabs.constFind(ID)==tabs.constEnd()){// 之前没有打开该插件
        QPluginLoader* loader;
        bool result=false;
        if(loaders.constFind(ID)==loaders.constEnd()){// 之前没有加载过
            loader = new QPluginLoader("plugins/"+ID+".dll",this);// 加载插件
            if(loader->load()){
                PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
                if(!plugin){// 如果转换失败
                    delete loader;loader = nullptr;
                    this->message("插件不符合规范",false);
                }else{
                    // 创建连接
                    connect(loader->instance(),SIGNAL(sendMessage(QJsonObject)),this,SLOT(callfunc(QJsonObject)));
                    loaders.insert(ID,loader);
                    result=true;
                }
            }else this->message("插件加载失败",false);
        }else{// 之前已经加载过
            loader=loaders.value(ID);
            result=true;
        }
        if(result){
            // 创建标签
            PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
            Tab* tab=new Tab(name,ID,this);
            connect(tab,&Tab::closePlugin,this,&Widget::closePlugin);
            connect(tab,&Tab::selectPlugin,this,&Widget::selectPlugin);
            tab->setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/Tab.qss"));
            ui->tabshorizontalLayout->insertWidget(ui->tabshorizontalLayout->count()-1,tab);
            tabs.insert(ID,tab);
            // 切换页面
            initSize=plugin->getWidget()->size();
            plugin->getWidget()->setMinimumSize(initSize);
            for(int i=0;i<2;i++){// 循环两遍才能把样式表加载上
                ui->pluginWidget->takeWidget();
                ui->pluginWidget->setWidget(plugin->getWidget());
            }
            ui->screenComboBox->setCurrentIndex(0);// 默认显示原始尺寸
            ui->mainWidget->setCurrentIndex(0);
        }else this->message("插件加载失败",false);
    }else this->selectPlugin(ID,tabs.value(ID));// 如果已经打开了该插件，切换到相应的插件
}
void Widget::selectPlugin(QString ID,Tab* value){
    QList<Tab*> tabs = ui->tabWidget->findChildren<Tab*>();
    foreach(Tab* tab, tabs)tab->setFlag(false);
    value->setFlag(true);
    QPluginLoader* loader=loaders.value(ID);
    PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
    // 切换页面
    initSize=plugin->getWidget()->size();
    plugin->getWidget()->setMinimumSize(initSize);
    ui->pluginWidget->takeWidget();
    ui->pluginWidget->setWidget(plugin->getWidget());
    ui->screenComboBox->setCurrentIndex(0);// 默认显示原始尺寸
    ui->mainWidget->setCurrentIndex(0);
}
void Widget::closePlugin(QString ID, Tab *value){
    if(value!=nullptr){
        tabs.remove(ID);
        if(value->getFlag())ui->mainWidget->setCurrentIndex(1);// 只有关闭的是活跃窗口时，才跳转到首页
        delete value;
    }
}
void Widget::closeAllPlugins(){
    QList<Tab*> tabs = ui->tabWidget->findChildren<Tab*>();
    foreach(Tab* tab, tabs)delete tab;
    this->tabs.clear();
    ui->mainWidget->setCurrentIndex(1);
}
Widget::~Widget()
{
    delete ui;
    for(QHash<QListWidgetItem*,PluginItem*>::const_iterator iter = manageTable.constBegin();iter!=manageTable.constEnd();iter++){
        // 退出后析构创建的项
        delete iter.key();
        delete iter.value();
    }
    for(QHash<QString,QPluginLoader*>::const_iterator iter = loaders.constBegin();iter!=loaders.constEnd();iter++){
        // 退出后析构创建的项
        if(iter.value()!=nullptr&&iter.value()->unload())delete iter.value();// 卸载所有插件
    }
}
bool Widget::message(QString text,bool flag){
        bool select=true;
        MessageBox m(text,flag,&select);
        m.setStyleSheet(getStyleSheet("style/"+ui->themeComboBox->currentText()+"/MessageBox.qss"));
        m.setWindowOpacity(this->windowOpacity());
        m.move(this->x()+(this->width()-m.width())/2,this->y()+(this->height()-m.height())/2);
        m.exec();
        if(flag)return select;
        else return false;
}
void Widget::logoSwitch(){
    if(ui->myLogoComboBox->currentIndex()==0){
        ui->logoPictureComboBox->setDisabled(false);
        ui->logoWidget->setImage("image/logo/"+ui->logoPictureComboBox->currentText()+".png");
        ui->logoWidget_2->setImage("image/logo/"+ui->logoPictureComboBox->currentText()+".png");
    }else{
        ui->logoPictureComboBox->setDisabled(true);
        ui->logoWidget->setImage("image/mylogo/"+ui->myLogoComboBox->currentText()+".png");
        ui->logoWidget_2->setImage("image/mylogo/"+ui->myLogoComboBox->currentText()+".png");
    }
}
void Widget::comboboxInit(QString filePath, QComboBox *combobox, bool flag){
    QDir dir(filePath);
    QFileInfoList Filelist;
    if(flag){
        Filelist = dir.entryInfoList(QDir::Dirs|QDir::NoDotAndDotDot);
        for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++)combobox->addItem(iter->fileName());
    }
    else {
        Filelist = dir.entryInfoList(QDir::Files);
        for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++)combobox->addItem(iter->fileName().mid(0,iter->fileName().size()-4));
    }
}
void Widget::aboutInit(){
    QFile file("config/about.txt");
    file.open(QIODevice::ReadOnly);
    ui->aboutTextEdit->setText(file.readAll());
    file.close();
    QSettings settings("config/version.ini",QSettings::IniFormat);
    settings.beginGroup("About");
    ui->TBoxVersionLabel->setText("v"+settings.value("version").toString());
    ui->TBoxVersionLabel_2->setText(settings.value("time").toString()+" v"+settings.value("version").toString()+
                                    (settings.value("type").toString()=="0"?" 测试版":" 正式版"));
    settings.endGroup();
}
// 提供给插件调用的函数
void Widget::callfunc(QJsonObject object){
    QByteArray temp = object.value("func").toString().toLocal8Bit();
    char* t=temp.data();
    strcpy(this->funcname,t);
    QMetaObject::invokeMethod(this,this->funcname,Qt::DirectConnection,QGenericReturnArgument());
}
void Widget::flushPlugins(){
    this->findPlugin();// 重新查询
}
void Widget::flushCombobox(){
    ui->myLogoComboBox->clear();// 删除所有项
    ui->myLogoComboBox->addItem("关闭");
    this->comboboxInit("image/mylogo",ui->myLogoComboBox,false);
}
