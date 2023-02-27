#ifndef COMMANDLINETOOL_H
#define COMMANDLINETOOL_H

#include "CommandLineTool_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class COMMANDLINETOOL_EXPORT CommandLineTool:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "CommandLineTool" FILE "CommandLineTool.json")
public:
    CommandLineTool();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // COMMANDLINETOOL_H
