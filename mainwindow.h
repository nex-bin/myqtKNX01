#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

#include <netipservicediscoveryagent_p.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void init();
    void fleshPort();
    QUdpSocket * m_udpSocket;
    QUdpSocket * udpSocket;

    // QKnxNetIpServerDiscoveryAgent * m_dis;
    //Discoverer * m_dic;//用于发现同网段可KNX通信的设备。
    QList <QNetworkInterface> ifaces;//用于管理本地电脑的网卡接口
    QList <QHostAddress> m_address;//用来管理本地ip地址

    QList <QHostAddress> serversIP;//用来管理发现的设备们的ip
    QList <quint16> serversport;
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

    void KNXIPMuiltconnect();
    void KNXIPMuiltDisconnect();
    void KNXIPTunnelDisconnect();


    void KNXIPSearchRequest();//发送发现设备请求

    void showServerinfo(QKnxNetIpServerInfo );//用于显示发现的设备信息
    void connectRequest();//发送连接请求
    void KNXIPTunnelconnectrequest();
    void onReadyread();//接收报文，解析
    void tuunelstatusasksend();
    void configRequest();//用于请求配置信息

    void connectStatusAsk(const quint8 channelID,const QKnxNetIpCrd seqaddressnumber);//连接状态问询
    void KNXIPDisConnect();//主动断开连接
    void KNXsend();
    void onTimeout();

    void tunnelRequest();
    void oreadyread();

    void disConnectAcked();
    void tunnelRequestAck(quint8 channelID,quint8 seqnumber,QHostAddress address,quint16 port);

signals:
    void timeout();
    void connectAcked(const quint8 channelID,const QKnxNetIpCrd seqaddressnumber);//参数通道ID和物理地址
    void connectSuccess();
    void connectError();

    void disconnectAcked();
    void tunnelconnectRequest(quint8 channelID,quint8 seqnumber,QHostAddress address,quint16 port);
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
