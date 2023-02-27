#include "settingsextension.h"

SettingsExtension::SettingsExtension()
{
    widget=new Widget(this);
    connect(widget,&Widget::callfunc,this,&SettingsExtension::sendMessage);
}
QWidget* SettingsExtension::getWidget(){
    return widget;
}
QPixmap SettingsExtension::getIcon(){
    return QPixmap(":/resource/SettingsExtension.png");
}
void SettingsExtension::recMessage(QJsonObject object){

}
