#include "myknxdrivicepoint.h"
#include "netipservicediscoveryagent_p.h"
QT_BEGIN_NAMESPACE
myknxdrivicepoint::myknxdrivicepoint() {
    //void init();

}
/*
void myknxdrivicepoint::searchRequest(QNetworkInterface iface) {
    DiscovererConfig * m_disconfig = new DiscovererConfig();
    m_disconfig->DiscoveryCoreV1 = true;
    m_disconfig->Nat = false;
    m_disconfig->Multicast = true;

    Discoverer * m_dic = new Discoverer(m_hostaddress, iface,*m_disconfig);
    connect(m_dic,&Discoverer::deviceDiscovered,this,&myknxdrivicepoint::searchTimeout);
    m_dic->start();
}
//定时结束之后，将搜索到的设备信息转存下来。
void myknxdrivicepoint::searchTimeout(QKnxNetIpServerInfo server){
    qDebug()<<"##Feature Info : myknxdrivicepoint::searchTimeout ";

    qDebug()<<server.deviceName();
}
void myknxdrivicepoint::onReadyRead(){
    while (m_udpSocket->hasPendingDatagrams()) {
        qDebug()<<"__________ data stream is coming___________";
        if (m_state != QKnxNetIpServerDiscoveryAgent::State::Running)
            break;
        const auto datagram = m_udpSocket->receiveDatagram();
        const auto data = QKnxByteArray::fromByteArray(datagram.data());
        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        if (!header.isValid())
        {
            qDebug()<<"报文头不存在，退出";
            continue;
        }
    }
}

void myknxdrivicepoint::init(){

}
void myknxdrivicepoint::setmyhostaddress(QHostAddress address){
    m_hostaddress = address;
}
void myknxdrivicepoint::setmyiface(QNetworkInterface iface){
    m_iface = iface;
}
void myknxdrivicepoint::connectRequest(){

}

*/
QT_END_NAMESPACE