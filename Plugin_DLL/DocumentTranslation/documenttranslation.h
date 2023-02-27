#ifndef DOCUMENTTRANSLATION_H
#define DOCUMENTTRANSLATION_H

#include "DocumentTranslation_global.h"
#include "PluginInterface.h"
#include <QWidget>
#include <QIcon>
#include <QJsonObject>
#include "widget.h"

class DOCUMENTTRANSLATION_EXPORT DocumentTranslation:public QWidget,public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "DocumentTranslation" FILE "DocumentTranslation.json")
public:
    DocumentTranslation();
    QWidget* getWidget() override;
    QPixmap getIcon() override;
    void recMessage(QJsonObject object) override;
private:
    Widget* widget;
signals:
    void sendMessage(QJsonObject object) override;
};

#endif // DOCUMENTTRANSLATION_H
