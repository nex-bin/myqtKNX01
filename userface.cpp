#include "userface.h"
#include "ui_userface.h"

#include <mainwindow.h>
userface::userface(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::userface)
{

    ui->setupUi(this);
    init();
}

userface::~userface()
{
    delete ui;
}
void userface::init(){
    QPushButton * a = ui->pushButton_2;
    emit askLocalIpaddress();
    connect(a,&QPushButton::clicked,this,&userface::sendconnectrequest);
    connect(ui->pushButton,&QPushButton::clicked,this,&userface::sendfleshport);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&userface::sendsearchrequest);
    connect(ui->pushButton_5,&QPushButton::clicked,this,&userface::sendconnectrequest);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&userface::senddisconnectrequest);

}
void userface::resLocalIpaddress(QList <QHostAddress> addresses){
    ui->comboBox_2->clear();
    for(QHostAddress address : addresses){
        ui->comboBox_2->addItem(address.toString());
    }
}
void userface::sendfleshport(){
    emit fleshport();
}
void userface::sendsearchrequest(){
    emit searchrequest();
}
void userface::sendconnectrequest(){
    emit connectrequest();
}
void userface::senddisconnectrequest(){
    emit disconnect();
}

