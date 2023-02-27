#include "widget.h"

#include <QApplication>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDir dir;
    if(!dir.exists("temp"))dir.mkdir("temp");
    Widget w;
    w.show();
    return a.exec();
}
