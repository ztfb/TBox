#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QThread>
#include <QClipboard>
#include "tool.h"
#include "messagebox.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    bool message(QString text,bool flag);
    ~Widget();

private:
    Ui::Widget *ui;
    QThread* thread;
    Tool* tool;
    QList<QString> fileNames;
signals:
    void crypt(QString inputPath,QString outputPath,QByteArray key);
    void generateHash(QString path,int algorithm);
    void rename(QList<QString> paths,QString pre,int ini,int add,QString suf,QString suffixName);
};
#endif // WIDGET_H
