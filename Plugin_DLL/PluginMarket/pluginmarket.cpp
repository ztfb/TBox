#include "pluginmarket.h"

PluginMarket::PluginMarket()
{
    widget=new Widget(this);
    connect(widget,&Widget::callfunc,this,&PluginMarket::sendMessage);
}
QWidget* PluginMarket::getWidget(){
    return widget;
}
QPixmap PluginMarket::getIcon(){
    return QPixmap(":/resource/PluginMarket.png");
}
void PluginMarket::recMessage(QJsonObject object){

}
