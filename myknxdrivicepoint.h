#ifndef MYKNXDRIVICEPOINT_H
#define MYKNXDRIVICEPOINT_H

#include <QUdpSocket>
#include <qnewaddress.h>
#include <qtimer.h>
#include <netipservicediscoveryagent_p.h>

class myknxdrivicepoint : public QObject
{
    Q_OBJECT
public:
    //myknxdrivicepoint();

    /*
    void searchRequest(QNetworkInterface);
    void searchTimeout(QKnxNetIpServerInfo);
    void descriptionRequest();
    void connectRequest();
    void connectStatusRequest();
    void disconnectRequest();



    void onReadyRead();






    void setmyhostaddress(QHostAddress);
    void setmyiface(QNetworkInterface);

    void init();
private:
    QUdpSocket * m_udpSocket;
    QNetworkInterface m_iface;//用来标识使用的网卡信息

    QHostAddress m_hostaddress;//用来标识本地IP
    QHostAddress m_hostpoint;//用来装点对点通信对方的IP
    quint16 socketport = 3671;//用来KNX通信的端口
    quint16 manageport = 0;//用来管理KNX通信的端口

    quint8 m_channelid;//通道ID



    QTimer m_timer;
    QKnxNetIpServerDiscoveryAgent::State m_state;

    Discoverer * m_dic;//用于发现同网段可KNX通信的设备。
    bool connectFlag = false;//标识当前的连接状态


    quint8 m_area = 0;//用来管理三级地址的一级
    quint8 m_line = 0;//用来管理三级地址的二级
*/
};

#endif // MYKNXDRIVICEPOINT_H
