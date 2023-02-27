#include "commandlinetool.h"

CommandLineTool::CommandLineTool()
{
    widget=new Widget(this);
}
QWidget* CommandLineTool::getWidget(){
    return widget;
}
QPixmap CommandLineTool::getIcon(){
    return QPixmap(":/resource/CommandLineTool.png");
}
void CommandLineTool::recMessage(QJsonObject object){

}
