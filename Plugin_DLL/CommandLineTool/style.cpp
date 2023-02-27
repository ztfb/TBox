#include "style.h"

Style::Style()
{

}
void Style::setUserName(QString un){
    QByteArray temp = un.toLocal8Bit();
    char* t=temp.data();
    strcpy(this->userName,t);
}
QString Style::getUserName(){
    return QString::fromLocal8Bit(this->userName);
}
void Style::setFont(QString ft){
    QByteArray temp = ft.toLocal8Bit();
    char* t=temp.data();
    strcpy(this->font,t);
}
QString Style::getFont(){
    return QString::fromLocal8Bit(this->font);
}
