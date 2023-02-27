#ifndef TOOL_H
#define TOOL_H

#include <QObject>
#include <QCryptographicHash>
#include <QDir>
#include <QDebug>

class Tool : public QObject
{
    Q_OBJECT
public:
    explicit Tool(QObject *parent = nullptr);
    void crypt(QString inputPath,QString outputPath,QByteArray key);
    void generateHash(QString path,int algorithm);
    void rename(QList<QString> paths,QString pre,int ini,int add,QString suf,QString suffixName);
signals:
    void cryptFinished();
    void hashValue(QString value);
    void renameFinished();
public slots:
};

#endif // TOOL_H
