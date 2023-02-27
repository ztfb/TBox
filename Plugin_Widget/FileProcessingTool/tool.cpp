#include "tool.h"

Tool::Tool(QObject *parent) : QObject(parent)
{
}
void Tool::crypt(QString inputPath, QString outputPath, QByteArray key){
    QFile sourceFile(inputPath);
    sourceFile.open(QIODevice::ReadOnly);
    QFile targetFile(outputPath);
    targetFile.open(QIODevice::WriteOnly);
    QByteArray bytes;
    int maxFrameSize=1024*20;// 20MB
    while (!sourceFile.atEnd()) {
        bytes=sourceFile.read(maxFrameSize);
        for(int i=0;i<bytes.size();i++){
            int index=i%key.size();
            bytes[i]=bytes[i]^key[index];
        }
        targetFile.write(bytes);
    }
    sourceFile.close();
    targetFile.close();
    this->cryptFinished();
}
void Tool::generateHash(QString path, int algorithm){
    QCryptographicHash* Hash=nullptr;
    switch (algorithm) {
    case 0:Hash=new QCryptographicHash(QCryptographicHash::Md4);break;
    case 1:Hash=new QCryptographicHash(QCryptographicHash::Md5);break;
    case 2:Hash=new QCryptographicHash(QCryptographicHash::Sha1);break;
    case 3:Hash=new QCryptographicHash(QCryptographicHash::Sha224);break;
    case 4:Hash=new QCryptographicHash(QCryptographicHash::Sha256);break;
    case 5:Hash=new QCryptographicHash(QCryptographicHash::Sha384);break;
    case 6:Hash=new QCryptographicHash(QCryptographicHash::Sha512);break;
    case 7:Hash=new QCryptographicHash(QCryptographicHash::Sha3_224);break;
    case 8:Hash=new QCryptographicHash(QCryptographicHash::Sha3_256);break;
    case 9:Hash=new QCryptographicHash(QCryptographicHash::Sha3_384);break;
    case 10:Hash=new QCryptographicHash(QCryptographicHash::Sha3_512);break;
    }
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    Hash->addData(&file);
    QString value=Hash->result().toHex();
    delete Hash;// 析构
    file.close();// 关闭文件
    this->hashValue(value);
}
void Tool::rename(QList<QString> paths, QString pre, int ini, int add, QString suf, QString suffixName){
    foreach (QString path, paths) {
        QFileInfo fileinfo(path);
        QString dir=fileinfo.absoluteDir().absolutePath();
        QFile::rename(path,dir+"/"+pre+QString::number(ini)+suf+"."+suffixName);
        ini+=add;
    }
    this->renameFinished();
}
