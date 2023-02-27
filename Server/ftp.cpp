#include "ftp.h"

Ftp::Ftp(qintptr socket,QObject *parent) : QObject(parent)
{
    lock=new QSemaphore(1);// 创建互斥锁
    tcpSocket=new QTcpSocket(this);// 子线程创建套接字
    tcpSocket->setSocketDescriptor(socket);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&Ftp::clear);
    void(QTcpSocket:: *Error)(QAbstractSocket::SocketError)=&QTcpSocket::error;
    connect(tcpSocket,Error,this,&Ftp::clear);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Ftp::readData);

}
void Ftp::readData(){
    lock->acquire(1);
    cache.append(tcpSocket->readAll());
    // 解析cache
    int revlen=0;
    while (true) {
        if(cache.size()<revlen+headLen()){// 数据不足一个包头
            cache.remove(0,revlen);
            break;
        }
        else{
            QByteArray head=cache.mid(revlen,headLen());// 读包头
            int len=getLen(head);
            if(len>cache.size()-revlen){// 包不全
                cache.remove(0,revlen);
                break;
            }else{
                int action=getAction(head);
                revlen+=headLen();
                if(action==0)this->sendPlugin(cache.mid(revlen,len));
                else if(action==1)this->sendFile(cache.mid(revlen,len));
                else if(action==2)this->sendVersion();
                else if(action==3)this->sendTBox();
                revlen+=len;
                cache.remove(0,revlen);
            }
        }
    }
    lock->release(1);
}
void Ftp::sendPlugin(QByteArray request){
    this->log("查询插件");
    QString name(request);
    QJsonArray array;
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
                if(object.value("name").toString().contains(name)){
                    QString ID=iter->fileName().mid(0,iter->fileName().size()-5);
                    object.insert("ID",ID);
                    array.append(object);
                }
            }
        }
    }
    QByteArray data(QJsonDocument(array).toJson());
    data.insert(0,getHead(data.size(),0));
    tcpSocket->write(data);
}
void Ftp::sendFile(QByteArray request){
    this->log("下载插件");
    QString ID(request);
    // 发送插件文件信息
    QJsonArray array;
    QJsonObject object;
    QFileInfo fileInfo("plugins/"+ID+".dll");
    object.insert("ID",ID);
    object.insert("name",fileInfo.fileName());
    object.insert("size",fileInfo.size());
    array.append(object);
    QDir dir("data/"+ID);
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        object.insert("ID",ID);
        object.insert("name",iter->fileName());
        object.insert("size",iter->size());
        array.append(object);
    }
    QByteArray data(QJsonDocument(array).toJson());
    data.insert(0,getHead(data.size(),1));
    tcpSocket->write(data);
    // 发送文件
    QFile* file=new QFile;
    QTimer* timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        if(!file->atEnd()){
            QByteArray data(file->read(8192));
            data.insert(0,getHead(data.size(),2));
            tcpSocket->write(data);
        }else{
            timer->stop();
            file->close();
            this->quit();
        }
    });
    QEventLoop* loop=new QEventLoop(this);
    connect(this,&Ftp::quit,loop,&QEventLoop::quit);
    // 发送dll文件
    file->setFileName("plugins/"+ID+".dll");
    file->open(QIODevice::ReadOnly);
    timer->start(10);
    loop->exec();
    // 发送配置文件
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        file->setFileName("data/"+ID+"/"+iter->fileName());
        file->open(QIODevice::ReadOnly);
        timer->start(10);
        loop->exec();
    }
    timer->deleteLater();
    file->deleteLater();
    loop->deleteLater();
}
void Ftp::sendVersion(){
    this->log("检查更新");
    QSettings s("tbox/version.ini",QSettings::IniFormat);
    s.beginGroup("About");
    QJsonObject object;
    object.insert("time",s.value("time").toString());
    object.insert("version",s.value("version").toString());
    object.insert("type",s.value("type").toString());
    s.endGroup();
    QByteArray data(QJsonDocument(object).toJson());
    data.insert(0,getHead(data.size(),3));
    tcpSocket->write(data);
}
void Ftp::sendTBox(){
    this->log("下载更新包");
    // 发送更新包文件信息
    QJsonArray array;
    QJsonObject object;
    QDir dir("tbox");
    QFileInfoList Filelist = dir.entryInfoList(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        object.insert("name",iter->fileName());
        object.insert("size",iter->size());
        array.append(object);
    }
    QByteArray data(QJsonDocument(array).toJson());
    data.insert(0,getHead(data.size(),4));
    tcpSocket->write(data);
    // 发送文件
    QFile* file=new QFile;
    QTimer* timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        if(!file->atEnd()){
            QByteArray data(file->read(8192));
            data.insert(0,getHead(data.size(),5));
            tcpSocket->write(data);
        }else{
            timer->stop();
            file->close();
            this->quit();
        }
    });
    QEventLoop* loop=new QEventLoop(this);
    connect(this,&Ftp::quit,loop,&QEventLoop::quit);
    // 发送文件
    for(QFileInfoList::iterator iter=Filelist.begin();iter!=Filelist.end();iter++){
        file->setFileName("tbox/"+iter->fileName());
        file->open(QIODevice::ReadOnly);
        timer->start(10);
        loop->exec();
    }
    timer->deleteLater();
    file->deleteLater();
    loop->deleteLater();
}
void Ftp::log(QString log){
    this->addLog(tcpSocket->peerAddress().toString()+":"+QString::number(tcpSocket->peerPort())+" "+log);
}
QByteArray Ftp::getHead(int len,int action){
    QByteArray head;
    head.append(reinterpret_cast<char*>(&len),sizeof(int));
    head.append(reinterpret_cast<char*>(&action),sizeof(int));
    return head;
}
int Ftp::getLen(QByteArray& head){
    QByteArray len=head.mid(0,sizeof(int));
    return *(reinterpret_cast<int*>(len.data()));
}
int Ftp::getAction(QByteArray& head){
    QByteArray action=head.mid(sizeof(int),sizeof(int));
    return *(reinterpret_cast<int*>(action.data()));
}
int Ftp::headLen(){
    return 2*sizeof(int);
}
void Ftp::clear(){
    static bool first=true;
    if(first){
        first=false;
        this->log("离开");
        tcpSocket->close();// 关闭套接字
        tcpSocket->deleteLater();// 析构套接字
        this->finished();// 通知主线程子线程结束
    }
}
