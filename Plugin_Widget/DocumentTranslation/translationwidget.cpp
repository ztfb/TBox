#include "translationwidget.h"
#include "ui_translationwidget.h"

TranslationWidget::TranslationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TranslationWidget)
{
    ui->setupUi(this);
    isShow=false;
    isPress=false;
    this->setWindowFlags(Qt::Tool|Qt::FramelessWindowHint);// 任务栏不显示，去边框
    this->setAttribute(Qt::WA_TranslucentBackground);// 设置窗体透明
    this->setFocusPolicy(Qt::StrongFocus);// 获取强焦点
    ui->textEdit->setTextInteractionFlags(Qt::NoTextInteraction);// 不可选中
    manager=new QNetworkAccessManager(this);
    connect(manager,&QNetworkAccessManager::finished,this,&TranslationWidget::transReply);
    connect(ui->addButton,&QPushButton::clicked,this,[=](){
        this->getText(toText);
    });
    if(!QFile::exists("data/DocumentTranslation/license.zf"))ui->typeCcomboBox->setItemData(1,0,Qt::UserRole-1);
}
void TranslationWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::LeftButton){
        position=event->globalPos();//获取鼠标绝对位置
        isPress=true;
    }
}
void TranslationWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(isPress){
        QCursor cursor(Qt::OpenHandCursor);
        setCursor(cursor);
        QPoint p=event->globalPos();//获取当前位置
        this->move(this->pos().x()+p.x()-position.x(),this->pos().y()+p.y()-position.y());//调整窗口位置
        position=p;//更新位置
    }
}
void TranslationWidget::mouseReleaseEvent(QMouseEvent *event)
{
    QCursor cursor(Qt::ArrowCursor);
    setCursor(cursor);
    isPress=false;
}
void TranslationWidget::setisShow(bool flag){
    if(flag)this->show();
    else this->hide();
    isShow=flag;
}
void TranslationWidget::setisTop(bool flag){
    if(flag)this->setWindowFlags(this->windowFlags()|Qt::WindowStaysOnTopHint);// 置顶
    else this->setWindowFlags(this->windowFlags()&~Qt::WindowStaysOnTopHint);// 取消置顶
    if(isShow)this->show();
}
TranslationWidget::~TranslationWidget()
{
    delete ui;
}
void TranslationWidget::translation(QString text){
    text.remove('\n');// 待翻译文本（删除换行符）
    fromText=text;// 设置原文
    QString url="";
    QString rand=QString::number(QRandomGenerator::global()->bounded(0,100000000));// 获取随机数
    int index=ui->dirCcomboBox->currentIndex();// 翻译方向
    if(ui->typeCcomboBox->currentIndex()==0){// 有道
        qint64 curtime=QDateTime::currentDateTime().toMSecsSinceEpoch()/1000;// 时间戳
        QString sign=youdaoID+(text.size()<=20?text:text.mid(0,10)+QString::number(text.size())+text.mid(text.size()-10,10))+rand+QString::number(curtime)+youdaoKEY;// 生成数字签名
        QString sha256 = QCryptographicHash::hash(sign.toUtf8(),QCryptographicHash::Sha256).toHex();// 生成sha256加密签名
        url=youdaoURL+QString("?q=%1&from=%2&to=%3&appKey=%4""&salt=%5&sign=%6&signType=%7&curtime=%8")
                .arg(text).arg((index==0?"en":"zh-CHS")).arg((index==0?"zh-CHS":"en")).arg(youdaoID).arg(rand).arg(sha256).arg("v3").arg(QString::number(curtime));// 生成URL
    }else if(ui->typeCcomboBox->currentIndex()==1){// 百度
        QString sign=baiduID+text+rand+baiduKEY;// 生成数字签名
        QString MD5 = QCryptographicHash::hash(sign.toUtf8(),QCryptographicHash::Md5).toHex();// 生成md5加密签名
        url=baiduURL+QString("?q=%1&from=%2&to=%3&appid=%4""&salt=%5&sign=%6")
                .arg(text).arg((index==0?"en":"zh")).arg((index==0?"zh":"en")).arg(baiduID).arg(rand).arg(MD5);// 生成URL
    }
    manager->get(QNetworkRequest(QUrl(url)));// 上传
}
void TranslationWidget::transReply(QNetworkReply *reply){
    QJsonParseError jsonError;// 错误类型
    QByteArray data=reply->readAll();// 获得返回值
    QJsonDocument json = QJsonDocument::fromJson(data,&jsonError);
    if(!json.isEmpty()&&json.isObject()){
        QJsonObject object = json.object();
        if(object.contains("errorCode")){// 有道
            if(object.value("errorCode").toInt()==0){
                toText=object.value("translation").toArray().at(0).toString();// 二次解码
                this->setResult();
            }
            else{// 翻译出错
                QString result;
                switch (object.value("errorCode").toInt()) {
                case 101:result ="必填参数为空";break;case 202:result ="签名错误";break;case 411:result ="速度过快访问频率受限";break;
                case 401:result ="账户余额不足";break;case 412:result ="请求频繁";break;default:result ="其他错误";break;
                }
                QMessageBox::critical(this,"错误",result);
            }
        }else{// 百度
            if(object.contains("error_code")){// 出错
                QString result;
                int code=object.value("error_code").toInt();// 获取错误代码
                switch (code) {
                case 52001:result ="请求超时";break;case 52002:result ="系统错误";break;case 54000:result ="必填参数为空";break;
                case 54001:result ="签名错误";break;case 54003:result ="速度过快访问频率受限";break;case 54004:result ="账户余额不足";break;
                case 54005:result ="请求频繁";break;case 58002:result ="服务关闭";break;default:result ="其他错误";break;
                }
                QMessageBox::critical(this,"错误",result);
            }else{
                toText=object.value("trans_result").toArray().at(0).toObject().value("dst").toString();// 二次解码
                this->setResult();
            }
        }
    }
    reply->deleteLater();
}
void TranslationWidget::setResult(){
    ui->textEdit->clear();
    ui->textEdit->append("原文：");
    ui->textEdit->append(fromText);
    ui->textEdit->append("译文：");
    ui->textEdit->append(toText);
}
