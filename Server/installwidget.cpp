#include "installwidget.h"
#include "ui_installwidget.h"

InstallWidget::InstallWidget(bool flag,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InstallWidget),
    flag(flag)
{
    ui->setupUi(this);
    if(!flag)this->setFixedHeight(200);
    isSuccess=false;
    this->setAcceptDrops(true);// 接受拖动事件
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::Tool);// 去除标题栏
    if(flag)this->clear("请拖入插件包");
    else this->clear("请拖入TBox更新包");
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 禁止鼠标选中
    connect(ui->cancelButton,&QPushButton::clicked,this,&InstallWidget::close);
    if(flag)connect(ui->installButton,&QPushButton::clicked,this,&InstallWidget::installPlugin);
    else connect(ui->installButton,&QPushButton::clicked,this,&InstallWidget::updateTBox);
}
void InstallWidget::dragEnterEvent(QDragEnterEvent *event){
    if(event->mimeData()->hasUrls())event->acceptProposedAction();
    else event->ignore();// 拒绝其他事件
}
void InstallWidget::dropEvent(QDropEvent *event){
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls()){
        this->clear("");
        QList<QUrl> urls = mimeData->urls();
        if(flag){
            if(urls.size()==1){// 拖入项目只有一个时
                QFileInfo fileInfo(urls.at(0).toLocalFile());
                if(fileInfo.isDir()){// 拖入项目是为文件夹时
                    QDir dir(urls.at(0).toLocalFile());
                    QFileInfoList fileList = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
                    if(fileList.size()==2)this->checkFile(fileList.at(0).absoluteFilePath(),fileList.at(1).absoluteFilePath());// 检查包是否包含两个项目
                    else this->clear("请拖入正确的插件包！");
                }else if(fileInfo.isFile()&&fileInfo.suffix()=="dll")this->showPlugin(fileInfo.absoluteFilePath(),"");// 展示插件信息
                else this->clear("请拖入正确的插件包！");
            }else if(urls.size()==2)this->checkFile(urls.at(0).toLocalFile(),urls.at(1).toLocalFile());// 拖入项目有两个时
            else this->clear("请拖入正确的插件包！");
        }else{
            if(urls.size()==1){// 拖入项目只有一个时
                QFileInfo fileInfo(urls.at(0).toLocalFile());
                if(fileInfo.isDir()){// 拖入项目是为文件夹时
                    QDir dir(urls.at(0).toLocalFile());
                    QFileInfoList fileList = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
                    bool isAbout=false,isVersion=false,isExe=false,isInterface=false,isSettings=false;
                    foreach (QFileInfo fileInfo, fileList) {
                        if(fileInfo.fileName()=="about.txt")isAbout=true;
                        else if(fileInfo.fileName()=="version.ini")isVersion=true;
                        else if(fileInfo.fileName()=="TBox.exe")isExe=true;
                        else if(fileInfo.fileName()=="PluginInterface.h")isInterface=true;
                        else if(fileInfo.fileName()=="settings.ini")isSettings=true;
                    }
                    if(!isExe)ui->textEdit->append("缺少[TBox.exe]");
                    if(!isAbout)ui->textEdit->append("缺少[about.txt]");
                    if(!isVersion)ui->textEdit->append("缺少[version.ini]");
                    if(!isInterface)ui->textEdit->append("缺少[PluginInterface.h]");
                    if(!isSettings)ui->textEdit->append("缺少[settings.ini]");
                    ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->minimum());// 滚动条置顶
                    if(isExe&&isAbout&&isVersion&&isInterface&&isSettings)this->showTBox(urls.at(0).toLocalFile());
                }else this->clear("请拖入正确的更新包！");
            }else this->clear("请拖入正确的更新包！");
        }
    }
}
void InstallWidget::showPlugin(QString dllPath, QString configPath){
    // 读取并展示插件信息
    QPluginLoader *loader = new QPluginLoader(dllPath,this);
    if(loader->load()){
        PluginInterface *plugin = qobject_cast<PluginInterface *>(loader->instance());
        if(!plugin){// 如果转换失败
            delete loader;loader = nullptr;
            this->clear("插件不符合规范！\n请检查是否存在以下问题：\n（1）插件未实现规范的API\n（2）插件版本太低，请及时更新！");
        }else{
            QJsonObject jsonObject=loader->metaData();
            pixmap=plugin->getIcon();
            if(loader->unload()){// 卸载插件
                delete loader;loader=nullptr;
            }
            IID=jsonObject.value("IID").toString();
            data=jsonObject.value("MetaData").toObject();
            ui->textEdit->clear();
            ui->textEdit->append("一、插件参数：");
            ui->textEdit->append("（1）插件ID："+IID);
            this->append(data,2);
            ui->textEdit->append("二、配置文件列表：");
            this->getFileNamesFromDir(configPath);
            ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->minimum());// 滚动条置顶
            isSuccess=true;this->dllPath=dllPath;this->configPath=configPath;
        }
    }else{
        delete loader;loader = nullptr;
        this->clear("插件加载失败！\n请检查是否存在以下问题：\n（1）插件路径错误\n（2）文件已损坏\n（3）插件发行类型与TBox不一致");
    }
}
void InstallWidget::getFileNamesFromDir(QString source){
    QStringList filePaths;
    QDir dir(source);
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    if(Filelist.size()==0||source=="")ui->textEdit->append("无配置文件");
    else{
        int counter=1;
        for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++,counter++){
            if(iter->isFile())ui->textEdit->append("（"+QString::number(counter)+"）"+iter->fileName());
            else if(iter->isDir())getFileNamesFromDir(iter->absoluteFilePath());
        }
    }
}
void InstallWidget::installPlugin(){
    if(isSuccess&&ui->installButton->text()=="提交"){
        if(QFile::exists("json/"+IID+".json")){// 该插件已经存在
            QFile file("json/"+IID+".json");
            file.open(QIODevice::ReadOnly);
            QJsonDocument document = QJsonDocument::fromJson(file.readAll());
            file.close();
            if (!document.isNull()){// 正确解析
                QJsonObject object = document.object();
                ui->textEdit->clear();
                ui->textEdit->append("之前已提交过该插件的以下版本：");
                this->append(object,1);
                ui->textEdit->append("是否进行版本更新？");
                ui->textEdit->verticalScrollBar()->setValue(ui->textEdit->verticalScrollBar()->minimum());// 滚动条置顶
                ui->installButton->setText("更新");
            }
        }else{
            newPlugin();
            this->clear("");
            this->addItem(IID,data);
            this->close();
        }
    }else if(isSuccess&&ui->installButton->text()=="更新"){
        QFile::remove("json/"+IID+".json");// 移除记录文件
        QFile::remove("json/"+IID+".png");// 移除图标
        QFile::remove("plugins/"+IID+".dll");// 移除动态链接库
        deleteDir("data/"+IID);// 移除文件夹
        newPlugin();
        this->clear("");
        this->updateItem(IID,data);
        this->close();
    }
}
void InstallWidget::newPlugin(){
    // 拷贝dll文件
    QFile::copy(dllPath,"plugins/"+IID+".dll");
    // 拷贝文件夹
    QDir dir;
    dir.mkdir("data/"+IID);
    if(configPath!=""){
        copyDir(configPath,"data/"+IID);
    }
    // 新建JSON文件
    QFile file("json/"+IID+".json");
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(this->data).toJson());
    file.close();
    // 新建icon
    pixmap.save("json/"+IID+".png");
}
void InstallWidget::deleteDir(QString source){
    QDir dir(source);
    if(dir.exists()){
        QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
        for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
            if(iter->isFile())QFile::remove(iter->absoluteFilePath());
            else if(iter->isDir())deleteDir(iter->absoluteFilePath());
        }
        dir.rmdir(dir.absolutePath());
    }
}
void InstallWidget::copyDir(QString source, QString target){
    QDir dir(source);
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        if(iter->isFile())QFile::copy(iter->absoluteFilePath(),target+'/'+iter->fileName());// 复制文件
        else if(iter->isDir()){
            QDir d;
            d.mkdir(target+'/'+iter->fileName());
            copyDir(iter->absoluteFilePath(),target+'/'+iter->fileName());
        }
    }
}
void InstallWidget::showTBox(QString path){
    QSettings s(path+"/version.ini",QSettings::IniFormat);
    s.beginGroup("About");
    ui->textEdit->clear();
    ui->textEdit->append("TBox版本信息如下：");
    ui->textEdit->append(s.value("time").toString()+" v"+s.value("version").toString()+
                         (s.value("type").toString()=="0"?" 测试版":" 正式版"));
    s.endGroup();
    isSuccess=true;this->path=path;
}
void InstallWidget::updateTBox(){
    if(isSuccess&&ui->installButton->text()=="提交"){
        ui->textEdit->append("是否提交TBox更新包？");
        ui->installButton->setText("更新");
    }else if(isSuccess&&ui->installButton->text()=="更新"){
        QFile::remove("tbox/TBox.exe");
        QFile::remove("tbox/about.txt");
        QFile::remove("tbox/PluginInterface.h");
        QFile::remove("tbox/version.ini");
        QFile::remove("tbox/settings.ini");
        QFile::copy(path+"/TBox.exe","tbox/TBox.exe");
        QFile::copy(path+"/about.txt","tbox/about.txt");
        QFile::copy(path+"/PluginInterface.h","tbox/PluginInterface.h");
        QFile::copy(path+"/version.ini","tbox/version.ini");
        QFile::copy(path+"/settings.ini","tbox/settings.ini");
        this->commit();
        this->close();
    }
}
InstallWidget::~InstallWidget()
{
    delete ui;
}
void InstallWidget::clear(QString text){
    ui->textEdit->setText(text);
    ui->installButton->setText("提交");
    isSuccess=false;
}
void InstallWidget::append(QJsonObject object,int start){
    ui->textEdit->append("（"+QString::number(start)+"）插件名："+object.value("name").toString());
    ui->textEdit->append("（"+QString::number(start+1)+"）作者："+object.value("author").toString());
    ui->textEdit->append("（"+QString::number(start+2)+"）版本："+object.value("version").toString());
    ui->textEdit->append("（"+QString::number(start+3)+"）大小："+object.value("size").toString());
    ui->textEdit->append("（"+QString::number(start+4)+"）类别："+object.value("type").toString());
    ui->textEdit->append("（"+QString::number(start+5)+"）描述："+object.value("description").toString());
}
void InstallWidget::checkFile(QString path1, QString path2){
    bool isDll=false,isConfig=false;
    QString dllPath,configPath;
    QFileInfo fileInfo1(path1),fileInfo2(path2);
    if(fileInfo1.isFile()&&fileInfo1.suffix()=="dll"){isDll=true;dllPath=fileInfo1.absoluteFilePath();}
    else if(fileInfo1.isDir()){isConfig=true;configPath=fileInfo1.absoluteFilePath();}
    if(fileInfo2.isFile()&&fileInfo2.suffix()=="dll"){isDll=true;dllPath=fileInfo2.absoluteFilePath();}
    else if(fileInfo2.isDir()){isConfig=true;configPath=fileInfo2.absoluteFilePath();}
    if(isDll&&isConfig){// 检查包内容是否为dll+配置文件
        this->showPlugin(dllPath,configPath);// 展示插件信息
    }else this->clear("请拖入正确的插件包！");
}
