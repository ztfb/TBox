#ifndef PLUGININTERFACE_H
#define PLUGININTERFACE_H

#include <QWidget>

class PluginInterface{// 通信接口
public:
    virtual ~PluginInterface(){}
    virtual QWidget* getWidget()=0;// 如果不是窗口程序，则返回空指针
    virtual QPixmap getIcon()=0;// 返回插件图标，大小为200*200，后缀名为png
    virtual void recMessage(QJsonObject object)=0;// 插件接收信息
    virtual void sendMessage(QJsonObject object)=0;// 插件发送信息（以信号方式实现）
};
// 接口唯一标识
#define PluginInterface_iid "PulginInterface"
// 声明接口
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(PluginInterface,PluginInterface_iid);
QT_END_NAMESPACE

#endif // PLUGININTERFACE_H
