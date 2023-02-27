#ifndef HOOK_H
#define HOOK_H

#include <QObject>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QCursor>


class HOOK : public QObject
{
    Q_OBJECT
public:
    static HOOK* instance(){
        static HOOK hook;
        return &hook;
    }
    static bool install();// 安装监听器
    static bool uninstall();// 卸载监听器
    explicit HOOK(QObject *parent = nullptr);
signals:
    void mouseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);
public slots:
};

#endif // HOOK_H
