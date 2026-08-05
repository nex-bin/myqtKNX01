#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUdpSocket>
#include <qnewbytearray.h>

#include <QNetworkDatagram>   //需要有这个头，否则m_udpSocket->receiveDatagram();报错

#include <netipconnectionheader.h>
#include <netipframe.h>
#include <netipservicediscoveryagent_p.h>
#include <netipconnectrequest.h>
#include <netipconnectresponse.h>
#include <netipconnectionstaterequest.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        for (const auto ipentry : iface.addressEntries()){
            if(ipentry.ip().protocol() == QAbstractSocket::IPv4Protocol && !ipentry.ip().isLoopback() && !ipentry.ip().isLinkLocal()){
                qDebug()<<ipentry.ip().toString();
                ui->comboBox_2->addItem(ipentry.ip().toString());
                ifaces.append(iface);
                m_address.append(ipentry.ip());
            }
        }
    }

    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::KNXIPMuiltconnect);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::KNXIPMuiltDisconnect);

    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::KNXIPSearchRequest);
    connect(ui->pushButton_5,&QPushButton::clicked,this,&MainWindow::connectRequest);
    connect(ui->pushButton_4,&QPushButton::clicked,this,&MainWindow::KNXIPDisConnect);

    ui->comboBox_5->addItem("Link");
    ui->comboBox_5->addItem("Raw");
    ui->lineEdit_4->setText("3671");

}
void MainWindow::KNXIPMuiltconnect(){


    qDebug()<<"clicked button";

}
void MainWindow::KNXIPSearchRequest(){


    qDebug()<<"__________________KNXIPSearchRequest button clicked____________________ ";
    DiscovererConfig * m_disconfig = new DiscovererConfig();
    m_disconfig->DiscoveryCoreV1 = true;
    m_disconfig->Nat = false;
    m_disconfig->Multicast = true;

    qint8 index = ui->comboBox_2->currentIndex();

    Discoverer * m_dic = new Discoverer(ifaces.value(index).addressEntries().value(1).ip(),ifaces.value(index),*m_disconfig);
    connect(m_dic,&Discoverer::deviceDiscovered,this,&MainWindow::showServerinfo);

    m_dic->start();


}

void MainWindow::KNXIPMuiltDisconnect(){


    qDebug()<<"##Feature Info : KNXIPMuiltDisconnect button clicked";

}
void MainWindow::KNXIPTunnelDisconnect(){
    qDebug()<<"##Feature Info : KNXIPTunnelDisconnect button clicked";
}
void MainWindow::KNXIPTunnelconnectrequest(QKnxNetIpServerInfo info){
    qDebug()<<"this is KNXIPTunnelconnectrequest";
    qDebug()<<"info.deviceName() is : "<<info.deviceName();
    qDebug()<<"info.tunnelingInfo() is : "<<info.tunnelingInfo()  ;
    qDebug()<<"info.hostAddress() is : "<<info.hostAddress()  ;
    qDebug()<<"info.controlEndpointPort() is : "<<info.controlEndpointPort()  ;
    qDebug()<<"info.controlEndpointAddress() is : "<<info.controlEndpointAddress()  ;
    qDebug()<<"info.individualAddress() is : "<<info.individualAddress()  ;

    auto builder = QKnxNetIpSearchRequestProxy::Builder();
    auto mhpai = QKnxNetIpHpaiProxy::Builder().setHostAddress(QHostAddress("192.168.0.68")).setPort(3671).create();
    qDebug()<<typeid(mhpai).name();
    builder.setDiscoveryEndpoint(mhpai);
    QKnxNetIpFrame M_r = builder.create();

}

void MainWindow::connectRequest(){

    m_udpSocket = new QUdpSocket(this);
    m_udpSocket->bind(m_address.value(ui->comboBox_2->currentIndex()),0);
    //构建连接请求
    //构建连接控制端的HPAI
    const auto controlHpai = QKnxNetIpHpaiProxy::builder()
                                 .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                 .setHostAddress(m_address.value(ui->comboBox_2->currentIndex()))
                                 .setPort(m_udpSocket->localPort()).create();
    const auto dataEndpoint = QKnxNetIpHpaiProxy::builder()
                                 .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                 .setHostAddress(m_address.value(ui->comboBox_2->currentIndex()))
                                 .setPort(3671).create();

    QKnx::NetIp::TunnelLayer link;
    if(ui->comboBox_5->currentIndex() == 1){
        link = QKnx::NetIp::TunnelLayer::Raw;
    }
    if(ui->comboBox_5->currentIndex() == 0){
        link = QKnx::NetIp::TunnelLayer::Link;
    }


    const auto connect_cri = QKnxNetIpCriProxy::Builder()
                                 .setConnectionType(QKnx::NetIp::ConnectionType::Tunnel)
                                 .setTunnelLayer(link)
                                 // .setIndividualAddress(QKnxAddress::createIndividual(area,line,sequentialNumber)) //生成独立的物理地址
                                 // .setAdditionalData()添加附加信息
                                 .create();
    const auto connct_request = QKnxNetIpConnectRequestProxy::builder()
                                    .setControlEndpoint(controlHpai)
                                    .setDataEndpoint(dataEndpoint)
                                    .setRequestInformation(connect_cri).create();
    qDebug()<<m_address.value(ui->comboBox_2->currentIndex());
    qDebug()<<"controlHpai: "<<controlHpai.bytes().toHex();
    qDebug()<<"connect_cri: "<<connect_cri.bytes().toHex();
    qDebug()<<"connct_request: "<<connct_request.bytes().toHex();

    if(m_udpSocket->isValid()){
        qDebug()<<"分配的端口号："<<m_udpSocket->localPort();
    }
    if(connct_request.isValid())
    {
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::onReadyread);
        bool wirteflag = m_udpSocket->writeDatagram(connct_request.bytes().toByteArray(),serversinfo.value(ui->comboBox_3->currentIndex()).controlEndpointAddress() ,3671);
        if(!wirteflag){
            qDebug()<<"socket写数据失败。";
        }
    }
    else  {
        qDebug()<<"connect_requests构建失败!";
    }

}

void MainWindow::showServerinfo(QKnxNetIpServerInfo server){
    qDebug()<<"####Feature Info :this is MainWindow::showServerinfo!!!!!";
    ui->textEdit->setText("设备IP： "+server.controlEndpointAddress().toString()+"\n"+"设备名称： "+server.deviceName()+"\n"+"设备物理地址： "+server.individualAddress().toString());

    if(!serversIP.contains(server.controlEndpointAddress())){
        serversIP.insert(server.controlEndpointAddress());
        serversinfo.append(server);
    }

    ui->comboBox_3->addItem(server.controlEndpointAddress().toString());
}



void MainWindow::onReadyread(){
    qDebug()<<"####Feature Info :this is MainWindow::onReadyread!!!!!";

    while(m_udpSocket->hasPendingDatagrams()){
        const auto mdata = m_udpSocket->receiveDatagram();
        qDebug()<<mdata.data().toHex();
        const auto data = QKnxByteArray::fromByteArray(mdata.data());
        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        if(header.serviceType() == QKnxNetIp::ServiceType::ConnectResponse){
            const auto frame = QKnxNetIpFrame::fromBytes(data);
            const auto proxy = QKnxNetIpConnectResponseProxy( frame);
            qDebug()<<header.bytes().toHex();
            qDebug()<<"proxy"<<proxy.channelId()<<proxy.dataEndpoint()<<proxy.responseData();
            const auto ad = proxy.responseData().bytes().toByteArray();
            m_area = (ad.at(2)>>4)&0x0f;
            m_line =ad.at(2)&0x0f;
            m_seqaddressnumber = ad.at(3);

            m_channelID = proxy.channelId();
            ui->textEdit_3->setText("channelID: "+QString::number(m_channelID)+"   individual: "+QString::number((int)m_area)+"."+QString::number((int)m_line)+"."+QString::number((int)m_seqaddressnumber));
            ui->lineEdit->setText(QString::number((int)m_area)+"."+QString::number((int)m_line)+"."+QString::number((int)m_seqaddressnumber));
            connect(this,&MainWindow::connectAcked,this,&MainWindow::tunnelStatusAsk);

            emit connectAcked(m_channelID,proxy.responseData());
        }

        if(header.serviceType() == QKnxNetIp::ServiceType::ConnectionStateResponse){
            qDebug()<<"连接成功";
            if(!connectFlag){
                timerStatus->stop();
            }
        }

    }
}
void MainWindow::tunnelStatusAsk(const quint8 channelID,const QKnxNetIpCrd seqaddressnumber){
    qDebug()<<"####Feature Info :this is MainWindow::tunnelStatusAsk!!!!!";

    timerStatus = new QTimer;
    // timer->setSingleShot(true);
    timerStatus->setInterval(50*1000);
    connect(timerStatus, &QTimer::timeout, this, &MainWindow::tuunelstatusasksend);

    timerStatus->start();
}
void MainWindow::tuunelstatusasksend(){
    qDebug()<<"####Feature Info :this is MainWindow::tuunelstatusasksend!!!!!";

    const auto hadress = serversIP.values();
    qDebug()<<"hadress"<<hadress.value(0);
    const auto hpai = QKnxNetIpHpaiProxy::builder()
                          .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                          .setPort(3671)
                          .setHostAddress(QHostAddress("192.168.0.118"))
                          .create();
    const auto proxy = QKnxNetIpConnectionStateRequestProxy::builder()
                           .setChannelId(m_channelID)
                           .setControlEndpoint(hpai)
                           .create();
    if(connectFlag&&m_udpSocket->isValid()){
        bool flag = m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),QHostAddress("192.168.0.118"),3671);
        qDebug()<<"写成功？"<<flag;
    }
}
void MainWindow::KNXIPDisConnect(){
    qDebug()<<"####Feature Info :this is MainWindow::KNXIPDisConnect!!!!!";

    connectFlag = false;
    if(timerStatus->isActive()){
        //关掉计时器
        timerStatus->stop();
    }
    //断开连接
}




