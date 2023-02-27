#ifndef FTP_H
#define FTP_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>
#include <QSemaphore>
#include <QEventLoop>
#include <QSettings>
#include <QDebug>

class Ftp : public QObject
{
    Q_OBJECT
public:
    explicit Ftp(qintptr socket,QObject *parent = nullptr);
    void log(QString log);
    void readData();
    void sendPlugin(QByteArray request);
    void sendFile(QByteArray request);
    void sendVersion();
    void sendTBox();
private:
    QTcpSocket* tcpSocket;
    QByteArray cache;// 缓冲区
    QSemaphore* lock;// 锁
private:// 工具函数
    QByteArray getHead(int len,int action);
    // action:(0,plugin);(1,file)
    int getLen(QByteArray& head);
    int getAction(QByteArray& head);
    int headLen();
    void clear();
signals:
    void finished();
    void addLog(QString log);
    void quit();
public slots:
};

#endif // FTP_H
