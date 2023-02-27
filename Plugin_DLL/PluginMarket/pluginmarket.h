#ifndef PLUGINMARKET_H
#define PLUGINMARKET_H

#include "PluginMarket_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class PLUGINMARKET_EXPORT PluginMarket:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "PluginMarket" FILE "PluginMarket.json")
public:
    PluginMarket();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // PLUGINMARKET_H
