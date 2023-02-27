#ifndef FTP_H
#define FTP_H

#include <QObject>
#include <QFileDialog>
#include <QTcpSocket>
#include <QFile>
#include <QSemaphore>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

class Ftp : public QObject
{
    Q_OBJECT
public:
    explicit Ftp(QObject *parent = nullptr);
    void findPlugin(QString ip,ushort port,QString name);
    void downloadPlugin(QString ip, ushort port, QString ID);
    void readData();
    void getInfo(QByteArray reply);// 获取插件信息
    void fileInfo(QByteArray reply);// 插件文件信息
    void download(QByteArray reply);// 下载插件
    ~Ftp();
private:
    QTcpSocket* tcpSocket;
    QByteArray cache;// 缓冲区
    QSemaphore* lock;// 锁
    QJsonArray array;// 待下载文件列表
private:// 工具函数
    QByteArray getHead(int len,int action);
    // action:(0,findPlugin);(1,downloadPlugin)
    int getLen(QByteArray& head);
    int getAction(QByteArray& head);
    int headLen();
    void clear();
signals:
    void disconnection();
    void pluginInit(QJsonArray array);// 获取插件信息
    void pluginInstall(QString ID);// 下载完成，开始安装
public slots:
};

#endif // FTP_H
