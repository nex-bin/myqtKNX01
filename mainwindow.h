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
    QUdpSocket * m_udpSocket;

    // QKnxNetIpServerDiscoveryAgent * m_dis;
    Discoverer * m_dic;//用于发现同网段可KNX通信的设备。
    QList <QNetworkInterface> ifaces;//用于管理本地电脑的网卡接口
    QList <QHostAddress> m_address;//用来管理本地ip地址
    QSet <QHostAddress> serversIP;//用来管理发现的设备们的ip
    QList <QKnxNetIpServerInfo> serversinfo;//用来管理发现的设备们的信息
    quint8 m_channelID=0;//用来管理通信的通道iD。
    quint8 m_area = 0;//用来管理三级地址的一级
    quint8 m_line = 0;//用来管理三级地址的二级
    quint8 m_seqaddressnumber = 0;//用来管理三级地址的三级
    QTimer * timerStatus;
    bool connectFlag = false;//标识当前的连接状态

    void KNXIPMuiltconnect();
    void KNXIPMuiltDisconnect();
    void KNXIPTunnelDisconnect();

    void KNXIPSearchRequest();//发送发现设备请求

    void showServerinfo(QKnxNetIpServerInfo );//用于显示发现的设备信息
    void connectRequest();//发送连接请求
    void KNXIPTunnelconnectrequest(QKnxNetIpServerInfo);
    void onReadyread();
    void tuunelstatusasksend();
    // void onReadyRead();

    void tunnelStatusAsk(const quint8 channelID,const QKnxNetIpCrd seqaddressnumber);
    void KNXIPDisConnect();

signals:
    void connectAcked(const quint8 channelID,const QKnxNetIpCrd seqaddressnumber);//参数通道ID和物理地址
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
