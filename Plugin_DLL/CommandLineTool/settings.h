#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include <QFile>
#include <QDebug>
#include "style.h"
namespace Ui {
class Settings;
}

class Settings : public QDialog
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = nullptr);
    void init();// 初始化函数
    void saveStyle();
    void recoverStyle();
    ~Settings();

private:
    Ui::Settings *ui;
    Style style;
signals:
    void styleChanged();
};

#endif // SETTINGS_H
