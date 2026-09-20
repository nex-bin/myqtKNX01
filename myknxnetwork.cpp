#include "myknxnetwork.h"

myKNXnetwork::myKNXnetwork(QObject *) {

}
void myKNXnetwork::getNetworkInfo(){
    //发现本地网卡和IP地址
    qDebug()<<"getNetworkInfo";
    QList <QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
    QList <QHostAddress> addresses;

    for(const QNetworkInterface iface : ifaces){
        for(const auto ifaceEntry : iface.addressEntries()){
            if(ifaceEntry.ip().protocol() == QAbstractSocket::IPv4Protocol && !ifaceEntry.ip().isLoopback() && !ifaceEntry.ip().isLinkLocal()){
                addresses.append(ifaceEntry.ip());
            }



        }
    }
    emit resLocalIpaddress(addresses);
}
void myKNXnetwork::flushIpport(){
    getNetworkInfo();

}

bool myKNXnetwork::init(QHostAddress address,qint16 port){
    QUdpSocket * m_udpSocket = new QUdpSocket(this);
    bool flag = m_udpSocket->bind(address,port,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if(!flag){
        qDebug()<<"myknxnetwork"<<"init"<<"flag"<<flag<<"端口绑定失败";
    }
    bool flag_join = m_udpSocket->joinMulticastGroup(QHostAddress("224.0.23.12"));
    if(!flag_join){
        qDebug()<<"myknxnetwork"<<"init"<<"flag_join"<<flag_join<<"加入组播失败";
    }
    return flag && flag_join;
}
void myKNXnetwork::sendsearchrequest(QString ip){
    QHostAddress ha = QHostAddress(ip);

    if(ha.isNull()){
        qDebug()<<"输入参数 地址无效；"<<ip;
    }

    bool flag_myupdbind = m_udpSocket->bind(ha,0,QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if(!flag_myupdbind) {
        qDebug()<<"m_udpsocket 套接字绑定端口失败";
    }
    bool flag_myudpjoin = m_udpSocket->joinMulticastGroup(QHostAddress("224.0.23.12"));
    if(!flag_myudpjoin){
        qDebug()<<"m_udpSocket 加入组播失败";
    }


}
void myKNXnetwork::listenKNXManageMessage(){

}