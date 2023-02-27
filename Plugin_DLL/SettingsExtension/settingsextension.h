#ifndef SETTINGSEXTENSION_H
#define SETTINGSEXTENSION_H

#include "SettingsExtension_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class SETTINGSEXTENSION_EXPORT SettingsExtension:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "SettingsExtension" FILE "SettingsExtension.json")
public:
    SettingsExtension();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // SETTINGSEXTENSION_H
