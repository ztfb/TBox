#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QThread>
#include <QSettings>
#include <QProcess>
#include "messagebox.h"
#include "ftp.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void basicStyle();
    void updateTBox();
    ~Widget();
private:
    Ui::Widget *ui;
    QThread* thread;
    Ftp* ftp;// 负责请求插件信息
private:
    bool message(QString text,bool flag);
signals:
    void checkUpdate(QString ip,ushort port);
    void TBoxUpdate(QString ip,ushort port);
};
#endif // WIDGET_H
