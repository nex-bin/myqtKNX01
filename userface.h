#ifndef USERFACE_H
#define USERFACE_H

#include <basicdatatype.h>
#include <QWidget>
#include <QHostAddress>
#include "QPlainTextEdit"

#include "QGroupBox"
#include "QButtonGroup"
namespace Ui {
class userface;
}

class userface : public QWidget
{
    Q_OBJECT

public:
    explicit userface(QWidget *parent = nullptr);
    ~userface();
    QList <QHostAddress> m_address;
    QPlainTextEdit * deviceFinded_plain;
    QPlainTextEdit * deviceDetailInfo_plain;

    bool busyflag = false;


    void init();

    void init_checkbox();

    void onLocalIpaddress(QList <QHostAddress> addresses);
    void onFindedDeviceInfo(QList <DeviceInfo>);
    void onDeviceDetailInfo(QString);
    void onconnectInfo(QString , quint8);
    void onConnectState(Appstate);
    void onValueSendState(ValueSendState);

private:
    Ui::userface *ui;
    void sendfleshport();
    void sendsearchrequest();
    void senddescriptionrequest();
    void sendconnectrequest();
    void senddisconnectrequest();

    void sendknxvalue();
    void sendknxvalueread();
signals:
    void sigLocalIpaddress();
    void sigfleshport();
    void sigsearchrequest(QString localIP);
    void sigdescriptionrequest(QList<QString>);
    void sigconnectrequest(QList<QString>);
    void sigdisconnect();
    void sigsendvalue(QString,QString);
    void sigsendvalueread(QString);
};

#endif // USERFACE_H
