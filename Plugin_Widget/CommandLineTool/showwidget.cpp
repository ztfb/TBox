#include "showwidget.h"

ShowWidget::ShowWidget(QWidget *parent) : QWidget(parent)
{
    this->setAttribute(Qt::WA_InputMethodEnabled);
    currentN=1;
    usedW=0;
    cursorP=0;
    isShow=true;
    control=false;
    encode=0;
    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,[=](){
        this->isShow=!this->isShow;
        this->update();
    });
}
void ShowWidget::keyPressEvent(QKeyEvent *event){
    if(event->key()>=Qt::Key_Space&&event->key()<=Qt::Key_AsciiTilde){
        char key;
        if(event->key()>=Qt::Key_A&&event->key()<=Qt::Key_Z&&!LOBYTE(GetKeyState(VK_CAPITAL)))key=static_cast<char>('a'+(event->key()-Qt::Key_A));
        else key=static_cast<char>(' '+(event->key()-Qt::Key_Space));
        inText.insert(cursorP,key);
        cursorP++;
        this->update();
        bottom=true;
    }else{
        switch (event->key()) {
        //特殊功能按键
        case Qt::Key_Control:control=true;break;
        case Qt::Key_Backspace:
            if(cursorP!=0){
                cursorP--;
                inText.remove(cursorP,1);
                this->update();
            }
            break;
        case Qt::Key_Return:case Qt::Key_Enter:
        {
            QString temp="";
            if(encode==0)temp=inText;
            else if(encode==1)temp.fill('*',inText.size());
            else if(encode==2)temp.fill(' ',inText.size());
            outText.push_back(QPair<QColor,QString>(command,temp));
            history.push_back(outText);
            outText.clear();
            temp=inText;
            inText.clear();
            cursorP=0;
            this->inputFinished(temp);
            break;
        }
        case Qt::Key_Left:
            if(cursorP!=0){
                cursorP--;
                this->update();
            }
            break;
        case Qt::Key_Right:
            if(cursorP<inText.size()){
                cursorP++;
                this->update();
            }
            break;
        }
    }
}
void ShowWidget::keyReleaseEvent(QKeyEvent *event){
    control=false;
}
void ShowWidget::inputMethodEvent(QInputMethodEvent *event){
    inText.insert(cursorP,event->commitString());
    cursorP+=event->commitString().size();
    this->update();
    bottom=true;
}
void ShowWidget::wheelEvent(QWheelEvent *event){
    const int maxPt=100;
    const int minPt=6;
    int zoom=event->delta();
    if(control){
        if(zoom>0&&font.pointSize()<maxPt){
            font.setPointSize(font.pointSize()+1);
        }else if(zoom<0&&font.pointSize()>minPt){
            font.setPointSize(font.pointSize()-1);
        }
        this->update();
    }
}
void ShowWidget::paintEvent(QPaintEvent *event){
    currentN=1;
    usedW=0;// 每次绘制时恢复初值
    QPainter painter(this);
    // 绘制背景
    painter.setBrush(this->background);
    painter.drawRect(0,0,this->width(),this->height());
    // 绘制历史记录
    for(QList<QList<QPair<QColor,QString>>>::iterator i1=history.begin();i1!=history.end();i1++){
        for(QList<QPair<QColor,QString>>::iterator i2=i1->begin();i2!=i1->end();i2++){
            this->writeText(*i2,this->width()-usedW,currentN);
        }
        currentN++;// 人工换行
        usedW=0;
    }
    // 绘制当前输出
    for(QList<QPair<QColor,QString>>::iterator iter=outText.begin();iter!=outText.end();iter++){
        this->writeText(*iter,this->width()-usedW,currentN);
    }
    int tempN=currentN;
    int tempUsedW=usedW;
    // 绘制当前输入
    this->writeText(QPair<QColor,QString>(command,inText),this->width()-usedW,currentN,encode);
    // 绘制光标
    this->writeCursor(tempN,tempUsedW);
    // 计算高度
    QFontMetrics metrics=QFontMetrics(font);
    this->setMinimumHeight((initH>currentN*metrics.height()?initH:currentN*metrics.height()));
    if(bottom){// 检查置底信号
        bottom=false;
        this->reachBottom();
    }
}
void ShowWidget::output(QString outText, QColor outColor,bool isNew){
    this->outText.push_back(QPair<QColor,QString>(outColor,outText));
    if(isNew)this->newLine();
    bottom=true;
    this->update();
}
void ShowWidget::newLine(){// 换行
    cursorP=0;
    history.push_back(outText);
    outText.clear();
    inText.clear();
    bottom=true;
    this->update();
}
void ShowWidget::clear(){// 清屏
    cursorP=0;
    history.clear();
    outText.clear();
    inText.clear();
    this->update();
}
void ShowWidget::setFont(QFont f){
    font=f;
    this->update();
}
void ShowWidget::setBackground(QColor b){
    background=b;
    this->update();
}
void ShowWidget::setCommand(QColor c){
    command=c;
    this->update();
}
void ShowWidget::setIsVisible(bool is){
    isVisible=is;
    if(isVisible){
        if(!timer->isActive())timer->start(500);
    }else{
        if(timer->isActive())timer->stop();
    }
    this->update();
}
void ShowWidget::setEncode(int encode){
    this->encode=encode;
}
int ShowWidget::getLen(QString text, int i){
    if(i>=0){
        QFontMetrics metrics=QFontMetrics(font);
        int temp=metrics.boundingRect(text.mid(0,i+1)).width();
        if(text[i]==' ')temp+=metrics.boundingRect('a').width();// 空格不参与边界宽度计算，因此需要单独计算
        return temp;
    }else return 0;
}
int ShowWidget::getMaxIndex(int availableW, QString text){
    int i=0;
    for(;i<text.size();i++){
        if(availableW<=getLen(text,i))break;
    }
    return i-1;
}
void ShowWidget::writeText(QPair<QColor,QString> text,int availableW,int cN,int encode){
    QPainter painter(this);
    painter.setPen(text.first);
    painter.setFont(font);
    int i=getMaxIndex(availableW,text.second);
    QFontMetrics metrics=QFontMetrics(font);
    if(i!=-1){
        QRect rect(this->width()-availableW,(cN-1)*metrics.height(),this->width(),metrics.height());
        QString temp="";
        if(encode==0)temp=text.second.mid(0,i+1);
        else if(encode==1)temp.fill('*',i+1);
        else if(encode==2)temp.fill(' ',i+1);
        painter.drawText(rect,temp);
    }
    if(i==text.second.size()-1){
        usedW+=getLen(text.second,i);
    }else if(i!=-1){
        currentN++;
        usedW=0;
        text.second.remove(0,i+1);
        this->writeText(text,this->width(),currentN);
    }else if(i==-1){
        currentN++;
        usedW=0;
        this->writeText(text,this->width(),currentN);
    }
}
void ShowWidget::writeCursor(int currentN,int usedW){
    QFontMetrics metrics=QFontMetrics(font);
    int r=currentN;// 光标行坐标
    int c=usedW;// 光标列坐标
    int num=0;// 已经检查的数量
    QString temp=inText;
    for(int i=0;i<cursorP;i++){
        int len=getLen(temp,i-num);
        if(usedW+len<=this->width())c=usedW+len;
        else{
            temp.remove(0,i-num);
            num+=(i-num);
            c=getLen(temp,0);
            usedW=0;
            r++;
        }
    }
    if(this->isVisible&&this->isShow){
        QPainter painter(this);
        painter.setPen(Qt::white);
        painter.drawLine(c+1,(r-1)*metrics.height(),c+1,r*metrics.height());
    }
}
