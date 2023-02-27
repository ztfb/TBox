#include "ftp.h"

Ftp::Ftp(QObject *parent) : QObject(parent)
{
    lock=new QSemaphore(1);// 创建互斥锁
    tcpSocket=new QTcpSocket(this);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&Ftp::clear);
    void(QTcpSocket:: *Error)(QAbstractSocket::SocketError)=&QTcpSocket::error;
    connect(tcpSocket,Error,this,&Ftp::clear);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Ftp::readData);
}
void Ftp::findPlugin(QString ip, ushort port, QString name){
    if(tcpSocket->state()!=QAbstractSocket::HostLookupState&&tcpSocket->state()!=QAbstractSocket::ConnectingState&&tcpSocket->state()!=QAbstractSocket::ConnectedState)
        tcpSocket->connectToHost(ip,port);// 重新连接服务器
    bool result=tcpSocket->waitForConnected(3000);
    if(result){
        QByteArray data;
        data.append(name);
        data.insert(0,getHead(data.size(),0));
        tcpSocket->write(data);
    }
}
void Ftp::downloadPlugin(QString ip, ushort port, QString ID){
    if(tcpSocket->state()!=QAbstractSocket::HostLookupState&&tcpSocket->state()!=QAbstractSocket::ConnectingState&&tcpSocket->state()!=QAbstractSocket::ConnectedState)
        tcpSocket->connectToHost(ip,port);// 重新连接服务器
    bool result=tcpSocket->waitForConnected(3000);
    if(result){
        QByteArray data;
        data.append(ID);
        data.insert(0,getHead(data.size(),1));
        tcpSocket->write(data);
    }
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
                if(action==0)this->getInfo(cache.mid(revlen,len));
                else if(action==1)this->fileInfo(cache.mid(revlen,len));
                else if(action==2)this->download(cache.mid(revlen,len));
                revlen+=len;
                cache.remove(0,revlen);
            }
        }
    }
    lock->release(1);
}
void Ftp::getInfo(QByteArray reply){
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply);
    if(!jsonDocument.isNull()&&jsonDocument.isArray()){// 正确解析
        this->pluginInit(jsonDocument.array());// 获取插件信息列表
    }
}
void Ftp::fileInfo(QByteArray reply){
    QJsonDocument jsonDocument = QJsonDocument::fromJson(reply);
    if(!jsonDocument.isNull()&&jsonDocument.isArray()){// 正确解析
        array=jsonDocument.array();// 获取文件列表
        QDir dir;
        if(!dir.exists("data/PluginMarket/"+array.at(0).toObject().value("ID").toString()))dir.mkdir("data/PluginMarket/"+array.at(0).toObject().value("ID").toString());
    }
}
void Ftp::download(QByteArray reply){
    QString path="data/PluginMarket/"+array.at(0).toObject().value("ID").toString()+"/";
    QFile file(path+array.at(0).toObject().value("name").toString());
    file.open(QIODevice::Append);
    file.write(reply);
    if(file.size()==array.at(0).toObject().value("size").toInt()){
        if(array.size()==1)this->pluginInstall(array.at(0).toObject().value("ID").toString());// 最后一个文件下载完成
        array.pop_front();
    }
    file.close();
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
        tcpSocket->close();
        this->disconnection();// 断开连接
    }
}
Ftp::~Ftp(){
    delete lock;
}
