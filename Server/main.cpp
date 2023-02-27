#include "widget.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir dir;
    if(!dir.exists("plugins"))dir.mkdir("plugins");// 插件dll所在文件夹
    if(!dir.exists("data"))dir.mkdir("data");// 插件依赖文件所在文件夹
    if(!dir.exists("json"))dir.mkdir("json");// 插件信息所在文件夹
    if(!dir.exists("tbox"))dir.mkdir("tbox");// TBox更新包所在文件夹
    Widget w;
    w.show();
    return a.exec();
}
