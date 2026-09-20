#include "userpage.h"
#include "ui_userpage.h"

userpage::userpage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::userpage)
{
    ui->setupUi(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&userpage::turnToMainpage);

    init();
}

userpage::~userpage()
{
    delete ui;
}
void userpage::init(){
    connect(ui->pushButton_2,&QPushButton::clicked,this,&userpage::sendwurao);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&userpage::sendwurao);

    connect(ui->pushButton_4,&QPushButton::clicked,this,&userpage::sendwurao);

    connect(ui->pushButton_5,&QPushButton::clicked,this,&userpage::sendwurao);

    connect(ui->pushButton_6,&QPushButton::clicked,this,&userpage::sendwurao);

    connect(ui->pushButton_7,&QPushButton::clicked,this,&userpage::sendwurao);

    connect(ui->pushButton_8,&QPushButton::clicked,this,&userpage::sendwurao);
    connect(ui->pushButton_9,&QPushButton::clicked,this,&userpage::sendwurao);

}
void userpage::sendwurao(){
    qDebug()<<"wurao clicked";
    emit wurao();
}
void userpage::sendqingjie(){
    qDebug()<<"qingjie clicked";
    emit qingjie();
}
void userpage::sendopenlighting(){
    qDebug()<<"openlighting clicked";

    emit openlighting();
}
void userpage::sendcloselighting(){
    qDebug()<<"closelighting clicked";

    emit closelighting();
}
void userpage::sendrichangmode(){
    qDebug()<<"richangmode clicked";

    emit richangmode();
}
void userpage::sendmingliangmode(){
    qDebug()<<"mingliangmode clicked";

    emit mingliangmode();
}
void userpage::sendshuimianmode(){
    qDebug()<<"shuimianmode clicked";

    emit shuimianmode();
}
void userpage::sendrouhemode(){
    qDebug()<<"rouhemode clicked";

    emit rouhemode();
}
