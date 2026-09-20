#ifndef MYKNXNETWORK_H
#define MYKNXNETWORK_H

#include <QObject>
#include <QUdpSocket>

#include <netipservicediscoveryagent_p.h>

class myKNXnetwork : public QObject
{
    Q_OBJECT
public:
    explicit myKNXnetwork(QObject * parent = nullptr);

    QUdpSocket * m_udpSocket;
    QUdpSocket * udpSocket;

    QList <QNetworkInterface> ifaces;//用于管理本地电脑的网卡接口
    QList <QHostAddress> addresses;//用于管理本地电脑的IP地址

    QList <QHostAddress> serversIPlist;//用来管理发现的设备们的ip
    QList <quint16> serversportlist;

    QList <QKnxNetIpServerInfo> serversinfo;//用来管理发现的设备们的信息

    quint8 m_channelID = 0;//用来管理通信的通道iD。
    quint8 m_area = 0;//用来管理三级地址的一级
    quint8 m_line = 0;//用来管理三级地址的二级
    quint8 m_seqaddressnumber = 0;//用来管理三级地址的三级
    QTimer * timerStatus;
    qint16 configTimeout = 3000;
    quint16 managePort = 0;//用来维护链接状态的端口
    bool connectFlag = false;//标识当前的连接状态
    quint8 SequenceNumber = 0;//标识报文序列号

    bool init(QHostAddress,qint16);//初始化。
    void getNetworkInfo();//刷新本地网络信息
    void flushIpport();//刷新本地IP地址显示控件。


    void sendManagementMessage(const QByteArray & data,const QHostAddress & address,const qint16 & port);
    void sendSearchExtendRequest(const QKnxNetIpSrp & srp,const QHostAddress & address,const qint16 & port );
    void sendSearchRequest(const QHostAddress & address,const qint16 & port);
    void listenOnReadyRead();
signals:
    void resLocalIpaddress(QList <QHostAddress> addresses);
};
#endif // MYKNXNETWORK_H
