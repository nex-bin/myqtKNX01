/*

#include "mainwin.h"
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
#include <netipconnectionstateresponse.h>
#include <netipdisconnectrequest.h>
#include <netipdisconnectresponse.h>
#include <netiptunnelingrequest.h>
#include <netipserviceinfo.h>
#include <netipdeviceconfigurationrequest.h>
#include <qnewdevicemanagementframefactory.h>
#include <netipdeviceconfigurationacknowledge.h>
#include <qnewlinklayerframebuilder.h>
#include <dpt1bit.h>
#include <netipsrp.h>

#include "QPlainTextEdit"
#include <netipdescriptionrequest.h>
#include <QButtonGroup>
#include <netiptunnelingacknowledge.h>

#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_udpSocket = new QUdpSocket(this);
    udpSocket = new QUdpSocket(this);

    QThread *networkThread = new QThread(this);
    myKNXnetwork *worker = new myKNXnetwork();

    // 【核心步骤】将 Worker 移动到子线程
    worker->moveToThread(networkThread);


    init();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::init(){

    qDebug()<<"本地IP： ";
    fleshPort();



    connect(ui->pushButton_2,&QPushButton::clicked,this,&MainWindow::KNXIPSearchRequest);//搜索
    connect(ui->pushButton_5,&QPushButton::clicked,this,&MainWindow::connectRequest);//连接
    connect(ui->pushButton_4,&QPushButton::clicked,this,&MainWindow::KNXIPDisConnect);//断开连接
    connect(ui->pushButton_6,&QPushButton::clicked,this,&MainWindow::tunnelRequest);
    connect(ui->pushButton_3,&QPushButton::clicked,this,&MainWindow::configRequest);

    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::fleshPort);//刷新
    connect(this,&MainWindow::disconnectAcked,this,&MainWindow::disConnectAcked);
    connect(this,&MainWindow::tunnelconnectRequest,this,&MainWindow::tunnelRequestAck);



    ui->comboBox_5->addItem("Link");
    ui->comboBox_5->addItem("Raw");
    ui->lineEdit_4->setText("3671");

    m_channelID = 1;
    SequenceNumber = 0;

}
void MainWindow::fleshPort(){
    qDebug()<<"fleshPort _________打印本地IP信息";
    ui->comboBox_2->clear();

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
    ui->comboBox_2->addItem("0.0.0.0");

}
void MainWindow::KNXIPMuiltconnect(){


    qDebug()<<"clicked button";

}
void MainWindow::KNXIPSearchRequest(){

    qDebug()<<"__________________KNXIPSearchRequest button clicked____________________ ";
    qDebug()<<"打印网卡信息：";
    qDebug()<<ifaces.value(ui->comboBox_2->currentIndex());

    QHostAddress ha = QHostAddress(ui->comboBox_2->currentText());

    //处理套接字网络通信的回环设置
    if(ui->radioButton->isChecked()){
        m_udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 1);
    }
    if(!ui->radioButton->isChecked()){
        m_udpSocket->setSocketOption(QAbstractSocket::MulticastLoopbackOption, 0);
    }

    if(!m_udpSocket->bind(ha,0,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint)){
        qDebug()<<"@@@        socket 绑定失败。";
    }

    qDebug()<<m_udpSocket->state()<<m_udpSocket->isValid();

    bool flag = m_udpSocket->joinMulticastGroup(QHostAddress("224.0.23.12"));
    if(!flag){
        qDebug()<<"@@@        --加入组播失败--";
    }
    connect(m_udpSocket,&QUdpSocket::readyRead,this,&MainWindow::onReadyread);







    const auto hpai = QKnxNetIpHpaiProxy::Builder()
                          .setHostAddress(ha)
                          .setPort(m_udpSocket->localPort())
                          .setHostProtocol(QKnx::NetIp::HostProtocol::UDP_IPv4)
                          .create();
    const auto proxy = QKnxNetIpSearchRequestProxy::Builder()
                           .setDiscoveryEndpoint(hpai)
                           .create();

    qDebug()<<"socket 初始化"<<"port:"<<m_udpSocket->localPort();






    const auto srp = QKnxNetIpSrpProxy::supportedFamilyBuilder()
                         .setMandatory(false)
                         .setServiceInfo({QKnxNetIp::ServiceFamily::Core,2})
                         .create();
    const auto srp1 = QKnxNetIpSrpProxy::programmingModeBuilder()
                          //对仅处于编程模式的设备进行搜索。
                          .setMandatory(true)
                          .create();
    const auto srp2 = QKnxNetIpSrpProxy::requestDibsBuilder()
                          .setDescriptionTypes({QKnxNetIp::DescriptionType::DeviceInfo,
                              QKnxNetIp::DescriptionType::SupportedServiceFamilies,
                              QKnxNetIp::DescriptionType::ExtendedDeviceInfo,
                              // QKnxNetIp::DescriptionType::ManufacturerData,
                              QKnxNetIp::DescriptionType::SecuredServices,
                              QKnxNetIp::DescriptionType::TunnelingInfo
                          })
                          .setMandatory(false)
                          .create();
    // const auto srp3 = QKnxNetIpSrpProxy::macAddressBuilder()
    //                         .setMac()
    //                         .setMandatory()
    //                       .create();

    QVector<QKnxNetIpSrp> srps;
    srps.append(srp2);
    qDebug()<<"搜索请求附加字段："<<srp2.bytes().toByteArray().toHex();
    const auto proxyrequestextend = QKnxNetIpSearchRequestProxy::ExtendedBuilder()
                                        .setExtendedParameters(srps)
                                        .setDiscoveryEndpoint(hpai)
                                        .create();
    m_udpSocket->writeDatagram(proxyrequestextend.bytes().toByteArray(),QHostAddress("224.0.23.12"),3671);



    bool flag1 = m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),QHostAddress("224.0.23.12"),3671);
    if(!flag1){
        qDebug()<<"@@@        --组播广播失败--";
    }
    qDebug()<<"search request:"<<proxy.bytes().toByteArray().toHex();

    timerStatus = new QTimer;
    timerStatus->setSingleShot(true);
    connect(timerStatus, &QTimer::timeout, this, &MainWindow::onTimeout);
    qDebug()<<"开始定时器 ----- 3000ms";
    timerStatus->start(configTimeout);


}
void MainWindow::onTimeout(){
    qDebug()<<"searchrequest------------定时结束";

    if(serversinfo.size() == 0){
        qDebug()<<"搜索到设备数量："<<serversIP.size();
        //如果一个设备都没有搜索到。
        // emit timeout();
    }else {
        qDebug()<<"搜索到设备数量："<<serversIP.size()<<"_!0_";
        //如果搜多到了设备
        // timerStatus->stop();
        timerStatus->disconnect();
        timerStatus->deleteLater();
        timerStatus = nullptr;
    }
}


void MainWindow::KNXIPMuiltDisconnect(){


    qDebug()<<"##Feature Info : KNXIPMuiltDisconnect button clicked";

}
void MainWindow::KNXIPTunnelDisconnect(){
    qDebug()<<"##Feature Info : KNXIPTunnelDisconnect button clicked";
}
void MainWindow::KNXIPTunnelconnectrequest(){
    qDebug()<<"this is KNXIPTunnelconnectrequest";
    connectRequest();
    //发送隧道连接请求
    //解析隧道连接请求
    //发送隧道连接请求
    //解析隧道连接请求
    //发送隧道连接请求
    //解析隧道连接请求
    //申请断开连接
    //解析断开连接
    //发送配置连接请求
    //解析配置连接请求
    //发送配置连接请求
    //解析配置连接请求
    //发送配置连接请求
    //解析配置连接请求
    //申请断开连接
    //解析断开连接
    //发值
}

void MainWindow::connectRequest(){
    qDebug()<<"####Feature Info :this is MainWindow::connectRequest!!!!!";

    ui->lineEdit_3->setText("connectRequest");

    QHostAddress ha = QHostAddress(ui->comboBox_2->currentText());
    m_udpSocket->bind(ha,0);
    connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::onReadyread);

    qDebug()<<"socket is: "<<m_udpSocket->isValid()<<"ha is :"<<ha;

    managePort = m_udpSocket->localPort();
    //构建连接请求
    //构建连接控制端的HPAI
    const auto controlHpai = QKnxNetIpHpaiProxy::builder()
                                 .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                 .setHostAddress(ha)
                                 .setPort(managePort).create();
    const auto dataEndpoint = QKnxNetIpHpaiProxy::builder()
                                  .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                  .setHostAddress(ha)
                                  .setPort(3671).create();

    QKnx::NetIp::TunnelLayer link;
    if(ui->comboBox_5->currentIndex() == 1){
        link = QKnx::NetIp::TunnelLayer::Raw;
    }
    if(ui->comboBox_5->currentIndex() == 0){
        link = QKnx::NetIp::TunnelLayer::Link;
    }

    const auto proxy_describetion = QKnxNetIpDescriptionRequestProxy::builder()
                                        .setControlEndpoint(controlHpai)
                                        .create();
    bool flag_desc = m_udpSocket->writeDatagram(proxy_describetion.bytes().toByteArray(),QHostAddress(ui->comboBox_3->currentText()),3671);
    if( !flag_desc ){
        qDebug()<<"describe request 写错误";
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
    qDebug()<<ha;
    qDebug()<<"controlHpai: "<<controlHpai.bytes().toHex();
    qDebug()<<"connect_cri: "<<connect_cri.bytes().toHex();
    qDebug()<<"connct_request: "<<connct_request.bytes().toHex();

    if(m_udpSocket->isValid()){
        ui->lineEdit_2->setText(QString::number(managePort));
        qDebug()<<"分配的端口号："<<managePort ;
    }
    if(connct_request.isValid())
    {
        connect(m_udpSocket, &QUdpSocket::readyRead, this, &MainWindow::onReadyread);

        bool wirteflag = m_udpSocket->writeDatagram(connct_request.bytes().toByteArray(),serversIP.value(ui->comboBox_3->currentIndex()),3671);
        if(!wirteflag){
            qDebug()<<"socket写数据失败。";
        }

        qDebug()<<"构建成功：——————————"<<connct_request.bytes().toByteArray().toHex()<<"目标IP: "<<serversIP.value(ui->comboBox_3->currentIndex());
    }
    else  {
        qDebug()<<"connect_requests构建失败!";
    }

    connect(udpSocket,&QUdpSocket::readyRead,this,&MainWindow::oreadyread);

    if (!udpSocket) {
        udpSocket = new QUdpSocket(this);
    }
    if (udpSocket->state() != QAbstractSocket::BoundState) {
        bool flag = udpSocket->bind(QHostAddress(ui->comboBox_2->currentText()), 3671,
                                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        if (!flag) {
            qDebug() << "绑定失败：" << udpSocket->errorString();
            return; // 绑定失败，直接退出
        }
        qDebug() << "绑定成功，本地端口：" << udpSocket->localPort();
    }

}

void MainWindow::showServerinfo(QKnxNetIpServerInfo server){
    qDebug()<<"####Feature Info :this is MainWindow::showServerinfo!!!!!";
    qDebug()<<server.tunnelingInfo();
    // QString channel = ("设备IP： "+server.controlEndpointAddress().toString()+"\n"+"设备名称： "+server.deviceName()+"\n"+"设备物理地址： "+server.individualAddress().toString());

    // if(!serversIP.contains(server.controlEndpointAddress())){
    //     serversIP.append(server.controlEndpointAddress());
    //     serversinfo.append(server);
    // }

    // ui->comboBox_3->addItem(server.controlEndpointAddress().toString());
}



void MainWindow::onReadyread(){
    qDebug()<<"####Feature Info :this is MainWindow::onReadyread!!!!!";

    while(m_udpSocket->hasPendingDatagrams()){
        const auto mdata = m_udpSocket->receiveDatagram();
        qDebug()<<mdata.data().toHex();
        const auto data = QKnxByteArray::fromByteArray(mdata.data());
        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        const auto frame = QKnxNetIpFrame::fromBytes(data);


        switch (header.serviceType()) {
        case QKnxNetIp::ServiceType::ExtendedSearchRequest:{
            qDebug()<<"ExtendedSearchRequest-0x020B-       "<<mdata.senderAddress();
            const auto proxy = QKnxNetIpSearchRequestProxy(frame);
            qDebug()<<proxy.extendedSearchParameters();
            break;
        }

        case QKnxNetIp::ServiceType::SearchRequest:
        {
            qDebug()<<"searchRequest-0x0201-       "<<mdata.senderAddress();
            const auto proxy = QKnxNetIpSearchRequestProxy(frame);
            QKnxNetIpHpai hpai = proxy.discoveryEndpoint();
            const auto hpaiproxy = QKnxNetIpHpaiProxy(hpai);
            qDebug()<<"Message Sender is :"<<mdata.senderAddress()<<"Message hpai is :"<<hpaiproxy.hostAddress();
            break;
        }
        case QKnxNetIp::ServiceType::SearchResponse:
        {
            qDebug()<<"searchresponse-0x202-              ";
            const auto proxy = QKnxNetIpSearchResponseProxy(frame);
            const auto hpai1 = QKnxNetIpDib::fromBytes(data,6);
            const auto dib1 = QKnxNetIpDib::fromBytes(data,6+hpai1.bytes().size());
            const auto dib1proxy = QKnxNetIpDeviceDibProxy(dib1);
            const auto dib2 = QKnxNetIpDib::fromBytes(data,6+hpai1.bytes().size()+dib1.size());
            const auto dib2proxy = QKnxNetIpDeviceDibProxy(dib2);

            QKnxNetIpHpai hpai = proxy.controlEndpoint();
            QKnxNetIpHpaiProxy pro = QKnxNetIpHpaiProxy(hpai);

            qDebug()<<"响应的HPAI："<<hpai1.bytes().toByteArray().toHex();
            qDebug()<<"响应的设备描述："<<dib1.bytes().toByteArray().toHex();




            if(!serversIP.contains(pro.hostAddress())){


                QMetaEnum meta = QMetaEnum::fromType<QKnx::NetIp::ProgrammingMode>();

                QString deviceStatus = meta.valueToKey(static_cast<int>(dib1proxy.deviceStatus()));
                qDebug()<<"设备状态："<<deviceStatus<<"\n"<<"物理地址："<<dib1proxy.individualAddress();
                qDebug()<<"设备名称："<<dib1proxy.deviceName()<<dib1proxy.macAddress().toHex();

                qDebug()<<"响应来自："<<pro.hostAddress()<<pro.port();


                QPlainTextEdit * plainTextEdit = new QPlainTextEdit(ui->verticalLayout_6->widget());
                plainTextEdit->setFixedHeight(100);
                plainTextEdit->setObjectName("device:"+pro.hostAddress().toString());
                QString ab = "IP地址：" + pro.hostAddress().toString()+" : "+QString::number(pro.port())+"\n"+ "编程模式：" + deviceStatus+"\n"+"物理地址："+dib1proxy.individualAddress().toString()+
                             "\n"+"设备名称："+dib1proxy.deviceName()+"\n"+"mac地址："+dib1proxy.macAddress().toHex().toByteArray()+
                             "\n"+"设备硬件编码："+dib1proxy.serialNumber().toHex().toByteArray();
                plainTextEdit->setPlainText(ab);
                qDebug()<<ab;
                ui->verticalLayout_6->addWidget(plainTextEdit);

                serversIP.append(mdata.senderAddress());
                serversport.append(mdata.senderPort());




            }
            qDebug()<<"serversIp size :"<<serversIP.size()<<serversport.size()<<serversinfo.size();

            for(QHostAddress ip : serversIP){
                ui->comboBox_3->addItem(ip.toString());
            }

            break;
        }
        case QKnxNetIp::ServiceType::ExtendedSearchResponse:
        {
            qDebug()<<"ExtendedSearchResponse-0x020c-              ";
            qDebug()<<mdata.data();

            break;
        }
        case QKnxNetIp::ServiceType::ConnectRequest:
        {
            qDebug()<<"connectRequest-0x0205-              ";
            // const auto proxy = QKnxNetIpConnectRequestProxy(frame);
            qDebug()<<frame.bytes().toHex();
            break;
        }
        case QKnxNetIp::ServiceType::ConnectResponse:
        {
            qDebug()<<"ConnectResponse-0x0206-              ";
            const auto proxy = QKnxNetIpConnectResponseProxy( frame);
            qDebug()<<"header is : "<<header.bytes().toHex();
            qDebug()<<"channelID is : "<<proxy.channelId()<<"dataENdPoint is : "<<proxy.dataEndpoint()<<proxy.responseData();
            const auto ad = proxy.responseData().bytes().toByteArray();
            m_area = (ad.at(2)>>4)&0x0f;
            m_line =ad.at(2)&0x0f;
            m_seqaddressnumber = ad.at(3);

            m_channelID = proxy.channelId();
            const auto da = proxy.responseData();
            // qDebug()<<"连接响应数据："<<da;
            QPlainTextEdit * plainedit_channelID = new QPlainTextEdit(ui->verticalLayout_7->widget());
            QString text_channelId = "channelID: "+QString::number(m_channelID)+"   individual: "+QString::number((int)m_area)+"."+QString::number((int)m_line)+"."+QString::number((int)m_seqaddressnumber);
            plainedit_channelID->setPlainText(text_channelId);
            ui->verticalLayout_7->addWidget(plainedit_channelID);
            ui->lineEdit->setText(QString::number((int)m_area)+"."+QString::number((int)m_line)+"."+QString::number((int)m_seqaddressnumber));

            timerStatus = new QTimer;
            // timer->setSingleShot(true);
            timerStatus->setInterval(50*1000);

            connect(this,&MainWindow::connectAcked,this,&MainWindow::connectStatusAsk);
            connect(timerStatus,&QTimer::timeout,this,[=](){
                qDebug()<<"定时器触发:  开始状态问询";
                emit connectAcked(m_channelID,da);
            });
            timerStatus->start();
            break;
        }
        case QKnxNetIp::ServiceType::ConnectionStateRequest:{
            qDebug()<<"ConnectStateRequest-0x207-              ";
            break;
        }
        case QKnxNetIp::ServiceType::ConnectionStateResponse:
        {
            qDebug()<<"ConnectionStateResponse-0x0208-              ";
            const auto proxy = QKnxNetIpConnectionStateResponseProxy( frame);
            if(proxy.status() == QKnx::NetIp::Error::None){
                qDebug()<<"连接成功";
                // connect(this,&MainWindow::connectSuccess,this,&MainWindow::tunnelRequest);
                emit connectSuccess();
            }else{
                //关闭链接 disconnect();
                qDebug()<<"响应异常：状态码"<<proxy.status();
                connect(this,&MainWindow::connectError,this,&MainWindow::KNXIPDisConnect);
                emit connectError();
            }
            break;
        }


        case QKnxNetIp::ServiceType::DeviceConfigurationRequest:{
            qDebug()<<"DeviceConfigurationRequest-0x0310-              ";
            break;
        }
        case QKnxNetIp::ServiceType::DeviceConfigurationAcknowledge:{
            qDebug()<<"DeviceConfigurationAcknowledge-0x0311-              "<<QKnxNetIp::ServiceType::DeviceConfigurationAcknowledge;
            const auto proxy = QKnxNetIpDeviceConfigurationAcknowledgeProxy(frame);

            qDebug()<<frame<<proxy.status();

            break;
        }

        case QKnxNetIp::ServiceType::DisconnectRequest:
        {
            qDebug()<<"DisconnectRequest-0x0209-              ";
            // const auto proxy = QKnxNetIpDisconnectResponseProxy.builder().creat;

            const auto proxy = QKnxNetIpDisconnectResponseProxy(frame);
            if(proxy.status() == QKnxNetIp::Error::None){
                qDebug()<<"disconnect normally------"<<proxy.status();
            }else{
                qDebug()<<"disconnect error------"<<proxy.status();
            }
            timerStatus->stop();
            qDebug()<<"状态问询停止";
            emit disconnectAcked();
            break;
        }
        case QKnxNetIp::ServiceType::DisconnectResponse:
        {
            qDebug()<<"DisconnectResponse-0x020A-              ";
            const auto proxy = QKnxNetIpDisconnectResponseProxy( frame);
            qDebug()<<proxy.channelId()<<proxy.status();
            break;
        }
        case QKnxNetIp::ServiceType::DescriptionRequest:{
            qDebug()<<"DescriptionRequest-0x0203-              ";
            break;
        }
        case QKnxNetIp::ServiceType::DescriptionResponse:
        {
            qDebug()<<"DescriptionResponse-0x0204-              ";
            break;
        }

        // TunnelingRequest = 0x0420,
        //     TunnelingAcknowledge = 0x0421,
        //     TunnelingFeatureGet = 0x0422,
        //     TunnelingFeatureResponse = 0x0423,
        //     TunnelingFeatureSet = 0x0424,
        //     TunnelingFeatureInfo = 0x0425,
        case QKnxNetIp::ServiceType::TunnelingRequest:{
            qDebug()<<"TunnelingRequest-0x0420-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingAcknowledge:{
            qDebug()<<"TunnelingAcknowledge-0x0421-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureGet:{
            qDebug()<<"TunnelingFeatureGet-0x0422-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureResponse:{
            qDebug()<<"TunnelingFeatureResponse-0x0423-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureSet:{
            qDebug()<<"TunnelingFeatureSet-0x0424-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureInfo:{
            qDebug()<<"TunnelingFeatureInfo-0x0425-              ";
            break;
        }


        case QKnxNetIp::ServiceType::RoutingIndication:
        {
            qDebug()<<"RoutingIndication-0x0530-              ";
            break;

        }
        case QKnxNetIp::ServiceType::RoutingBusy:
        {
            qDebug()<<"RoutingBusy-0x0532-              ";
            break;
        }
        case QKnxNetIp::ServiceType::RoutingLostMessage:
        {
            qDebug()<<"RoutingLostMessage-0x0531-              ";
            break;
        }
        case QKnxNetIp::ServiceType::RoutingSystemBroadcast:
        {
            qDebug()<<"RoutingSystemBroadcast-0x0533-              ";
            break;
        }
            // SecureWrapper = 0x0950,
            //     SessionRequest = 0x0951,
            //     SessionResponse = 0x0952,
            //     SessionAuthenticate = 0x0953,
            //     SessionStatus = 0x0954,
            //     TimerNotify = 0x0955
        case QKnxNetIp::ServiceType::SecureWrapper:
        {
            qDebug()<<"SecureWrapper-0x0950-              ";
            break;
        }
        case QKnxNetIp::ServiceType::SessionRequest:
        {
            qDebug()<<"SessionRequest-0x0951-              ";
            break;
        }
        case QKnxNetIp::ServiceType::SessionResponse:
        {
            qDebug()<<"SessionResponse-0x0952-              ";
            break;
        }
        case QKnxNetIp::ServiceType::SessionAuthenticate:
        {
            qDebug()<<"SessionAuthenticate-0x0953-              ";
            break;
        }
        case QKnxNetIp::ServiceType::SessionStatus:
        {
            qDebug()<<"SessionStatus-0x0954-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TimerNotify:
        {
            qDebug()<<"TimerNotify-0x0955-              ";
            break;
        }
        default:
            qDebug()<<"出乎意料的报文头："<<header.serviceType();
            qDebug()<<"报文： "<<QKnxNetIpFrame::fromBytes(data);
            break;
        }

    }
}

void MainWindow::connectStatusAsk(const quint8 channelID,const QKnxNetIpCrd crd){
    qDebug()<<"####Feature Info :this is MainWindow::connectStatusAsk!!!!!";
    ui->lineEdit_3->setText("connectStatusRequest");

    const auto hisadress = serversIP.value(0);
    qDebug()<<"目标IP："<<hisadress;
    const auto hpai = QKnxNetIpHpaiProxy::builder()
                          .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                          .setPort(managePort)
                          .setHostAddress(m_address.value(ui->comboBox_2->currentIndex()))
                          .create();
    const auto proxy = QKnxNetIpConnectionStateRequestProxy::builder()
                           .setChannelId(channelID)
                           .setControlEndpoint(hpai)
                           .create();

    bool flag = m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),hisadress,3671);
    qDebug()<<"写成功？"<<flag<<" connectStatusAsk is : "<<proxy.bytes().toByteArray().toHex();

}
void MainWindow::tuunelstatusasksend(){
    qDebug()<<"####Feature Info :this is MainWindow::tuunelstatusasksend!!!!!";


}
void MainWindow::KNXIPDisConnect(){
    qDebug()<<"####Feature Info :this is MainWindow::KNXIPDisConnect!!!!!";
    const auto hisadress = serversIP.value(0);
    //检查
    connectFlag = false;
    if(timerStatus->isActive()){
        //关掉计时器
        timerStatus->stop();
    }

    //断开连接
    const auto hpai = QKnxNetIpHpaiProxy::Builder()
                          .setHostAddress(m_address.value((ui->comboBox_2->currentIndex())))
                          .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                          .setPort(managePort)
                          .create();
    const auto proxy = QKnxNetIpDisconnectRequestProxy::builder()
                           .setChannelId(m_channelID)
                           .setControlEndpoint(hpai)
                           .create();
    bool flag = m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),hisadress,3671);
    qDebug()<<"写成功？"<<flag<<proxy.bytes().toByteArray().toHex();


}
void MainWindow::configRequest(){
    //配置请求，需要先有配置连接请求，然后才可以配置请求。
    //配置连接请求和隧道连接请求的CRI是不一样的。 隧道连接请求 0404 0200   配置连接请求 0203
    qDebug()<<"_______configRequest________";

    QKnxByteArray da = QKnxByteArray(0x1001,2);
    const auto cemi = QKnxDeviceManagementFrame::PropertyReadBuilder()
                          .setNumberOfElements(1)
                          .setStartIndex(1)
                          .setObjectType(QKnxInterfaceObjectType::Device)
                          .setObjectInstance(1)
                          .setProperty(QKnxInterfaceObjectProperty::Device::DeviceDescriptor)
                          .createRequest();
    // const auto cemi = QKnxDeviceManagementFrame::Builder()
    //                       .setMessageCode(QKnxDeviceManagementFrame::MessageCode::PropertyReadRequest)
    //                       .setObjectType(QKnxInterfaceObjectType::Device)
    //                       .setObjectInstance(1)
    //                       .setProperty(QKnxInterfaceObjectProperty::Device::DeviceDescriptor)
    //                       // .setData(da)
    //                       .createFrame();

    const auto proxy = QKnxNetIpDeviceConfigurationRequestProxy::Builder()
                           .setChannelId(m_channelID)
                           .setSequenceNumber(SequenceNumber)
                           .setCemi(cemi)
                           .create();
    bool flag = m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),serversIP.value(0),3671);
    qDebug()<<"configRequest"<<proxy.bytes().toByteArray().toHex()<<"\n";
    if(!flag){
        qDebug()<<"socket configRequest 写出错";
    }


}
void MainWindow::KNXsend(){
    qDebug()<<"隧道连接，发送数据。";
    // const auto proxy = QKnxNetIpTunnelingRequestProxy::Builder()
    //     .setChannelId(m_channelID).setSequenceNumber(0);
}
void MainWindow::tunnelRequest(){
    qDebug()<<"####Feature Info :this is MainWindow::tunnelRequest!!!!!";
    const auto tpci = QKnxTpdu::TransportControlField::DataGroup;
    const auto apci = QKnxTpdu::ApplicationControlField::GroupValueWrite;
    QKnxByteArray data = QKnxByteArray::fromHex("00 01");

    QString address = ui->lineEdit_7->text();

    if(address.isEmpty()){
        address = "2/0/0";
    }
    QButtonGroup * valueGroup = new QButtonGroup(this);
    valueGroup->addButton(ui->radioButton_2,1);
    valueGroup->addButton(ui->radioButton_3,0);

    int selectID = valueGroup->checkedId();
    if(selectID == 0){
        data = QKnxByteArray::fromHex("00 00");
    }else     if(selectID == 1){
        data = QKnxByteArray::fromHex("00 01");
    }
    const auto tpdu = QKnxTpdu(tpci,apci,data);

    // 1. 构建一个cEMI帧 (比如控制一个灯)
    QKnxLinkLayerFrame linkFrame = QKnxLinkLayerFrame::builder()
                                       .setDestinationAddress({ QKnxAddress::Type::Group, QLatin1String(address.toLatin1()) }) // 示例组地址
                                       // .setSourceAddress(QKnxAddress(QKnxAddress::Type::Group,241)) // 设置源地址
                                       .setSourceAddress({ QKnxAddress::Type::Individual, QLatin1String("14.0.0") })
                                       // ... 设置其他cEMI参数
                                       .setTpdu(tpdu)

                                       .createFrame();

    if(!ui->lineEdit_5->text().isEmpty()){
        m_channelID = ui->lineEdit_5->text().toInt();
    }
    if(!ui->lineEdit_6->text().isEmpty()){
        SequenceNumber = ui->lineEdit_6->text().toInt();
    }

    const auto  proxy = QKnxNetIpTunnelingRequestProxy::builder()
                           .setChannelId(m_channelID)
                           .setSequenceNumber(SequenceNumber)
                           .setCemi(linkFrame)
                           .create();
    SequenceNumber = SequenceNumber +1;
    qDebug()<<"cemi"<<linkFrame<<"SequenceNumber:"<<SequenceNumber;
    qDebug()<<"tunnelRequest"<<proxy.bytes().toByteArray().toHex()<<"\n";

    if (!udpSocket) {
        udpSocket = new QUdpSocket(this);
    }
    if (udpSocket->state() != QAbstractSocket::BoundState) {
        bool flag = udpSocket->bind(QHostAddress(ui->comboBox_2->currentText()), 3671,
                                    QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        if (!flag) {
            qDebug() << "绑定失败：" << udpSocket->errorString();
            return; // 绑定失败，直接退出
        }
        qDebug() << "绑定成功，本地端口：" << udpSocket->localPort();
    }

    // udpSocket->bind(QHostAddress(ui->comboBox_2->currentText()),3671,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    // udpSocket->bind(QHostAddress::AnyIPv4,3671,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);

    bool flag = udpSocket->writeDatagram(proxy.bytes().toByteArray(),QHostAddress(serversIP.value(0)),3671);
    if(!flag){
        qDebug()<<"写失败";
    }

}
void MainWindow::oreadyread(){
    qDebug()<<"####Feature Info :this is MainWindow::oreadyread!!!!!";


    if (udpSocket->state() != QAbstractSocket::BoundState) {
        qDebug() << "错误：Socket 未绑定，无法读取数据。状态：" << udpSocket->state();
        return;
    }

    // 2. 标准的数据读取循环（防止多包丢失）
    while (udpSocket->hasPendingDatagrams()) {
        // 获取当前待读数据包的大小
        qint64 datagramSize = udpSocket->pendingDatagramSize();

        // 3. 非 KNX 数据或空包过滤
        // KNXnet/IP 最小帧头至少是 6 字节，加上连接头也至少 10 字节左右
        if (datagramSize <= 0 || datagramSize < 6) {
            // 丢弃无效大小的包
            udpSocket->readDatagram(nullptr, 0);
            continue;
        }

        // 分配缓冲区并读取
        QByteArray buffer;
        buffer.resize(datagramSize);
        udpSocket->readDatagram(buffer.data(), buffer.size());

        // 4. 尝试解析为 KNX Net/IP 帧
        // 注意：这里使用 QKnxByteArray::fromByteArray 转换，确保类型一致
        const auto byte_buffer = QKnxByteArray::fromByteArray(buffer);

        // 尝试解析，如果数据损坏，Qt 可能会打印警告
        const auto frame = QKnxNetIpFrame::fromBytes(byte_buffer);

        qDebug() << "收到 KNX 帧，原始数据 (Hex):" << frame.bytes().toHex();
        const auto service_type = frame.serviceType();
        switch (service_type){
        case QKnxNetIp::ServiceType::TunnelingRequest:{
            qDebug()<<"TunnelingRequest-0x0420-              ";
            const auto proxy_request = QKnxNetIpTunnelingRequestProxy(frame);
            qDebug()<<"___________________________";
            qDebug()<<"ACK"<<frame.bytes().toHex()<<proxy_request.cemi().tpdu()<<proxy_request.cemi().messageCode();
            const auto cemi = proxy_request.cemi();
            qDebug()<<"@@@@@@@@@@@@@@@@@@@@";
            qDebug()<<cemi.additionalInfos()<<cemi.additionalInfosSize()<<cemi.controlField()<<cemi.destinationAddress()<<cemi.extendedControlField();
            const auto data_channel = proxy_request.channelId();
            const auto data_seq = proxy_request.sequenceNumber();
            emit tunnelconnectRequest(data_channel,data_seq,QHostAddress(serversIP.value(0)),3671);

            break;
        }
        case QKnxNetIp::ServiceType::TunnelingAcknowledge:{
            qDebug()<<"TunnelingAcknowledge-0x0421-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureGet:{
            qDebug()<<"TunnelingFeatureGet-0x0422-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureResponse:{
            qDebug()<<"TunnelingFeatureResponse-0x0423-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureSet:{
            qDebug()<<"TunnelingFeatureSet-0x0424-              ";
            break;
        }
        case QKnxNetIp::ServiceType::TunnelingFeatureInfo:{
            qDebug()<<"TunnelingFeatureInfo-0x0425-              ";
            break;
        }
        case QKnxNetIp::ServiceType::RoutingIndication:{
            qDebug()<<"RoutingIndication-0x0530-              ";
            break;
        }
        default:{
            qDebug()<<"未知的报文头";
            qDebug()<<service_type<<frame;
        }
        }

        // 5. (可选) 进一步解析头部信息
        // 例如，判断消息类型是否是我们关心的 TUNNELLING_REQUEST
        // const auto header = QKnxNetIpFrameHeader::fromBytes(byte);
        // qDebug() << "消息类型:" << header.serviceType();
    }

}
void MainWindow::disConnectAcked(){
    qDebug()<<"####Feature Info :this is MainWindow::disConnectAcked!!!!!";
    const auto proxy = QKnxNetIpDisconnectResponseProxy::Builder()
                           .setChannelId(m_channelID)
                           .setStatus(QKnxNetIp::Error::None)
                           .create();
    m_udpSocket->writeDatagram(proxy.bytes().toByteArray(),QHostAddress(serversIP.value(0)),3671);
}
void MainWindow::tunnelRequestAck(quint8 channelID,quint8 seqnumber,QHostAddress address,quint16 port){
    qDebug()<<"tunnelRequestAck()";
    const auto proxy_ack = QKnxNetIpTunnelingAcknowledgeProxy::builder()
                               .setChannelId(channelID)
                               .setSequenceNumber(seqnumber)
                               .setStatus(QKnxNetIp::Error::None)
                               .create();
    bool flag_udp = udpSocket->writeDatagram(proxy_ack.bytes().toByteArray(),address,port);
    qDebug()<<proxy_ack.bytes().toByteArray().toHex()<<"shuchubaowen:"<<flag_udp;
    qDebug()<<"port"<<port<<"address"<<address<<udpSocket->localAddress();
}












*/