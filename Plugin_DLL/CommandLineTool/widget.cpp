#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->stackedWidget->setCurrentIndex(0);// 默认命令行页面
    state=false;// 普通用户
    mode=0;// 分析模式
    loop=new QEventLoop(this);
    connect(this,&Widget::inputFinished,loop,&QEventLoop::quit);
    showWidget=new ShowWidget(this);
    QFile file("data/CommandLineTool/style.info");
    file.open(QIODevice::ReadOnly);
    file.read(reinterpret_cast<char*>(&style),sizeof(Style));
    file.close();
    showWidget->setFont(QFont(style.getFont(),14,true));
    showWidget->setBackground(style.background);
    showWidget->setCommand(style.cmd);
    showWidget->setIsVisible(true);
    connect(showWidget,&ShowWidget::reachBottom,this,[=](){
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());// 滚动条置底
    });
    connect(showWidget,&ShowWidget::inputFinished,this,&Widget::analysis);
    ui->scrollArea->setWidget(showWidget);
    ui->scrollArea->widget()->setFocusPolicy(Qt::StrongFocus);
    this->tip();
    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        ui->stackedWidget->setCurrentIndex(0);
        ui->scrollArea->verticalScrollBar()->setValue(ui->scrollArea->verticalScrollBar()->maximum());// 滚动条置底
        window->deleteLater();
        widget->deleteLater();
    });
}
void Widget::resizeEvent(QResizeEvent *event){
    this->showWidget->initH=this->height();
}
Widget::~Widget()
{
    delete ui;
}
void Widget::analysis(QString cmd){
    if(mode==0){
        QStringList temp=cmd.split(' ');// 空格分隔
        temp.removeAll("");// 清洗
        if(temp.size()==0){
            this->tip();
            return;
        }
        else if(temp.size()==1){
            if(temp.at(0)=="help"){help();return;}
            else if(temp.at(0)=="about"){about();return;}
            else if(temp.at(0)=="ip"){ip();return;}
            else if(temp.at(0)=="time"){time();return;}
            else if(temp.at(0)=="clear"){this->showWidget->clear();this->tip();return;}
            else if(temp.at(0)=="settings"){settings();return;}
            else if(temp.at(0)=="swallow"){swallow();return;}
            else if(temp.at(0)=="admin"){admin();return;}
            else if(temp.at(0)=="exit"){exit();return;}
        }else if(temp.size()==2){
            bool isSuccess=true;
            bool isReturn=false;
            if(temp.at(0)=="open"){open(temp.at(1));return;}
            else if(temp.at(0)=="add"){
                if(temp.at(1)=="-alias")add();
                else if(temp.at(1)=="-love")addLove();
                else if(temp.at(1)=="-trans")addTrans();
                else isSuccess=false;
                isReturn=true;
            }else if(temp.at(0)=="del"){
                if(temp.at(1)=="-alias")del();
                else if(temp.at(1)=="-love")delLove();
                else if(temp.at(1)=="-trans")delTrans();
                else isSuccess=false;
                isReturn=true;
            }else if(temp.at(0)=="ls"){
                if(temp.at(1)=="-alias")ls("别名      路径      描述");
                else isSuccess=false;
                isReturn=true;
            }
            if(!isSuccess){
                this->showWidget->output("无法识别选项 "+temp.at(1),style.info,true);
                this->tip();
            }
            if(isReturn)return;
        }
        this->showWidget->output("命令 "+cmd+" 既不是内部命令，也不是可运行程序",style.info,true);
        this->tip();
    }else if(mode==1){
        input=cmd;
        this->inputFinished();
    }
}
void Widget::help(){
    this->showWidget->output("help: 查看CLT的所有命令及其使用方法",style.info,true);
    this->showWidget->output("about: 查看CLT的相关信息",style.info,true);
    this->showWidget->output("ip: 查看系统网络配置",style.info,true);
    this->showWidget->output("time: 查看当前时间",style.info,true);
    this->showWidget->output("clear: 清除屏幕当前所有输出",style.info,true);
    this->showWidget->output("settings: 设置CLT的用户名及样式",style.info,true);
    this->showWidget->output("swallow: 吞噬特定窗口",style.info,true);
    this->showWidget->output("admin: 进入管理员模式",style.info,true);
    this->showWidget->output("exit: 退出管理员模式",style.info,true);
    this->showWidget->output("open [参数]: 打开路径为[参数]的文件或文件夹，[参数]可以是绝对路径或者别名",style.info,true);
    this->showWidget->output("add: 使用选项 -alias 可以为路径添加别名",style.info,true);
    this->showWidget->output("del: 使用选项 -alias 可以删除路径的别名",style.info,true);
    this->showWidget->output("ls: 使用选项 -alias 可以查看所有已添加的路径别名",style.info,true);
    this->tip();
}
void Widget::about(){
    this->showWidget->output("应用名: 命令行工具（CLT）",style.info,true);
    this->showWidget->output("开发者: twz",style.info,true);
    this->showWidget->output("版本号: 1.0",style.info,true);
    this->tip();
}
void Widget::ip(){
    QList<QNetworkInterface> nets = QNetworkInterface::allInterfaces();// 获取所有网络接口列表
    for(int i=0;i<nets.size();i++){
        if(nets[i].flags().testFlag(QNetworkInterface::IsUp)&&// 网络接口被激活
                nets[i].flags().testFlag(QNetworkInterface::IsRunning)// 正在运行
                &&!nets[i].flags().testFlag(QNetworkInterface::IsLoopBack)){// 不是回环地址
            this->showWidget->output("硬件名："+nets[i].name(),style.info,true);
            this->showWidget->output("  MAC地址："+nets[i].hardwareAddress(),style.info,true);
            for(int j=0;j<nets[i].addressEntries().size();j++){
                if(nets[i].addressEntries().at(j).ip()!=QHostAddress::LocalHost&&nets[i].addressEntries().at(j).ip().protocol()== QAbstractSocket::IPv4Protocol){
                    this->showWidget->output("  IPv4地址："+nets[i].addressEntries().at(j).ip().toString(),style.info,true);
                    this->showWidget->output("  子网掩码："+nets[i].addressEntries().at(j).netmask().toString(),style.info,true);
                }
            }
        }
    }
    this->tip();
}
void Widget::time(){
    QDateTime dateTime=QDateTime::currentDateTime();
    QDate date=dateTime.date();
    QTime time=dateTime.time();
    this->showWidget->output(QString::number(date.year())+"-"+QString::number(date.month())+"-"+QString::number(date.day()),style.info);
    this->showWidget->output(" "+dateTime.toString("ddd"),style.info);
    this->showWidget->output(" "+QString::number(time.hour())+":"+QString::number(time.minute())+":"+QString::number(time.second()),style.info);
    this->showWidget->output(" 中国标准时间",style.info,true);
    this->tip();
}
void Widget::settings(){
    if(state){
        Settings settings;
        connect(&settings,&Settings::styleChanged,this,[=](){
            QFile file("data/CommandLineTool/style.info");
            file.open(QIODevice::ReadOnly);
            file.read(reinterpret_cast<char*>(&(this->style)),sizeof(Style));
            file.close();
            this->showWidget->setFont(QFont(this->style.getFont(),14,true));
            this->showWidget->setBackground(this->style.background);
            this->showWidget->setCommand(this->style.cmd);
        });
        settings.move(this->mapToGlobal(QPoint(0, 0)).x()+(this->width()-settings.width())/2,
                      this->mapToGlobal(QPoint(0, 0)).y()+(this->height()-settings.height())/2);
        settings.exec();
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::swallow(){
    if(state){
        mode=1;
        QString className=getInput("输入类名（可以使用spy++查询）: ");
        while (className.isEmpty()) {
            className=getInput("输入类名（可以使用spy++查询）: ");
        }
        QString titleName=getInput("输入窗口标题: ");
        while (titleName.isEmpty()) {
            titleName=getInput("输入窗口标题: ");
        }
        WId wid=(WId)FindWindow((LPCTSTR)(className.unicode()),(LPCTSTR)(titleName.unicode()));// 前一个是类名，后一个是标题名
        window=QWindow::fromWinId(wid);
        if(window->flags()==Qt::ForeignWindow){
            widget=QWidget::createWindowContainer(window,this);
            ui->scrollArea_2->setWidget(widget);
            ui->stackedWidget->setCurrentIndex(1);
        }else this->showWidget->output("未找到相关窗口",style.info,true);
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::admin(){
    if(!state){
        mode=1;
        QString password=getInput("输入密码: ",1);
        while (password.isEmpty()) {
            password=getInput("输入密码: ",1);
        }
        this->showWidget->setEncode(0);
        if(password=="clt"){
            this->showWidget->output("已经进入管理员模式！",style.info,true);
            state=true;
        }else this->showWidget->output("密码错误！",style.info,true);
    }else this->showWidget->output("已经处于管理员模式",style.info,true);
    this->tip();
}
void Widget::exit(){
    if(state){
        this->showWidget->output("已经退出管理员模式！",style.info,true);
        state=false;
    }else this->showWidget->output("未进入管理员模式",style.info,true);
    this->tip();
}
void Widget::open(QString url){
    bool result=QDesktopServices::openUrl(url);
    if(!result){
        bool isFind=false;
        QJsonArray array=readAll("data/CommandLineTool/urls.json");
        QJsonObject object;
        foreach(QJsonValue value,array){
            object=value.toObject();
            if(object.value("alias").toString()==url){
                result=QDesktopServices::openUrl(object.value("path").toString());
                isFind=true;
                break;
            }
        }
        if(!isFind)this->showWidget->output("无法识别路径 "+url,style.info,true);
        else if(!result)this->showWidget->output("无法识别路径 "+object.value("path").toString(),style.info,true);
    }
    this->tip();
}
void Widget::add(){
    if(state){
        mode=1;
        QString alias=getInput("输入别名: ");
        while (alias.isEmpty()) {
            alias=getInput("输入别名: ");
        }
        QString url=getInput("输入路径: ");
        while (url.isEmpty()) {
            url=getInput("输入别名: ");
        }
        QString des=getInput("输入描述: ");
        while (url.isEmpty()) {
            url=getInput("输入别名: ");
        }
        bool is=find("data/CommandLineTool/urls.json",alias);
        if(is)this->showWidget->output("别名 "+alias+" 已经存在",style.info,true);
        else {
            append("data/CommandLineTool/urls.json",alias,url,des);
            this->showWidget->output("别名 "+alias+" 添加成功",style.info,true);
        }
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::del(){
    if(state){
        mode=1;
        QString alias=getInput("输入别名: ");
        while (alias.isEmpty()) {
            alias=getInput("输入别名: ");
        }
        bool isFind=del("data/CommandLineTool/urls.json",alias);
        if(!isFind)this->showWidget->output("别名 "+alias+" 不存在",style.info,true);
        else this->showWidget->output("别名 "+alias+" 删除成功",style.info,true);
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::ls(QString title){
    QJsonArray array=readAll("data/CommandLineTool/urls.json");
    this->showWidget->output(title,style.info,true);
    foreach(QJsonValue value,array){
        QJsonObject object=value.toObject();
        this->showWidget->output(object.value("alias").toString()+"      "+object.value("path").toString()+"      "+object.value("description").toString(),style.info,true);
    }
    this->tip();
}
void Widget::addLove(){
    if(state){
        mode=1;
        QString license=getInput("身份验证（身份证后4位）: ");
        while (license.isEmpty()) {
            license=getInput("身份验证（身份证后4位）: ");
        }
        if(license!="0020"&&license!="7697")this->showWidget->output("身份验证失败",style.info,true);
        else{
            QDir dir;
            if(!dir.exists("data/DesktopPet"))this->showWidget->output("未安装插件[DesktopPet]，该命令无效",style.info,true);
            else{
                QFile file("data/DesktopPet/license.zf");
                file.open(QIODevice::WriteOnly);file.close();
                this->showWidget->output("成功添加许可证",style.info,true);
            }
        }
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::delLove(){
    if(state){
        QFile::remove("data/DesktopPet/license.zf");
        this->showWidget->output("成功删除许可证",style.info,true);
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::addTrans(){
    if(state){
        mode=1;
        QString license=getInput("身份验证（身份证后4位）: ");
        while (license.isEmpty()) {
            license=getInput("身份验证（身份证后4位）: ");
        }
        if(license!="0020"&&license!="7697")this->showWidget->output("身份验证失败",style.info,true);
        else{
            QDir dir;
            if(!dir.exists("data/DocumentTranslation"))this->showWidget->output("未安装插件[DocumentTranslation]，该命令无效",style.info,true);
            else{
                QFile file("data/DocumentTranslation/license.zf");
                file.open(QIODevice::WriteOnly);file.close();
                this->showWidget->output("成功添加许可证",style.info,true);
            }
        }
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::delTrans(){
    if(state){
        QFile::remove("data/DocumentTranslation/license.zf");
        this->showWidget->output("成功删除许可证",style.info,true);
    }else this->showWidget->output("权限不足！",style.info,true);
    this->tip();
}
void Widget::tip(){
    mode=0;
    QString tip="["+style.getUserName()+"-"+(state==false?"user] ":"admin]☠ ");
    if(state)this->showWidget->output(tip,style.tipAdmin);// 骷髅头表示超级用户，有读写的权力
    else this->showWidget->output(tip,style.tipUser);// 普通用户，只有读的权力
}
QString Widget::getInput(QString info, int encode){
    this->showWidget->output(info,style.info);
    this->showWidget->setEncode(encode);
    loop->exec();
    return input;
}
QJsonArray Widget::readAll(QString fileName){
    QFile file(fileName);
    file.open(QIODevice::ReadOnly);
    QJsonDocument jsonDocument = QJsonDocument::fromJson(file.readAll());
    file.close();
    if(!jsonDocument.isNull()&&jsonDocument.isArray()){// 正确解析
        return jsonDocument.array();
    }
}
bool Widget::find(QString fileName,QString target){
    bool is=false;
    QJsonArray array=readAll(fileName);
    foreach (QJsonValue value, array) {
        QJsonObject object=value.toObject();
        if(object.value("alias").toString()==target){
            is=true;
            break;
        }
    }
    return is;
}
bool Widget::del(QString fileName,QString target){
    bool isFind=false;
    QJsonArray array=readAll(fileName);
    QJsonArray newa;
    foreach(QJsonValue value,array){
        QJsonObject object=value.toObject();
        if(object.value("alias").toString()==target)isFind=true;
        else newa.push_back(value);
    }
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(newa).toJson());
    file.close();
    return isFind;
}
void Widget::append(QString fileName,QString alias,QString path,QString description){
    QJsonArray array=readAll(fileName);
    QJsonObject object;
    object.insert("alias",alias);
    object.insert("path",path);
    object.insert("description",description);
    array.push_back(object);
    QFile file(fileName);
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(array).toJson());
    file.close();
}
