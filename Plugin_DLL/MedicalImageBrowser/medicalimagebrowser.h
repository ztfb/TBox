#ifndef MEDICALIMAGEBROWSER_H
#define MEDICALIMAGEBROWSER_H

#include "MedicalImageBrowser_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class MEDICALIMAGEBROWSER_EXPORT MedicalImageBrowser:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "MedicalImageBrowser" FILE "MedicalImageBrowser.json")
public:
    MedicalImageBrowser();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // MEDICALIMAGEBROWSER_H
