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
    void checkUpdate(QString ip,ushort port);
    void TBoxUpdate(QString ip,ushort port);
    void readData();
    void getVersion(QByteArray reply);// 获取版本信息
    void fileInfo(QByteArray reply);// 更新包文件信息
    void download(QByteArray reply);// 下载更新包
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
    void version(QJsonObject object);// 获取版本信息
    void finished();
public slots:
};

#endif // FTP_H
