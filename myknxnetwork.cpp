#include "myknxnetwork.h"


myKNXnetwork::myKNXnetwork(QObject * parent)
    :QObject(parent)
    ,m_udpSocket(new QUdpSocket(this))
    ,udpSocket(new QUdpSocket(this))
{

    m_ackManager = new PendingAckManager(this);
    timerStatus_init();
    connect(&m_udpSocket,&QUdpSocket::readyRead,this,&myKNXnetwork::listenKNXManageMessage);
    connect(&udpSocket,&QUdpSocket::readyRead,this,&myKNXnetwork::listenKNXMessage);

}
myKNXnetwork::~myKNXnetwork(){
    if (timerStatus) {
        timerStatus->stop();
        delete timerStatus; // 或者依赖父对象销毁
        timerStatus = nullptr;
    }
    if (m_udpSocket.isOpen()) {
        m_udpSocket.close();
    }
}
bool myKNXnetwork::init(){
    m_cleanup_flag = false;

}
void myKNXnetwork::setState(Appstate state){
    m_state = state;
    emit sigstate(state);
}
void myKNXnetwork::m_cleanup(){
    cleanup();
}
void myKNXnetwork::cleanup(){
    if(m_cleanup_flag){
        return;
    }
    m_cleanup_flag = true;

//清空套接字
    //离开组播地址
    if(m_udpSocket.isValid()){
        qDebug()<<"leaving multicastgroup.......";
        m_udpSocket.leaveMulticastGroup(QHostAddress("224.0.23.12"));
        m_udpSocket.disconnect();
    }
    //断开所有信号和槽的连接

    //关闭套接字
    if(m_udpSocket.isOpen()){
        qDebug()<<"m_udpSocket is closing.......";
        m_udpSocket.close();
    }

    // 1023测试，不操作定时器
    // if (!timerStatus) {
    //     return;
    // }

    // if(timerStatus && timerStatus->isActive()){
    //     qDebug()<<"timerstatus is stoping.......";
    //     timerStatus->stop();
    // }
    setState(Appstate::Idle);
    qDebug() << "网络资源清理完成";


}

void myKNXnetwork::clearMessage(){
    m_sequencenumber = 0;
    server_sequence = 0;
    connectFlag = false;

    m_channelID = 0;//用来管理通信的通道iD。
    m_area = 0;//用来管理三级地址的一级
    m_line = 0;//用来管理三级地址的二级
    m_seqaddressnumber = 0;//用来管理三级地址的三级

    if(timerStatus && timerStatus->isActive()){
        qDebug()<<"timerstatus is stoping.......";
        timerStatus->stop();
    }
}

void myKNXnetwork::m_udpSocketCheck(){
    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState){
        qWarning()<<"m_udpSocket state is wrong.maybe not be init";
        return;
    }
}
void myKNXnetwork::udpSocketCheck(){
    if(!udpSocket.isValid() || udpSocket.state() != QAbstractSocket::BoundState){
        qWarning()<<"udpSocket state is wrong.maybe not be init";
        return;
    }
}

void myKNXnetwork::onNetworkInfo(){
    //发现本地网卡和IP地址
    qDebug()<<"[myKNXnetwork::onNetworkInfo] 正在获取本地IP。。。。。";
    QList <QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    QList <QHostAddress> addresses;

    qDebug()<<"本地端口IP地址为：";
    addresses.clear();
    for(const QNetworkInterface iface : ifaces){
        for(const auto ifaceEntry : iface.addressEntries()){
            if(ifaceEntry.ip().protocol() == QAbstractSocket::IPv4Protocol && !ifaceEntry.ip().isLoopback() && !ifaceEntry.ip().isLinkLocal()){
                addresses.append(ifaceEntry.ip());
                qDebug()<<ifaceEntry.ip();
            }
        }
    }
    qDebug()<<QString("一共获取到%1个本地IP。退出。。").arg(addresses.size());
    emit sigLocalIpaddress(addresses);
}
void myKNXnetwork::onflushIpport(){
    onNetworkInfo();
}

void myKNXnetwork::onsearchrequest(QString ip){
    qDebug()<<"【myKNXnetwork::onsearchrequest】 正在发送searchrequest......";
    QHostAddress ha = QHostAddress(ip);
    qDebug()<<"本次发送请求使用的本地IP为："<<ip;


    serversinfo.clear();
    serversIPlist.clear();
    serversportlist.clear();


    if(ha.isNull()){
        qDebug()<<"输入参数 地址无效；"<<ip;
    }

    m_udpSocket.leaveMulticastGroup(QHostAddress("224.0.23.12"));
    bool flag_myudpjoin = m_udpSocket.joinMulticastGroup(QHostAddress("224.0.23.12"));
    if(!flag_myudpjoin){
        qDebug()<<"m_udpSocket 加入组播失败";
    }
    m_udpSocket.close();



    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState){
        qWarning() << "m_udpSocket套接字未绑定,正在重新绑定端口";
        bool flag_myudpbind = m_udpSocket.bind(ha,0,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
        if(!flag_myudpbind) {
            qDebug()<<"m_udpsocket 套接字绑定端口失败";
        }else{
            qDebug() << "套接字绑定成功";
        }
    }
    connect(&m_udpSocket,&QUdpSocket::readyRead,this,&myKNXnetwork::listenKNXManageMessage);

    qDebug()<<"获取的端口号为："<<m_udpSocket.localPort();



    const auto hpai = QKnxNetIpHpaiProxy::Builder()
                        .setHostAddress(ha)
                        .setHostProtocol(QKnx::NetIp::HostProtocol::UDP_IPv4)
                        .setPort(m_udpSocket.localPort())
                        .create();
    const auto parameter_extend_1 = QKnxNetIpSrpProxy::supportedFamilyBuilder()
                        //强制搜索。设置为true.之后，只有符合条件的才会响应。
                        //强制搜索。设置为false.之后，不符合条件的也可能会响应。
                        .setMandatory(false)
                        .setServiceInfo({QKnxNetIp::ServiceFamily::Core,2})
                        .create();
    const auto parmater_extend_2 = QKnxNetIpSrpProxy::programmingModeBuilder()
                        .setMandatory(true)
                        .create();
    const auto parmater_extend_3 = QKnxNetIpSrpProxy::requestDibsBuilder()
                        .setDescriptionTypes(
                            {
                                QKnxNetIp::DescriptionType::SupportedServiceFamilies,
                                QKnxNetIp::DescriptionType::ExtendedDeviceInfo,
                                QKnxNetIp::DescriptionType::DeviceInfo,
                                QKnxNetIp::DescriptionType::SecuredServices,
                                QKnxNetIp::DescriptionType::TunnelingInfo,
                                                             })
                        .setMandatory(false)
                        .create();
    QVector<QKnxNetIpSrp> srps;
    srps.append(parmater_extend_2);


    setState(Appstate::Searching);


    const auto proxy_extend = QKnxNetIpSearchRequestProxy::ExtendedBuilder()
                        .setExtendedParameters(srps)
                        .setDiscoveryEndpoint(hpai)
                        .create();
    bool flag_search_req = m_udpSocket.writeDatagram(proxy_extend.bytes().toByteArray(),QHostAddress("224.0.23.12"),3671);
    if(!flag_search_req){
        qDebug()<<"flag_search_req 0";
    }


    const auto proxy = QKnxNetIpSearchRequestProxy::Builder()
                        .setDiscoveryEndpoint(hpai)
                           .create();
    bool flag_search_sear = m_udpSocket.writeDatagram(proxy.bytes().toByteArray(),QHostAddress("224.0.23.12"),3671);
    if(!flag_search_sear){
        qDebug()<<"flag_search_sear 0";
    }
    QTimer * search_timer = new QTimer;
    search_timer->setSingleShot(true);
    connect(search_timer,&QTimer::timeout,this,&myKNXnetwork::searchrequest_ontime);

    qDebug()<<"searching  开始计时 ----- 3000ms";
    search_timer->start(configTimeout);

}
void myKNXnetwork::searchrequest_ontime(){
    qDebug()<<"search  计时结束";
    setState(Appstate::Idle);
    emit sigfindedDeviceInfo(serversinfo);
}
void myKNXnetwork::onsearchrequestextend(QString ip){

    //预计不会涉及到管理端口套接字去搜索其他类型的设备，比如搜索处于编程模式的设备。应该仅仅使用搜索全部设备。暂不实例化。
    QHostAddress hpai = QHostAddress(ip);

}

void myKNXnetwork::ondescriptionrequest(QList<QString> ipandlocalip){
    qDebug()<<"【myKNXnetwork::ondescriptionrequest】 正在发送描述请求。";

    localip = ipandlocalip.at(0);
    serverip = ipandlocalip.at(1);

    qDebug()<<"本地IP为："<<localip;
    qDebug()<<"对方IP为："<<serverip;

    if(ipandlocalip.size()!=2)return;

    const auto ha_local = QHostAddress(ipandlocalip.at(0));
    const auto hp_server = QHostAddress(ipandlocalip.at(1));

    m_udpSocket.close();
    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState )
    {
        qWarning()<<"m_udpSocket 未绑定端口，正在重新绑定。 错误标号：error0 ";
        bool flag_bind = m_udpSocket.bind(ha_local,0);
        if(!flag_bind){
            qWarning()<<"m_udpSocket 端口绑定失败。 错误标号：error1 ";
            return;
        }else{
            qDebug()<<"m_udpSocket,端口绑定成功"<<QString::number(m_udpSocket.localPort());
        }

    }
    const auto hpai = QKnxNetIpHpaiProxy::Builder()
                        .setHostAddress(m_udpSocket.localAddress())
                        .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                        .setPort(m_udpSocket.localPort())
                          .create();
    const auto proxy = QKnxNetIpDescriptionRequestProxy::Builder()
                        .setControlEndpoint(hpai)
                           .create();

    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState)
    {
        qWarning()<<"m_udpSocket未监听端口";
        return;
    }
    setState(Appstate::Descripting);

    bool flag_descript = m_udpSocket.writeDatagram(proxy.bytes().toByteArray(),hp_server,3671);
    if(!flag_descript){
        qDebug()<<"flag_descript 0";
    }
    QTimer * description_timer = new QTimer();
    description_timer->setSingleShot(true);
    connect(description_timer,&QTimer::timeout,this,&myKNXnetwork::description_ontime);
    description_timer->start(configTimeout);

    qDebug()<<"description 定时器开始 ";

}
void myKNXnetwork::description_ontime(){
    qDebug()<<"description_ontime定时器触发";
    setState(Appstate::Idle);



    emit sigDetailInfo(detailDeviceInfo);
    detailDeviceInfo.clear();
}
void myKNXnetwork::onconnectrequest(QList<QString> ipandlocalip){
    qDebug()<<"【myKNXnetwork::onconnectrequest】 正在发送连接请求。";

    times_formconnect = 4;

    if(ipandlocalip.size()!=2)return;

    const auto ha_local = QHostAddress(ipandlocalip.at(0));
    const auto hpai_server = QHostAddress(ipandlocalip.at(1));
    qDebug()<<"local ip is ： "<<ha_local;
    qDebug()<<"target ip is : "<<hpai_server;

    m_udpSocket.close();
    udpSocket.close();

    if(!udpSocket.isValid() || udpSocket.state() != QAbstractSocket::BoundState){
        qWarning()<<"udpSocket 端口绑定失败 正在重新绑定端口，错误标号：error2";
        bool flag_bind0 = udpSocket.bind(ha_local,0);
        if(flag_bind0){
            qDebug()<<"udpSocket 端口绑定成功"<<udpSocket.localPort();
        }else{
            qWarning()<<"udpSocket 端口绑定失败 错误标号：error3";
            return;
        }
    }
    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState )
    {
        qWarning()<<"m_udpSocket 未绑定端口,正在重新绑定，错误标号：error4";
        bool flag_bind = m_udpSocket.bind(ha_local,0);
        if(!flag_bind){
            qWarning()<<"m_udpSocket 端口绑定失败 错误标号：error5";
            return;
        }

    }


    const auto hpai_cont = QKnxNetIpHpaiProxy::Builder()
                            .setHostAddress(m_udpSocket.localAddress())
                            .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                            .setPort(m_udpSocket.localPort())
                            .create();
    const auto hpai_data = QKnxNetIpHpaiProxy::Builder()
                            .setHostAddress(udpSocket.localAddress())
                            .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                            .setPort(udpSocket.localPort())
                            .create();
    const auto connectInfor = QKnxNetIpCriProxy::Builder()
                            // .setConnectionType(QKnxNetIp::ConnectionType::DeviceManagement)//远程连接专用
                            .setConnectionType(QKnxNetIp::ConnectionType::Tunnel)
                            .setTunnelLayer(QKnxNetIp::TunnelLayer::Link)
                            // .setAdditionalData()
                            // .setIndividualAddress()
                                  .create();

    const auto proxy = QKnxNetIpConnectRequestProxy::Builder()
                            .setControlEndpoint(hpai_cont)
                            .setDataEndpoint(hpai_data)
                            .setRequestInformation(connectInfor)
                           .create();

    qDebug()<<"connect request frame is :"<<proxy.bytes().toByteArray().toHex();
    setState(Appstate::Connecting);


    bool flag_connreq = m_udpSocket.writeDatagram(proxy.bytes().toByteArray(),hpai_server,3671);
    if(!flag_connreq){
        qDebug()<<"flag_connreq 0";
    }

    QTimer * connectRequest_timer = new QTimer;
    connectRequest_timer->setSingleShot(true);
    connect(connectRequest_timer,&QTimer::timeout,this,&myKNXnetwork::connectrequest_ontime);
    connectRequest_timer->start(configTimeout);

    clearMessage();

}
void myKNXnetwork::connectrequest_ontime(){
    qDebug()<<"connectrequest_ontime定时器触发";
    // setState(Appstate::Idle);
    QString individua = QString::number((int)m_area)+"." + QString::number((int)m_line)+"." + QString::number((int)m_seqaddressnumber);

    emit sigconnectInfo(individua,m_channelID);

    timerStatus->setInterval(50*1000);
    timerStatus->start();
    if(timerStatus && timerStatus->isActive()){
        qDebug()<<"连接状态心跳计时开始";
        qDebug()<<timerStatus->interval();
    }


}

void myKNXnetwork::onconnectStatusReq(quint8 channelID){
    qDebug()<<"【myKNXnetwork::onconnectStatusReq】 正在查询连接状态。";


    const auto hpai = QKnxNetIpHpaiProxy::Builder()
                        .setHostAddress(m_udpSocket.localAddress())
                        .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                        .setPort(m_udpSocket.localPort())
                          .create();

    const auto frame = QKnxNetIpConnectionStateRequestProxy::Builder()
                        .setChannelId(channelID)
                        .setControlEndpoint(hpai)
                        .create();
    bool flag_connectStatus_req = m_udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);
    if(!flag_connectStatus_req){
        qDebug()<<"flag_connectStatus_req 0";
    }
}

void myKNXnetwork::timerStatus_init(){
    //连接状态问询函数。心跳 TTL timetolive
    timerStatus = new QTimer(this);
    connect(timerStatus,&QTimer::timeout,this,&myKNXnetwork::onconnectStatus_asktime);

}
void myKNXnetwork::mconnectstatus_init(){
    QTimer * timer_formconnect = new QTimer(this);
    timer_formconnect->setSingleShot(true);
    connect(timer_formconnect,&QTimer::timeout,this,&myKNXnetwork::onconnectStatus_asktime);
    timer_formconnect->start(1*1000);
    qDebug()<<"timer 次数： "<<times_formconnect;

}

// void myKNXnetwork::onconnectStatus_asktime(){
// 断线自动重连机制
//     times_formconnect = times_formconnect -1 ;
//     if(times_formconnect <= 0){
//         //表示问询次数用完，这个时候判断问询响应是否都是成功的。
//         if(m_state == Appstate::Connected){
//             //这个时候可以进行数据通信的连接。
//         }
//         // ondisconnectrequest();
//         return;
//     }
//     onconnectStatusReq(m_channelID);
//     mconnectstatus_init();
// }
void myKNXnetwork::onconnectStatus_asktime(){
    onconnectStatusReq(m_channelID);
}

void myKNXnetwork::ondisconnectrequest(){
    qDebug()<<"【myKNXnetwork::ondisconnectrequest】 正在断开连接。";
    const auto contendpoint = QKnxNetIpHpaiProxy::Builder()
                                .setHostAddress(QHostAddress(localip))
                                .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                .setPort(m_udpSocket.localPort())
                                .create();
    const auto frame = QKnxNetIpDisconnectRequestProxy::Builder()
                        .setChannelId(m_channelID)
                        .setControlEndpoint(contendpoint)
                           .create();
    if(!m_udpSocket.isValid() || m_udpSocket.state() != QAbstractSocket::BoundState){
        qWarning()<<"m_udpSocket wrong";
        return;
    }
    bool flag_write = m_udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);

    if(!flag_write){
        qWarning()<<"flag_write defalt";
        return;
    }
    clearMessage();
    m_ackManager->clearAll();
    setState(Appstate::Idle);
}
void myKNXnetwork::ondisconnectresponse(){
    qDebug()<<"【myKNXnetwork::ondisconnectresponse】 发送断开连接响应。";
    m_udpSocketCheck();

    const auto proxy = QKnxNetIpDisconnectResponseProxy::Builder()
                           .setChannelId(m_channelID)
                           .setStatus(QKnxNetIp::Error::None)
                           .create();
    bool flag_res_discon = m_udpSocket.writeDatagram(proxy.bytes().toByteArray(),QHostAddress(serverip),3671);
    if(!flag_res_discon){
        qDebug()<<"flag_res_discon 0";
    }
    clearMessage();
    m_ackManager->clearAll();

}
void myKNXnetwork::connectSuccess(){
    setState(Appstate::Connected);
}
void myKNXnetwork::connectFailure(){
    setState(Appstate::Failure);
    //清空通信的信息序列号
    clearMessage();
}


void myKNXnetwork::configConnectRequest(){
    qDebug()<<"myKNXnetwork::configConnectRequest";
    m_udpSocketCheck();
    udpSocketCheck();

    const auto controlpoint = QKnxNetIpHpaiProxy::Builder()
                                  .setHostAddress(QHostAddress(localip))
                                .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                                .setPort(m_udpSocket.localPort())
                                  .create();
    const auto datapoint = QKnxNetIpHpaiProxy::Builder()
                               .setHostAddress(QHostAddress(localip))
                               .setHostProtocol(QKnxNetIp::HostProtocol::UDP_IPv4)
                               .setPort(udpSocket.localPort())
                               .create();
    const auto cri = QKnxNetIpCriProxy::Builder()
                    .setConnectionType(QKnxNetIp::ConnectionType::DeviceManagement)
                    .setTunnelLayer(QKnxNetIp::TunnelLayer::Link)
                    .create();

    const auto frame = QKnxNetIpConnectRequestProxy::Builder()
                        .setControlEndpoint(controlpoint)
                        .setDataEndpoint(datapoint)
                        .setRequestInformation(cri)
                           .create();

    setState(Appstate::Connecting);

    bool flag_write = m_udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);
    if(!flag_write){
        qWarning()<<"m_udpSocket写错误";
    }

    QTimer * timer_config = new QTimer(this);
    timer_config->setSingleShot(true);
    connect(timer_config,&QTimer::timeout,this,&myKNXnetwork::onConfigTimerOut);
    timer_config->start(configTimeout);
    qDebug()<<"config connectrequet start";
}
void myKNXnetwork::onConfigTimerOut(){
    setState(Appstate::Idle);

}


void myKNXnetwork::listenKNXManageMessage(){
    while(m_udpSocket.hasPendingDatagrams()){

        const auto b_receive = m_udpSocket.receiveDatagram();
        const auto data = QKnxByteArray::fromByteArray(b_receive.data());
        const auto b_frame = QKnxNetIpFrame::fromBytes(data);
        const auto b_frame_header = b_frame.header();

        switch(b_frame_header.serviceType()){
            case QKnxNetIp::ServiceType::ExtendedSearchRequest:{
                const auto frame = QKnxNetIpSearchRequestProxy(b_frame);
                qDebug()<<"QKnxNetIp::ServiceType::ExtendedSearchRequest:"<<frame.extendedSearchParameters();
                break;

            };
            case QKnxNetIp::ServiceType::ExtendedSearchResponse:{
                if(m_state == Appstate::Searching){
                    const auto frame = QKnxNetIpSearchResponseProxy(b_frame);
                    qDebug()<<"QKnxNetIp::ServiceType::ExtendedSearchResponse"<<frame.controlEndpoint();
                    break;
                }

            }
            case QKnxNetIp::ServiceType::SearchRequest:{
                const auto frame = QKnxNetIpSearchRequestProxy(b_frame);
                qDebug()<<"QKnxNetIp::ServiceType::SearchRequest"<<frame.discoveryEndpoint();
                break;

            }
            case QKnxNetIp::ServiceType::SearchResponse:{
                if(m_state == Appstate::Searching){
                    const auto proxy = QKnxNetIpSearchResponseProxy(b_frame);
                    qDebug()<<"QKnxNetIp::ServiceType::SearchResponse:"<<proxy.controlEndpoint();

                    const auto hpai1 = QKnxNetIpDib::fromBytes(data,6);

                    const auto dib1 = QKnxNetIpDib::fromBytes(data,6+hpai1.bytes().size());
                    const auto dib1proxy = QKnxNetIpDeviceDibProxy(dib1);
                    const auto dib2 = QKnxNetIpDib::fromBytes(data,6+hpai1.bytes().size()+dib1.size());
                    const auto dib2proxy = QKnxNetIpDeviceDibProxy(dib2);

                    QKnxNetIpHpai hpai = proxy.controlEndpoint();
                    QKnxNetIpHpaiProxy pro = QKnxNetIpHpaiProxy(hpai);

                    qDebug()<<"响应的HPAI："<<hpai1.bytes().toByteArray().toHex();
                    qDebug()<<"响应的设备描述："<<dib1.bytes().toByteArray().toHex();

                    const auto deviceStatus = dib1proxy.deviceStatus();
                    QString deviceStatus_text = QString("%1").arg(static_cast<int>(dib1proxy.deviceStatus()));
                    QString ab = "IP地址：" + pro.hostAddress().toString()+" : "+QString::number(pro.port())+"\n"+ "编程模式：" + deviceStatus_text+"\n"+"物理地址："+dib1proxy.individualAddress().toString()+
                                 "\n"+"设备名称："+dib1proxy.deviceName()+"\n"+"mac地址："+dib1proxy.macAddress().toHex().toByteArray()+
                                 "\n"+"设备硬件编码："+dib1proxy.serialNumber().toHex().toByteArray();
                    // qDebug()<<"dib1proxy.descriptionType"<<dib1proxy.descriptionType();
                    // qDebug()<<"dib2proxy.descriptionType"<<dib2proxy.descriptionType();

                    qDebug()<<ab;

                    DeviceInfo deviceinfo;
                    deviceinfo.deviceStatus_text = ab;
                    deviceinfo.address = pro.hostAddress();
                    deviceinfo.port = pro.port();

                    if(!serversIPlist.contains(pro.hostAddress())){
                        serversIPlist.append(pro.hostAddress());
                        serversinfo.append(deviceinfo);
                    }
                }
                break;

            }
            case QKnxNetIp::ServiceType::ConnectRequest:{
                qDebug()<<"QKnxNetIp::ServiceType::ConnectRequest";

                const auto frame = QKnxNetIpConnectRequestProxy(b_frame);
                frame.controlEndpoint();
                break;
            }
            case QKnxNetIp::ServiceType::ConnectResponse:{
                qDebug()<<"QKnxNetIp::ServiceType::ConnectResponse";

                if(m_state == Appstate::Connecting){
                    const auto frame = QKnxNetIpConnectResponseProxy(b_frame);

                    const auto ad = frame.responseData().bytes().toByteArray();
                    m_area = (ad.at(2)>>4)&0x0f;
                    m_line =ad.at(2)&0x0f;
                    m_seqaddressnumber = ad.at(3);

                    m_channelID = frame.channelId();
                    qDebug()<<"m_channelID is: "<<m_channelID;
                    qDebug()<<"连接是否成功 :"<<frame.status();

                    // 离开发出连接状态问询
                    onconnectStatusReq(m_channelID);

                    // if(static_cast <int> (frame.status()) == 0){
                    //     connectSuccess();
                    //     qDebug()<<"连接状态：成功";
                    // }else{
                    //     connectFailure();
                    //     qDebug()<<"连接状态：失败";

                    // }
                }

                break;
            }
            case QKnxNetIp::ServiceType::DisconnectRequest:{
                qDebug()<<"监听到   QKnxNetIp::ServiceType::DisconnectRequest";
                const auto frame = QKnxNetIpDisconnectRequestProxy(b_frame);
                qDebug()<<"m_channelID is: "<<frame.channelId();
                const auto ha = frame.controlEndpoint();
                const auto h = QKnxNetIpHpaiProxy(ha);
                qDebug()<<h.hostAddress();
                setState(Appstate::Idle);
                ondisconnectresponse();

                m_sequencenumber = 0;
                break;
            }
            case QKnxNetIp::ServiceType::DisconnectResponse:{
                qDebug()<<"QKnxNetIp::ServiceType::DisconnectResponse";

                const auto frame = QKnxNetIpDisconnectResponseProxy(b_frame);
                qDebug()<<"disconnect channelid"<<frame.channelId();
                qDebug()<<"disconnect status"<<frame.status();
                setState(Appstate::Idle);
                break;
            }
            case QKnxNetIp::ServiceType::ConnectionStateRequest:{
                const auto frame = QKnxNetIpConnectionStateRequestProxy(b_frame);
                break;
            }
            case QKnxNetIp::ServiceType::ConnectionStateResponse:{
                qDebug()<<"收到了报文  QKnxNetIp::ServiceType::ConnectionStateResponse";
                const auto frame = QKnxNetIpConnectionStateResponseProxy(b_frame);


                if(static_cast <int> (frame.status()) == 0){
                    connectSuccess();
                    qDebug()<<"连接状态：成功";
                }else{
                    connectFailure();
                    qDebug()<<"连接状态：失败";

                }

                break;
            }
            case QKnxNetIp::ServiceType::DescriptionRequest:{

                const auto frame = QKnxNetIpDescriptionRequestProxy(b_frame);
                break;
            }
            case QKnxNetIp::ServiceType::DescriptionResponse:{
                qDebug()<<" listen DescriptionResponse******";

                if(m_state == Appstate::Descripting){
                    const auto frame = QKnxNetIpDescriptionResponseProxy(b_frame);
                    const auto device = frame.deviceHardware();
                    const auto deviceproxy = QKnxNetIpDeviceDibProxy(device);

                    if(!deviceproxy.isValid()){
                        qDebug()<<"deviceproxy is wrong";
                        return;
                    }

                    // deviceproxy.descriptionType();
                    // deviceproxy.mediumStatus();
                    // deviceproxy.macAddress();
                    // deviceproxy.projectInstallationId();
                    // deviceproxy.serialNumber();
                    // deviceproxy.multicastAddress();
                    QString name = "设备名称："+deviceproxy.deviceName()+"    ";
                    QString mode;
                    if(static_cast <int>(deviceproxy.deviceStatus())){
                        mode = "编程模式：是    ";
                    }else{
                        mode = "编程模式：否    ";
                    }
                    QString type;
                    switch(deviceproxy.mediumStatus()){
                    case QKnxNetIp::MediumStatus::TP:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::TP)<<"tp";
                        type = "连接类型：TP线   ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::TP0:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::TP0)<<"TP0";
                        type = "连接类型：TP线    ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::RF:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::RF)<<"RF";
                        type = "连接类型：RF    ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::PL132:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::PL132)<<"PL132";
                        type = "连接类型：132电力线    ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::PL:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::PL)<<"PL";
                        type = "连接类型：110电力线    ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::NetIP:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::NetIP)<<"NetIP";
                        type = "连接类型：网线    ";
                        break;
                    }
                    case QKnxNetIp::MediumStatus::Unknown:{
                        // qDebug()<<static_cast <int>(QKnxNetIp::MediumStatus::Unknown)<<"Unknown";
                        type = "连接类型：未知的类型    ";
                        break;
                    }
                    default:
                        qWarning()<<"未知的连接类型    ";

                    }

                    QString mac;
                    qDebug()<<"mac地址:"<<deviceproxy.macAddress().toHex();
                    mac = "mac地址:"+deviceproxy.macAddress().toByteArray().toHex().toUpper()+"    ";
                    QString projectId;
                    quint16 id = deviceproxy.projectInstallationId();
                    quint16 install = id & 0x000f;
                    quint16 project = id >> 4;
                    projectId = "绑定项目ID:"+QString("%1:%2").arg(project).arg(install)+"    ";
                    QString serial;
                    qDebug()<<"设备序列号: "<<deviceproxy.serialNumber().toHex();
                    serial = "设备序列号:"+deviceproxy.serialNumber().toByteArray().toHex()+"    ";
                    QString multicast;
                    multicast = "多播地址："+static_cast<QString>(deviceproxy.multicastAddress().toString())+"    ";

                    QString ad = name+mode+type+mac+projectId+serial+multicast;


                    //支持的服务类型
                    const auto dib = frame.supportedFamilies();
                    const auto sfproxy = QKnxNetIpServiceFamiliesDibProxy(dib);
                    QString supportservers;
                    const auto services = sfproxy.serviceInfos();
                    sfproxy.descriptionType();
                    sfproxy.isValid();
                    for (const auto &info : sfproxy.serviceInfos()) {
                        // info.ServiceFamily 是服务家族 ID
                        // info.Version 是版本号

                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::Security) {
                            qDebug() << "设备支持 Security:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持安全 " + "version:"+QString::number(info.ServiceFamilyVersion)+"    ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::IpRouting) {
                            qDebug() << "设备支持 Routing，版本:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持IP路由 "+ "version:"+QString::number(info.ServiceFamilyVersion)+"    ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::IpTunneling) {
                            qDebug() << "设备支持 tunneling:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持IP隧道  ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::Core) {
                            qDebug() << "设备支持 core:" << info.ServiceFamilyVersion<<static_cast<int>(QKnxNetIp::ServiceFamily::Core);
                            supportservers = supportservers + "支持core  ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::DeviceManagement) {
                            qDebug() << "设备支持 DeviceManagement:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持设备管理  ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::RemoteLogging) {
                            qDebug() << "设备支持 RemoteLogging:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持远程日志  ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::RemoteConfigAndDiagnosis) {
                            qDebug() << "设备支持 RemoteConfigAndDiagnosis:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持远程调试  ";
                        }
                        if (info.ServiceFamily == QKnxNetIp::ServiceFamily::ObjectServer) {
                            qDebug() << "设备支持 ObjectServer:" << info.ServiceFamilyVersion;
                            supportservers = supportservers + "支持对象服务  ";

                        }
                    }

                    detailDeviceInfo = ad+"\n"+supportservers;



                    //附加字段不是必须有的

                    const auto options = frame.optionalDibs();
                    if(!options.isEmpty()){
                        const auto optionaldibs = frame.optionalDibs();
                        if(!optionaldibs.isEmpty()){
                            qDebug()<<"optionaldibs is not empty";
                            for(const auto &optionaldib:optionaldibs){
                                // qDebug()<<optionaldib.bytes().size();
                                const auto dibproxy = QKnxNetIpDib(optionaldib);
                                if(dibproxy.size()){
                                    // qDebug()<<"附加字段："<<dibproxy.bytes().toHex();
                                    QByteArray data = dibproxy.bytes().toByteArray();   // 先转成 QByteArray
                                    for (int i = 0; i < data.size(); ++i) {
                                        quint8 byte = static_cast<quint8>(data.at(i));
                                        // qDebug() << QString("字节[%1]: 0x%2").arg(i).arg(byte, 2, 16, QChar('0')).toUpper();
                                    }

                                    if(dibproxy.bytes().size()>4){
                                        QByteArray da = frame.optionalDibs().data()->bytes().toByteArray().mid(2,2);
                                        QString manufactory = "厂商编码："+da.toHex()+"    ";


                                        detailDeviceInfo = ad+"\n"+supportservers+"\n"+manufactory;

                                        // qDebug()<<"dib : "<<dib<<"services "<<services.data();
                                    }

                                }
                            }
                        }

                    }else{
                        qDebug()<<"optionaldib is empty";
                        return;
                    }



                }

                break;
            }
            default:{

            }
        }

    }
}

void myKNXnetwork::tunnelRequest_KNXWrite(QString Gaddress,QString data){
    qDebug()<<"myKNXnetwork::tunnelRequest";
    qDebug()<<Gaddress<<"send value"<<data;
    QByteArray m_data = QByteArray::fromHex(data.toLatin1());
    QKnxByteArray messagedata = QKnxByteArray::fromByteArray(m_data);


    udpSocketCheck();
    const auto tpci = QKnxTpdu::TransportControlField::DataGroup;
    const auto apci = QKnxTpdu::ApplicationControlField::GroupValueWrite;


    const auto tpdu = QKnxTpdu(tpci,apci,messagedata);


    QString individualAddress = QString::number(m_area)+"."+QString::number(m_line)+"."+QString::number(m_seqaddressnumber);
    QKnxLinkLayerFrame cemi = QKnxLinkLayerFrame::builder()
                                       .setDestinationAddress({ QKnxAddress::Type::Group, QLatin1String(Gaddress.toLatin1()) }) // 示例组地址
                                       // .setSourceAddress(QKnxAddress(QKnxAddress::Type::Group,241)) // 设置源地址
                                       .setSourceAddress({ QKnxAddress::Type::Individual, QLatin1String(individualAddress.toLatin1()) })
                                       // ... 设置其他cEMI参数
                                       .setTpdu(tpdu)


                                       .createFrame();

    const auto frame = QKnxNetIpTunnelingRequestProxy::Builder()
                        .setSequenceNumber(m_sequencenumber)
                        .setCemi(cemi)
                        .setChannelId(m_channelID)
                           .create();
    bool flag_write = udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);
    if(!flag_write){
        qWarning()<<"udpSocket 写失败";
    }


    m_ackManager->addPending(m_sequencenumber,frame.bytes().toByteArray());
    m_sequencenumber =  m_sequencenumber + 1;


}
void myKNXnetwork::tunnelRequest_KNXRead(QString Gaddress){
    qDebug()<<"myKNXnetwork::tunnelRequest";

    udpSocketCheck();

    QKnxLinkLayerFrame cemi = QKnxLinkLayerFrame::builder()
                                  .setDestinationAddress({ QKnxAddress::Type::Group, QLatin1String(Gaddress.toLatin1()) }) // 示例组地址
                                  // .setSourceAddress(QKnxAddress(QKnxAddress::Type::Group,241)) // 设置源地址
                                  .createFrame();

    const auto frame = QKnxNetIpTunnelingRequestProxy::Builder()
                           .setSequenceNumber(m_sequencenumber)
                           .setCemi(cemi)
                           .setChannelId(m_channelID)
                           .create();
    bool flag_write = udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);

    m_ackManager->addPending(m_sequencenumber,frame.bytes().toByteArray());
    m_sequencenumber = m_sequencenumber + 1;

    if(!flag_write){
        qWarning()<<"udpSocket 写失败";
    }

}
void myKNXnetwork::tunnelRequest_KNXRead_tpdu(QString Gaddress){
    qDebug()<<"myKNXnetwork::tunnelRequest";

    udpSocketCheck();
    const auto tpci = QKnxTpdu::TransportControlField::DataGroup;
    const auto apci = QKnxTpdu::ApplicationControlField::GroupValueWrite;


    const auto tpdu = QKnxTpdu(tpci,apci,QKnxByteArray(1, 0x00));

    QKnxLinkLayerFrame cemi = QKnxLinkLayerFrame::builder()
                                  .setDestinationAddress({ QKnxAddress::Type::Group, QLatin1String(Gaddress.toLatin1()) }) // 示例组地址
                                  // .setSourceAddress(QKnxAddress(QKnxAddress::Type::Group,241)) // 设置源地址
                                  .setTpdu(tpdu)
                                  .createFrame();

    const auto frame = QKnxNetIpTunnelingRequestProxy::Builder()
                           .setSequenceNumber(m_sequencenumber)
                           .setCemi(cemi)
                           .setChannelId(m_channelID)
                           .create();
    bool flag_write = udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);

    m_ackManager->addPending(m_sequencenumber,frame.bytes().toByteArray());
    m_sequencenumber = m_sequencenumber + 1;

    if(!flag_write){
        qWarning()<<"udpSocket 写失败";
    }

}



void myKNXnetwork::tunnelAcknowlage(quint8 sequence){
    qDebug()<<"*******************myKNXnetwork::tunnelAcknowlage";
    const auto frame = QKnxNetIpTunnelingAcknowledgeProxy::Builder()
                           .setChannelId(m_channelID)
                           .setSequenceNumber(sequence)
                           .setStatus(QKnxNetIp::Error::None)
                           .create();
    udpSocketCheck();
    bool flag_write = udpSocket.writeDatagram(frame.bytes().toByteArray(),QHostAddress(serverip),3671);
    if(!flag_write){
        qWarning()<<"udpsocket write wrong";
    }
}

void myKNXnetwork::listenKNXMessage(){
    while (udpSocket.hasPendingDatagrams()) {
        const auto datagram = udpSocket.receiveDatagram();
        const auto datagram_bytearray = QKnxByteArray::fromByteArray(datagram.data());
        const auto frame_b = QKnxNetIpFrame::fromBytes(datagram_bytearray);
        const auto frame_header = frame_b.header();

        switch(frame_header.serviceType()){
            case QKnxNetIp::ServiceType::TunnelingRequest:{
                qDebug()<<"TunnelingRequest-0x0420-              ";

                const auto frame = QKnxNetIpTunnelingRequestProxy(frame_b);
                KnxBusRecord record ;

                const auto cemi = frame.cemi();
                const auto sourceAddress =cemi.sourceAddress().toString();
                QKnxAddress source(QKnxAddress::Type::Individual,sourceAddress);
                const auto destinationAddre =  cemi.destinationAddress().toString();
                QKnxAddress destion;
                QKnxLinkLayerFrame cemiFrame = frame.cemi();
                if(QKnxExtendedControlField(cemiFrame.extendedControlField()).destinationAddressType() == QKnxAddress::Type::Group)
                {
                    destion =QKnxAddress(QKnxAddress::Type::Group,destinationAddre);
                    record.isGroupAddress = true;
                }else{
                    destion = QKnxAddress(QKnxAddress::Type::Individual,destinationAddre);
                    record.isGroupAddress = false;
                }
                if(cemiFrame.controlField().frameFormat() == QKnxControlField::FrameFormat::Standard){
                    record.isStandardFormat = true;
                }else{
                    record.isStandardFormat = false;
                }

                record.timestamp = QDateTime::currentDateTime();
                record.sourceAddress = source.toString();
                record.destAddress = destion.toString();
                record.messageCode;
                switch(cemi.messageCode()){
                    case QKnxLinkLayerFrame::MessageCode::DataIndication:{
                        record.messageCode = "DataIndication";
                        break;
                    }
                    case QKnxLinkLayerFrame::MessageCode::DataRequest :{
                        record.messageCode = "DataRequest";
                        break;
                    }
                    case QKnxLinkLayerFrame::MessageCode::DataConfirmation  :{
                        record.messageCode = "DataConfirmation";
                        break;
                    }
                        default:{
                            record.messageCode = "ManageMessageCode;";
                            break;
                        }
                }

                record.hopCount = QKnxExtendedControlField(cemiFrame.extendedControlField()).hopCount();
                record.channelID = frame.channelId();
                record.sequenceNumber = frame.sequenceNumber();


                const auto tpdu = cemi.tpdu();
                qDebug()<<"tpdu.size"<<QKnxTpdu(tpdu).size()<<tpdu.bytes().toHex()<<tpdu.data().isEmpty();

                switch (tpdu.applicationControlField()) {
                case QKnxTpdu::ApplicationControlField::GroupValueRead:
                    record.dataType = "GroupValueRead";
                    break;
                case QKnxTpdu::ApplicationControlField::GroupValueResponse:
                    record.dataType = "GroupValueResponse";
                    break;
                case QKnxTpdu::ApplicationControlField::GroupValueWrite:
                    record.dataType = "GroupValueWrite";
                    if(QKnxTpdu(tpdu).size() == 2){
                        //这里粗略把1bit和4bit合并，都是1bit,暂不考虑DPT表的事情。


                        record.datalength = KNXDPT::DPT_1bit;
                    }else if(QKnxTpdu(tpdu).size()==3){
                            record.datalength = KNXDPT::DPT_1byte;
                        }else if(QKnxTpdu(tpdu).size()==4){
                        record.datalength = KNXDPT::DPT_2bytes;
                    }else if(QKnxTpdu(tpdu).size()==6){
                            record.datalength = KNXDPT::DPT_4bytes;
                        }else if(QKnxTpdu(tpdu).size()==10){
                        record.datalength = KNXDPT::DPT_8bytes;
                    }else if(QKnxTpdu(tpdu).size()==16){
                            record.datalength = KNXDPT::DPT_14bytes;
                        }
                    break;
                default:
                    record.dataType = "GroupValue";
                    break;
                }
                record.dataValue = tpdu.data().toByteArray();
                record.dataHex = tpdu.data().toByteArray().toHex(' ').toUpper();

                emit sigKNXBusMessageReceived(record);
                qDebug()<<"cemi"<<cemi<<"tpdu长度"<<tpdu.data().size()<<tpdu.data().toHex();
                tunnelAcknowlage(frame.sequenceNumber());
                break;
            }
            case QKnxNetIp::ServiceType::TunnelingAcknowledge:{
                qDebug()<<"TunnelingAcknowledge-0x0421-              ";
                const auto frame = QKnxNetIpTunnelingAcknowledgeProxy(frame_b);

                quint8 sequence = frame.sequenceNumber();
                if( frame.channelId()==m_channelID){

                    if(m_ackManager->contains(sequence)){
                        if(frame.status() == QKnxNetIp::Error::None){
                            emit sigValueSendState(ValueSendState::Success);
                        }
                    }else{
                        qDebug()<<"send default";
                        emit sigValueSendState(ValueSendState::Failure);
                    }
                    m_ackManager->removePending(sequence);

                }else{
                    qDebug()<<"channelis or sequencenumber is wrong";
                    qDebug()<<"内部channelID"<<m_channelID;
                    qDebug()<<"内部sequenceNUmber"<<m_sequencenumber;
                    qDebug()<<"外部channelID"<<frame.channelId();
                    qDebug()<<"外部sequenceNumber"<<frame.sequenceNumber();
                }


                break;
            }
            case QKnxNetIp::ServiceType::TunnelingFeatureGet :{
                qDebug()<<"TunnelingFeatureGet-0x0422-              ";


                break;
            }

            case QKnxNetIp::ServiceType::TunnelingFeatureResponse :{
                qDebug()<<"TunnelingFeatureResponse-0x0423-              ";


                break;
            }
            case QKnxNetIp::ServiceType::TunnelingFeatureSet :{
                qDebug()<<"TunnelingFeatureSet-0x0424-              ";


                break;
            }
            case QKnxNetIp::ServiceType::TimerNotify :{
                qDebug()<<"TimerNotify-0x0955-              ";


                break;
            }

            default:{
                qDebug()<<"其他未知的报文类型";
                qDebug()<<"servertype is : "<<frame_header.serviceType();
            }




        }
    }
}