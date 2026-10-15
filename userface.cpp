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
    emit sigLocalIpaddress();
    connect(ui->pushButton,&QPushButton::clicked,this,&userface::sendfleshport);
    connect(ui->pushButton_2,&QPushButton::clicked,this,&userface::sendsearchrequest);
    connect(ui->pushButton_7,&QPushButton::clicked,this,&userface::senddescriptionrequest);
    connect(ui->pushButton_5,&QPushButton::clicked,this,&userface::sendconnectrequest);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&userface::senddisconnectrequest);
    connect(ui->pushButton_6,&QPushButton::clicked,this,&userface::sendknxvalue);

    connect(ui->radioButton_2,&QRadioButton::toggled,this,&userface::init_checkbox);

    deviceFinded_plain = new QPlainTextEdit();
    deviceFinded_plain->setFixedHeight(100);
    ui->verticalLayout_6->addWidget(deviceFinded_plain);

    deviceDetailInfo_plain = new QPlainTextEdit;
    deviceDetailInfo_plain->setFixedHeight(100);
    ui->verticalLayout_2->addWidget(deviceDetailInfo_plain);

    ui->radioButton_4->setEnabled(false);

    ui->checkBox->setTristate(true);

    if(!m_address.empty()){
        for( QHostAddress ad : m_address){
            ui->comboBox_3->addItem(ad.toString());
        }
    }

    // QGroupBox * valueBox_01 = new QGroupBox("sendValue",this);
    QButtonGroup * buttongroup01 = new QButtonGroup(this);
    QRadioButton * radio1 = ui->radioButton_2;
    QRadioButton * radio2 = ui->radioButton_3;
    buttongroup01->addButton(radio1);
    buttongroup01->addButton(radio2);


}
void userface::onLocalIpaddress(QList <QHostAddress> addresses){
    ui->comboBox_2->clear();
    for(QHostAddress address : addresses){
        ui->comboBox_2->addItem(address.toString());
    }
    ui->pushButton->setEnabled(true);
}
void userface::sendfleshport(){
    qDebug()<<"click the flush button";
    emit sigfleshport();
}
void userface::sendsearchrequest(){
    //POD风格传递跨线程参数
    QString ip = ui->comboBox_2->currentText();
    qDebug()<<"click the search button";

    // ui->pushButton_2->setEnabled(false);
    emit sigsearchrequest(ip);
}
void userface::senddescriptionrequest(){
    qDebug()<<"click the description button";
    QString ip = ui->comboBox_3->currentText();
    QString iplocal = ui->comboBox_2->currentText();
    ips ipandiplocal;
    ipandiplocal.iplocal = iplocal;
    ipandiplocal.ip = ip;

    emit sigdescriptionrequest(ipandiplocal);
}
void userface::sendconnectrequest(){
    qDebug()<<"click the connect button";
    QString ip = ui->comboBox_3->currentText();
    QString iplocal = ui->comboBox_2->currentText();
    ips ipandiplocal;
    ipandiplocal.iplocal = iplocal;
    ipandiplocal.ip = ip;

    // ui->pushButton->setEnabled(false);
    // ui->pushButton_2->setEnabled(false);

    emit sigconnectrequest(ipandiplocal);
}
void userface::senddisconnectrequest(){
    qDebug()<<"click the disconnect button";

    emit sigdisconnect();
}
void userface::sendknxvalue(){
    qDebug()<<"click the sendvalue button";
    QString address = ui->lineEdit_7->text();
    if(address.isEmpty()){
        address = "2/0/0";
    }
    QString data = "00";

    if(ui->radioButton_2->isChecked()){
        data = "01";
    }
    if(ui->radioButton_3->isChecked()){
        data = "00";
    }

    init_checkbox();

    emit sigsendvalue(address,data);
}
void userface::onFindedDeviceInfo(QList <DeviceInfo> deviceinfos){
    for(DeviceInfo deviceinfo : deviceinfos){
        if(!m_address.contains(deviceinfo.address)){
            m_address.append(deviceinfo.address);
        }
        ui->comboBox_3->clear();
        for(QHostAddress ad : m_address){
            ui->comboBox_3->addItem(ad.toString());
        }
        // deviceFinded_plain->setPlainText(deviceinfo.deviceStatus_text);
        deviceFinded_plain->appendPlainText(deviceinfo.deviceStatus_text);

    }
    qDebug()<<"the number of devices discovered is :"<<deviceinfos.size()<<deviceinfos.length();
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);


}
void userface::onDeviceDetailInfo(QString st){

    deviceDetailInfo_plain->setPlainText(st);
}
void userface::onconnectInfo(QString st, quint8 channelID){
    ui->lineEdit_5->setText(QString::number(channelID));
    ui->lineEdit->setText(st);
}
void userface::onConnectState(Appstate state){
    switch(state){
        case Appstate::Connected:{
            ui->lineEdit_3->setText("connected");
        };
        break;

        case Appstate::Idle:{
            ui->lineEdit_3->setText("空闲");
        }
        break;

        case Appstate::Connecting:{
            ui->lineEdit_3->setText("连接中");

        }
        break;
        case Appstate::Descripting:{
            ui->lineEdit_3->setText("请求描述");

        }
        break;
        case Appstate::Failure:{
            ui->lineEdit_3->setText("error");
        }
        break;
        case Appstate::Searching:{
            ui->lineEdit_3->setText("searching");
        }
        break;
        defautl:{
            ui->lineEdit_3->setText("others undefined");
        }

    }
}
void userface::onValueSendState(ValueSendState state){
    switch(state){
        case ValueSendState::Failure :{
            ui->checkBox->setCheckState(Qt::Unchecked);
            break;
        }

        case ValueSendState::Success :{
            ui->checkBox->setCheckState(Qt::Checked);
            break;
        }
        default:{
            ui->checkBox->setCheckState(Qt::PartiallyChecked);
        }
    }

}
void userface::init_checkbox(){
    ui->checkBox->setCheckState(Qt::PartiallyChecked);
}