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
    connect(ui->pushButton_3,&QPushButton::clicked,this,&userpage::sendqingjie);

    connect(ui->pushButton_4,&QPushButton::clicked,this,&userpage::sendopenlighting);

    connect(ui->pushButton_5,&QPushButton::clicked,this,&userpage::sendcloselighting);

    connect(ui->pushButton_6,&QPushButton::clicked,this,&userpage::sendrichangmode);

    connect(ui->pushButton_7,&QPushButton::clicked,this,&userpage::sendmingliangmode);

    connect(ui->pushButton_8,&QPushButton::clicked,this,&userpage::sendshuimianmode);
    connect(ui->pushButton_9,&QPushButton::clicked,this,&userpage::sendrouhemode);


    connect(this,&userpage::wurao,this,&userpage::KNXValueSend);
    connect(this,&userpage::qingjie,this,&userpage::KNXValueSend);
    connect(this,&userpage::openlighting,this,&userpage::KNXValueSend);
    connect(this,&userpage::closelighting,this,&userpage::KNXValueSend);
    connect(this,&userpage::richangmode,this,&userpage::KNXValueSend);
    connect(this,&userpage::mingliangmode,this,&userpage::KNXValueSend);
    connect(this,&userpage::shuimianmode,this,&userpage::KNXValueSend);
    connect(this,&userpage::rouhemode,this,&userpage::KNXValueSend);

}
void userpage::sendwurao(){
    qDebug()<<"wurao clicked";
    emit wurao("2/0/10","01");
}
void userpage::sendqingjie(){
    qDebug()<<"qingjie clicked";
    emit qingjie("2/0/12","01");
}
void userpage::sendopenlighting(){
    qDebug()<<"openlighting clicked";

    emit openlighting("2/0/14","01");
}
void userpage::sendcloselighting(){
    qDebug()<<"closelighting clicked";

    emit closelighting("2/0/14","00");
}
void userpage::sendrichangmode(){
    qDebug()<<"richangmode clicked";

    emit richangmode("4/0/0","00");
}
void userpage::sendmingliangmode(){
    qDebug()<<"mingliangmode clicked";

    emit mingliangmode("4/0/0","01");
}
void userpage::sendshuimianmode(){
    qDebug()<<"shuimianmode clicked";

    emit shuimianmode("4/0/0","02");
}
void userpage::sendrouhemode(){
    qDebug()<<"rouhemode clicked";

    emit rouhemode("4/0/0","03");
}
