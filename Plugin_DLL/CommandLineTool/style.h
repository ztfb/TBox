#ifndef STYLE_H
#define STYLE_H

#include <QFont>
#include <QString>
#include <QColor>
class Style
{
public:
    Style();
    void setUserName(QString un);
    QString getUserName();
    void setFont(QString ft);
    QString getFont();
    char userName[25];
    char font[25];
    QColor background;
    QColor tipUser;
    QColor tipAdmin;
    QColor cmd;
    QColor info;
};

#endif // STYLE_H
