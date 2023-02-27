#include "desktoppet.h"

DesktopPet::DesktopPet()
{
    widget=new Widget(this);
}
QWidget* DesktopPet::getWidget(){
    return widget;
}
QPixmap DesktopPet::getIcon(){
    return QPixmap(":/resource/DesktopPet.png");
}
void DesktopPet::recMessage(QJsonObject object){
}
