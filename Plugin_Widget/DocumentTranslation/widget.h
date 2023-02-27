#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileDialog>
#include <QSettings>
#include <QClipboard>
#include "translationwidget.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    void closeEvent(QCloseEvent *event);
    ~Widget();

private:
    Ui::Widget *ui;
    QClipboard *board;
    TranslationWidget* transWidget;// 悬浮窗
    bool message(QString text,bool flag);
signals:
    void translation(QString text);
};
#endif // WIDGET_H
