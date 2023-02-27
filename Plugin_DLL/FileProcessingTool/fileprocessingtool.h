#ifndef FILEPROCESSINGTOOL_H
#define FILEPROCESSINGTOOL_H

#include "FileProcessingTool_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class FILEPROCESSINGTOOL_EXPORT FileProcessingTool:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "FileProcessingTool" FILE "FileProcessingTool.json")
public:
    FileProcessingTool();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // FILEPROCESSINGTOOL_H
