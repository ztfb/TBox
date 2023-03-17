#include "medicalimagebrowser.h"

MedicalImageBrowser::MedicalImageBrowser()
{
    widget=new Widget(this);
}
QWidget* MedicalImageBrowser::getWidget(){
    return widget;
}
QPixmap MedicalImageBrowser::getIcon(){
    return QPixmap(":/resource/MedicalImageBrowser.png");
}
void MedicalImageBrowser::recMessage(QJsonObject object){

}
