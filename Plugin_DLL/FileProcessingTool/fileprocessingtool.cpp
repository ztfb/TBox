#include "fileprocessingtool.h"

FileProcessingTool::FileProcessingTool()
{
    widget=new Widget(this);
}
QWidget* FileProcessingTool::getWidget(){
    return widget;
}
QPixmap FileProcessingTool::getIcon(){
    return QPixmap(":/resource/FileProcessingTool.png");
}
void FileProcessingTool::recMessage(QJsonObject object){

}
