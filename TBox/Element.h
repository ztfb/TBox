#ifndef ELEMENT_H
#define ELEMENT_H

#include <QWidget>
#include <QDialog>
#include <QToolButton>
#include <QMouseEvent>
#include <QTimer>
#include <QFile>
#include <QJsonObject>
#include <QDebug>

class Logo : public QWidget
{
    Q_OBJECT
public:
    explicit Logo(QWidget *parent = nullptr);
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void setMode(int mode);
    void setAngle(int angle);
    void setImage(QString path);
private:
    QTimer* timer;
    QPixmap image;
    int currAngle;// 旋转角度
    QToolButton* label;
    int mode;// 模式
    int angle;// 单次转动角度
signals:

public slots:
};

namespace Ui {
class MessageBox;
}

class MessageBox : public QDialog
{
    Q_OBJECT

public:
    explicit MessageBox(QString text="",bool isShow=true,bool* select=nullptr,QWidget *parent = nullptr);
    ~MessageBox();

private:
    Ui::MessageBox *ui;
};

namespace Ui {
class Tab;
}

class Tab : public QWidget
{
    Q_OBJECT

public:
    explicit Tab(QString text,QString ID,QWidget *parent = nullptr);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    QString getID();
    void setFlag(bool flag);
    bool getFlag();
    ~Tab();

private:
    Ui::Tab *ui;
    bool flag;
    QString ID;
    bool isPressed;
signals:
    void closePlugin(QString ID,Tab* value);
    void selectPlugin(QString ID,Tab* value);
};

namespace Ui {
class PluginItem;
}

class PluginItem : public QWidget
{
    Q_OBJECT

public:
    explicit PluginItem(QWidget *parent = nullptr);
    void setText(QString ID,QJsonObject object);
    QString getID();
    void mouseDoubleClickEvent(QMouseEvent *event);
    ~PluginItem();
private:
    Ui::PluginItem *ui;
    QString ID;
signals:
    void run(QString ID,QString name);
    void del(QString ID,QString name,PluginItem* value);
};
#endif // ELEMENT_H
