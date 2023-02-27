#include "documenttranslation.h"

DocumentTranslation::DocumentTranslation()
{
    widget=new Widget(this);
}
QWidget* DocumentTranslation::getWidget(){
    return widget;
}
QPixmap DocumentTranslation::getIcon(){
    return QPixmap(":/resource/DocumentTranslation.png");
}
void DocumentTranslation::recMessage(QJsonObject object){
}
