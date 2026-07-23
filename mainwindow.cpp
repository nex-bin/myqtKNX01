#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUdpSocket>
#include <qnewbytearray.h>

#include <QNetworkDatagram>   //需要有这个头，否则m_udpSocket->receiveDatagram();报错

#include <netipconnectionheader.h>
#include <netipframe.h>

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
    ui->comboBox->addItem("TCP client");
    ui->comboBox->addItem("TCP server");
    ui->comboBox->addItem("UDP server");

    ui->lineEdit->setText("192.168.0.118");
    ui->lineEdit_2->setText("3671");

    m_udpSocket = new QUdpSocket(this);
    connect(ui->pushButton,&QPushButton::clicked,this,&MainWindow::KNXIPconnect);
}
void MainWindow::KNXIPconnect(){
    qDebug()<<"clicked button";

    if (m_udpSocket->bind(QHostAddress::AnyIPv4, 3671)) {
        qDebug() << "UDP 端口绑定成功";
        // 连接 readyRead 信号，当有数据到达时会被触发
        connect(m_udpSocket, &QUdpSocket::readyRead,
                this, &MainWindow::onReadyRead);

        m_udpSocket->joinMulticastGroup(QHostAddress("224.0.23.12"));
    } else {
        qDebug() << "UDP 端口绑定失败";
    }
}
void MainWindow::onReadyRead(){
    qDebug()<<"you shuju laile\n";

    while (m_udpSocket->hasPendingDatagrams()) {
        qDebug()<<"m_udpSocket->hasPendingDatagrams()";
        // qDebug()<<m_udpSocket->receiveDatagram();
        const auto datagram = m_udpSocket->receiveDatagram();
        qDebug() << typeid(datagram).name()<<typeid(datagram.data()).name();
        qDebug() <<"datagram is : "<< datagram.data();


        const auto data = QKnxByteArray::fromByteArray(datagram.data());
        qDebug() << "data type is : "<< typeid(data).name();
        qDebug() << "data is : "<< data.data();




        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        qDebug() << "header type is : "<< typeid(header).name();
        qDebug() << "ipFrame header is : "<<header;
        const auto headerservice = header.serviceType();
        qDebug() << "headerservice is : "<< typeid(headerservice).name();
        qDebug() << "header.serviceType is : "<<headerservice;
        qDebug() << "this is  : "<<QKnxNetIp::ServiceType::SearchResponse;
        qDebug() << "this is  : "<<QKnxNetIp::ServiceType::ExtendedSearchResponse;

        if (!header.isValid())
            continue;

        if (header.serviceType() != QKnxNetIp::ServiceType::SearchResponse
            && header.serviceType() != QKnxNetIp::ServiceType::ExtendedSearchResponse) {
            qDebug()<<"is not equal.so continue";
            continue;
        }

        const auto frame = QKnxNetIpFrame::fromBytes(data);
        qDebug() << "frame type is : "<<typeid(frame).name();
        qDebug() << "frame is : "<<frame;

            //     const auto proxy = QKnxNetIpSearchResponseProxy(frame);
            //     if (!proxy.isValid())
            //         continue;

            // if (m_config.DiscoveryCoreV1 && !proxy.isExtended()) {
            //     emit deviceDiscovered(
            //         {
            //             (m_config.Nat ? QKnxNetIpHpaiProxy::builder()
            //                                 .setHostAddress(datagram.senderAddress())
            //                                 .setPort(datagram.senderPort()).create()
            //                           : proxy.controlEndpoint()),
            //                 proxy.deviceHardware(),
            //                 proxy.supportedFamilies(),
            //                 m_address, m_iface
            //         }
            //         );
            // }

    }

}

























































// void MainWindow::init(){
//     if (m_state != QKnxNetIpServerDiscoveryAgent::State::NotRunning)
//         return;

//     setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Starting);

//     auto isIPv4 = true;
//     m_address.toIPv4Address(&isIPv4);
//     if (!isIPv4) {
//         emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::NotIPv4,
//                            QKnxNetIpServerDiscoveryAgent::tr("Only IPv4 local address supported."));
//         return finish();
//     }

//     m_socket = new QUdpSocket;
//     connect(m_socket, &QUdpSocket::readyRead, this, &Discoverer::onReadyRead);
//     using overload = void (QUdpSocket::*)(QUdpSocket::SocketError);
//     connect(m_socket, static_cast<overload>(&QUdpSocket::error), this, &Discoverer::onError);

//     bool bound = m_socket->bind(m_config.Multicast ? QHostAddress::AnyIPv4
//                                                    : m_address,
//                                 m_config.Multicast ? 3671 : 0,
//                                 QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
//     if (!bound)
//         return finish();

//     setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Running);

//     if (m_config.Multicast) {
//         m_socket->setMulticastInterface(m_iface);
//         if (!m_socket->joinMulticastGroup(m_multicast, m_iface)) {
//             emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
//                                QKnxNetIpServerDiscoveryAgent::tr("Could not join multicast group."));
//             return finish();
//         }
//         m_socket->setSocketOption(QUdpSocket::SocketOption::MulticastTtlOption, m_config.Ttl);
//     }

//     m_timer = new QTimer;
//     m_timer->setSingleShot(true);
//     connect(m_timer, &QTimer::timeout, this, &Discoverer::onTimeout);

//     if (m_config.DiscoveryCoreV1) {
//         const auto frame = QKnxNetIpSearchRequestProxy::builder()
//         .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::Builder()
//                                   .setPort(m_config.Nat ? 0 : (m_config.Multicast ? 3671 : m_socket->localPort()))
//                                   .setHostAddress(m_config.Nat ? QHostAddress::AnyIPv4
//                                                                : (m_config.Multicast ? m_multicast : m_address))
//                                   .create())
//             .create();

//         if (frame.isValid())
//             m_socket->writeDatagram(frame.bytes().toByteArray(), m_multicast, 3671);
//     }

//     if (m_config.DiscoveryCoreV2) {
//         const auto frame = QKnxNetIpSearchRequestProxy::extendedBuilder()
//         .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::Builder()
//                                   .setPort(m_config.Nat ? 0 : (m_config.Multicast ? 3671 : m_socket->localPort()))
//                                   .setHostAddress(m_config.Nat ? QHostAddress::AnyIPv4
//                                                                : (m_config.Multicast ? m_multicast : m_address))
//                                   .create())
//             .setExtendedParameters(m_config.ExtendedSearchParameters)
//             .create();
//         if (frame.isValid())
//             m_socket->writeDatagram(frame.bytes().toByteArray(), m_multicast, 3671);
//     }

//     m_timer->start(m_config.Timeout);
// }


/* void Discoverer::start()
{
    if (m_state != QKnxNetIpServerDiscoveryAgent::State::NotRunning)
        return;

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Starting);

    auto isIPv4 = true;
    m_address.toIPv4Address(&isIPv4);     //toIPV4是QTnetwork里面的方法。返回值
    if (!isIPv4) {
        emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::NotIPv4,
                           QKnxNetIpServerDiscoveryAgent::tr("Only IPv4 local address supported."));
        return finish();
    }

    m_socket = new QUdpSocket;
    connect(m_socket, &QUdpSocket::readyRead, this, &Discoverer::onReadyRead);
    using overload = void (QUdpSocket::*)(QUdpSocket::SocketError);
    connect(m_socket, static_cast<overload>(&QUdpSocket::error), this, &Discoverer::onError);

    bool bound = m_socket->bind(m_config.Multicast ? QHostAddress::AnyIPv4
                                                   : m_address,
                                m_config.Multicast ? 3671 : 0,
                                QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    if (!bound)
        return finish();

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Running);

    if (m_config.Multicast) {
        m_socket->setMulticastInterface(m_iface);
        if (!m_socket->joinMulticastGroup(m_multicast, m_iface)) {
            emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
                               QKnxNetIpServerDiscoveryAgent::tr("Could not join multicast group."));
            return finish();
        }
        m_socket->setSocketOption(QUdpSocket::SocketOption::MulticastTtlOption, m_config.Ttl);
    }

    m_timer = new QTimer;
    m_timer->setSingleShot(true);
    connect(m_timer, &QTimer::timeout, this, &Discoverer::onTimeout);

    if (m_config.DiscoveryCoreV1) {
        const auto frame = QKnxNetIpSearchRequestProxy::builder()
        .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::Builder()
                                  .setPort(m_config.Nat ? 0 : (m_config.Multicast ? 3671 : m_socket->localPort()))
                                  .setHostAddress(m_config.Nat ? QHostAddress::AnyIPv4
                                                               : (m_config.Multicast ? m_multicast : m_address))
                                  .create())
            .create();

        if (frame.isValid())
            m_socket->writeDatagram(frame.bytes().toByteArray(), m_multicast, 3671);
    }

    if (m_config.DiscoveryCoreV2) {
        const auto frame = QKnxNetIpSearchRequestProxy::extendedBuilder()
        .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::Builder()
                                  .setPort(m_config.Nat ? 0 : (m_config.Multicast ? 3671 : m_socket->localPort()))
                                  .setHostAddress(m_config.Nat ? QHostAddress::AnyIPv4
                                                               : (m_config.Multicast ? m_multicast : m_address))
                                  .create())
            .setExtendedParameters(m_config.ExtendedSearchParameters)
            .create();
        if (frame.isValid())
            m_socket->writeDatagram(frame.bytes().toByteArray(), m_multicast, 3671);
    }

    m_timer->start(m_config.Timeout);
 }














while (m_socket->hasPendingDatagrams()) {
        if (m_state != QKnxNetIpServerDiscoveryAgent::State::Running)
            break;

        const auto datagram = m_socket->receiveDatagram();
        const auto data = QKnxByteArray::fromByteArray(datagram.data());
        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        if (!header.isValid())
            continue;

        if (header.serviceType() != QKnxNetIp::ServiceType::SearchResponse
            && header.serviceType() != QKnxNetIp::ServiceType::ExtendedSearchResponse) {
            continue;
        }

        const auto frame = QKnxNetIpFrame::fromBytes(data);
        const auto proxy = QKnxNetIpSearchResponseProxy(frame);
        if (!proxy.isValid())
            continue;

        if (m_config.DiscoveryCoreV1 && !proxy.isExtended()) {
            emit deviceDiscovered(
                {
                    (m_config.Nat ? QKnxNetIpHpaiProxy::builder()
                                        .setHostAddress(datagram.senderAddress())
                                        .setPort(datagram.senderPort()).create()
                                  : proxy.controlEndpoint()),
                    proxy.deviceHardware(),
                    proxy.supportedFamilies(),
                    m_address, m_iface
                }
            );
        }

        if (m_config.DiscoveryCoreV2 && proxy.isExtended()) {
            const auto optionalDibs = proxy.optionalDibs();
            emit deviceDiscovered(
                {
                    (m_config.Nat ? QKnxNetIpHpaiProxy::builder()
                                        .setHostAddress(datagram.senderAddress())
                                        .setPort(datagram.senderPort()).create()
                                  : proxy.controlEndpoint()),
                    proxy.deviceHardware(),
                    proxy.supportedFamilies(),
                    m_address, m_iface,
                    [&optionalDibs]() -> QKnxNetIpDib {
                        for (const auto &dib : qAsConst(optionalDibs)) {
                            if (dib.code() == QKnxNetIp::DescriptionType::TunnelingInfo)
                                return dib;
                        }
                        return {};
                    }(),
                    [&optionalDibs]() -> QKnxNetIpDib {
                        for (const auto &dib : qAsConst(optionalDibs)) {
                            if (dib.code() == QKnxNetIp::DescriptionType::ExtendedDeviceInfo)
                                return dib;
                        }
                        return {};
                    }()
                }
            );
        }
    }
}















*/