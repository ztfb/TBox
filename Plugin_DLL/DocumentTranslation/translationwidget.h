#ifndef TRANSLATIONWIDGET_H
#define TRANSLATIONWIDGET_H

#include <QWidget>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QRandomGenerator>
#include <QMouseEvent>

namespace Ui {
class TranslationWidget;
}

class TranslationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationWidget(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void setisShow(bool flag);// 设置是否可见
    void setisTop(bool flag);// 设置是否置顶
    void translation(QString text);// 请求译文
    void transReply(QNetworkReply *reply);// 得到翻译结果
    void setResult();// 显示翻译结果
    QString baiduURL;
    QString youdaoURL;
    QString baiduID;
    QString youdaoID;
    QString baiduKEY;
    QString youdaoKEY;
    ~TranslationWidget();

private:
    Ui::TranslationWidget *ui;
    QNetworkAccessManager * manager;
    bool isShow;
    bool isPress;//鼠标是否拖拽
    QPoint position;//鼠标的当前位置
    QString fromText;// 原文
    QString toText;// 译文
signals:
    void getText(QString text);
};

#endif // TRANSLATIONWIDGET_H
