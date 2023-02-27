#ifndef DESKTOPPET_H
#define DESKTOPPET_H

#include "DesktopPet_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class DESKTOPPET_EXPORT DesktopPet:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "DesktopPet" FILE "DesktopPet.json")
public:
    DesktopPet();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // DESKTOPPET_H
