#ifndef MYKNXNETWORK_H
#define MYKNXNETWORK_H

#include <QObject>
#include <QUdpSocket>
#include <QTimer>
#include <netipconnectrequest.h>
#include <netipconnectresponse.h>
#include <netipconnectionstaterequest.h>
#include <netipconnectionstateresponse.h>
#include <netipdisconnectrequest.h>
#include <netipdisconnectresponse.h>
#include <netipdescriptionrequest.h>
#include <netipdescriptionresponse.h>

#include <netipsrp.h>
#include <basicdatatype.h>

#include <netipservicediscoveryagent_p.h>
#include <netiptunnelingrequest.h>
#include <qnewlinklayerframebuilder.h>

#include <netiptunnelingacknowledge.h>

#include "pendingackmanager.h"

class MYLIB_EXPORT myKNXnetwork : public QObject
{
    Q_OBJECT
public:


    explicit myKNXnetwork(QObject * parent = nullptr);
    ~myKNXnetwork();
    void cleanup();

    void searchrequest_ontime();
    void description_ontime();
    void connectrequest_ontime();

    void timerStatus_init();
    void mconnectstatus_init();//管理连接的状态问询
    void mdisconnectstatus();
    void onconnectStatusReq(quint8 channelID);



    bool init();//初始化套接字和状态寄存器等资源
    void setState(Appstate);

    void onNetworkInfo();//刷新本地网络信息
    void onflushIpport();//刷新本地IP地址显示控件。

    void onsearchrequest(QString IP);
    void onsearchrequestextend(QString IP);

    void onconnectrequest(QList<QString> ipandlocalip);
    void ondescriptionrequest(QList<QString> ipandlocalip);

    void ondisconnectrequest();
    void ondisconnectresponse();

    void listenKNXManageMessage();
    void listenKNXMessage();
    void onconnectStatus_asktime();


    void connectSuccess();
    void connectFailure();

    void configConnectRequest();
    void m_udpSocketCheck();
    void udpSocketCheck();

    void onConfigTimerOut();
    void tunnelRequest_KNXWrite(QString Gaddress,QString data);
    void tunnelRequest_KNXRead(QString Gaddress);
    void tunnelRequest_KNXRead_tpdu(QString Gaddress);

    void tunnelAcknowlage(quint8 sequence);
    void clearMessage();//重置一些通信的标志，用于重新连接之后使用

signals:
    void sigLocalIpaddress(QList <QHostAddress> addresses);
    void sigfindedDeviceInfo(QList <DeviceInfo>);
    void sigDetailInfo(QString);
    void sigconnectInfo(QString , quint8);
    void sigstate(Appstate);
    void sigValueSendState(ValueSendState);

    void sigKNXBusMessageReceived(KnxBusRecord record);

public slots:
       void m_cleanup();
private:
    PendingAckManager *m_ackManager;

    Appstate m_state = Appstate::Idle;
    ValueSendState VSstate = ValueSendState::None;

    QList <QNetworkInterface> ifaces;//用于管理本地电脑的网卡接口
    QList <QHostAddress> addresses;//用于管理本地电脑的IP地址

    QList <QHostAddress> serversIPlist;//用来管理发现的设备们的ip
    QList <quint16> serversportlist;
    QList <DeviceInfo> serversinfo;//用来管理发现的设备们的信息


    QUdpSocket m_udpSocket; //管理的套接字
    QUdpSocket udpSocket;  //通信的套接字

    bool connectFlag = false;//标识当前的连接状态
    bool m_cleanup_flag = false;//标识当前清理状态

    QTimer * timerStatus = nullptr;

    quint8 times_formconnect; //管理连接的问询次数。
    QString localip;
    QString serverip;

    QString detailDeviceInfo;


    quint8 m_channelID = 0;//用来管理通信的通道iD。
    quint8 m_area = 0;//用来管理三级地址的一级
    quint8 m_line = 0;//用来管理三级地址的二级
    quint8 m_seqaddressnumber = 0;//用来管理三级地址的三级


    // ondescriptiontimeout
    quint8 server_sequence = 0;//标识报文序列号  接收客户端报文使用
    quint8 m_sequencenumber=0;//标识报文序列号  本地发送通信报文使用
    quint8 timetolive = 0;

    qint16 configTimeout = 3000;
    quint16 managePort = 0;//用来维护链接状态的端口
};
#endif // MYKNXNETWORK_H
