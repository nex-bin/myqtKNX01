#include "netipservicediscoveryagent.h"
#include "netipservicediscoveryagent_p.h"

#include <QtCore/qthread.h>

QT_BEGIN_NAMESPACE

namespace QKnxPrivate
{
static void clearSocket(QUdpSocket **socket)
{
    if (*socket) {
        (*socket)->disconnect();
        (*socket)->deleteLater();
        (*socket) = nullptr;
    }
}

static void clearTimer(QTimer **timer)
{
    if (*timer) {
        (*timer)->stop();
        (*timer)->disconnect();
        (*timer)->deleteLater();
        (*timer) = nullptr;
    }
}

static QVector<Adapter> adaptersForAddresses(const QVector<QHostAddress> addresses)
{
/*
     用来记录 本地所有可用 的IP地址和QNetworkInterface
*/
    QVector<Adapter> adapters;
    auto all = QNetworkInterface::allInterfaces();
    for (const auto &iface : all) {
        const auto flags = iface.flags();
        if (!flags.testFlag(QNetworkInterface::IsUp)
            && !flags.testFlag(QNetworkInterface::IsRunning)
            && !flags.testFlag(QNetworkInterface::CanMulticast)) continue;

        const auto type = iface.type();
        if (type != QNetworkInterface::Wifi
            && type != QNetworkInterface::Loopback
            && type != QNetworkInterface::Ethernet) continue;

        const auto entries = iface.addressEntries();
        for (const auto &entry : entries) {
            const auto address = entry.ip();
            if (address.protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            if (!addresses.contains(address))
                continue;
            adapters.append({ address, iface });
            break;
        }
    }
    return adapters;
}
}

Discoverer::Discoverer(const QHostAddress &address, const QNetworkInterface iface, const DiscovererConfig &c)
    : QObject(nullptr)
    , m_address(address)
    , m_iface(iface)
    , m_config(c)
{}

Discoverer::~Discoverer()
{
    finish();
}

void Discoverer::start()
{
    qDebug()<<"####Feature Info :this is Discoverer::start!!!!!";

    if (m_state != QKnxNetIpServerDiscoveryAgent::State::NotRunning)
        return;
    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Starting);

    auto isIPv4 = true;
    m_address.toIPv4Address(&isIPv4);
    if (!isIPv4) {
        emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::NotIPv4,
                           QKnxNetIpServerDiscoveryAgent::tr("Only IPv4 local address supported."));
        qDebug()<<"emit errorOccurred";
        return finish();
    }

    m_socket = new QUdpSocket;

    connect(m_socket, &QUdpSocket::readyRead, this, &Discoverer::onReadyRead);
    connect(m_socket, &QUdpSocket::errorOccurred, this, &Discoverer::onError);

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
            qDebug()<<"emit errorOccurred";
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
        .setHostAddress(m_config.Nat ? QHostAddress::AnyIPv4: (m_config.Multicast ? m_multicast : m_address))
        .create())
        .create();
        qDebug()<<m_config.Nat<<m_config.Multicast<<m_multicast;

        if (frame.isValid()){
            m_socket->writeDatagram(frame.bytes().toByteArray(), m_multicast, 3671);
            qDebug()<<"searchrequest send:"<<frame.bytes().toByteArray().toHex();
            // qDebug()<<"localaddress: "<<m_socket->localAddress()<<"localport: "<<m_socket->localPort();
            // localaddress:  QHostAddress("0.0.0.0") localport:  3671

        }
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
    qDebug()<<"####Feature Info :计时开始--------this is Discoverer::start end!!!!!";

}

void Discoverer::finish()
{
    qDebug()<<"####Feature Info :this is Discoverer::finish!!!!!";

    if (m_state == QKnxNetIpServerDiscoveryAgent::State::Stopping
        || m_state == QKnxNetIpServerDiscoveryAgent::State::NotRunning) {
        return;
    }

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Stopping);

    QKnxPrivate::clearTimer(&m_timer);

    if (m_socket) {
        //清空套接字
        qDebug()<<"---------m_socket清空----------";
        if (m_config.Multicast && m_socket->state() == QUdpSocket::BoundState) {
            m_socket->setMulticastInterface({});
            m_socket->leaveMulticastGroup(m_multicast, m_iface);
        }
        QKnxPrivate::clearSocket(&m_socket);
    }

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::NotRunning);
}

void Discoverer::onTimeout()
{
    if (m_devices.isEmpty())
    {
        qDebug()<<"-----------触发 ontimeout 信号——————————————。";
        emit timeout();
    }
    finish();
}

void Discoverer::onReadyRead()
{

    while (m_socket->hasPendingDatagrams()) {
        qDebug()<<"__________ data stream is coming___________";
        if (m_state != QKnxNetIpServerDiscoveryAgent::State::Running)
            break;
        const auto datagram = m_socket->receiveDatagram();
        const auto data = QKnxByteArray::fromByteArray(datagram.data());
        const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
        if (!header.isValid())
        {
            qDebug()<<"报文头不存在，退出";
            continue;
        }

        //仅仅对搜索响应处理。其他的响应是不做处理的。
        if (header.serviceType() != QKnxNetIp::ServiceType::SearchResponse
            && header.serviceType() != QKnxNetIp::ServiceType::ExtendedSearchResponse) {
            qDebug()<<"####header.servicetype is"<<header.serviceType()<<"不是searchResponse响应报文，退出";

            continue;
        }

        const auto frame = QKnxNetIpFrame::fromBytes(data);
        const auto proxy = QKnxNetIpSearchResponseProxy(frame);

        qDebug()<<frame.header()<<frame.data()<<frame.serviceType();

        if (!proxy.isValid())
            continue;

        if (m_config.DiscoveryCoreV1 && !proxy.isExtended()) {
            qDebug()<<"发现设备。";
            qDebug()<<"报文的发送方："<<datagram.senderAddress();
            qDebug()<<"searchresponse的控制端："<<proxy.controlEndpoint();
            qDebug()<<"支持的家族服务："<<proxy.supportedFamilies()<<typeid(proxy.supportedFamilies()).name();
            qDebug()<<"m_address: "<<m_address<<"/n网卡 信息："<<m_iface;
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
            qDebug()<<"————————触发发现设备——————————";
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

void Discoverer::onError(QUdpSocket::SocketError /* error */)
{
    emit errorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
                       m_socket ? m_socket->errorString()
                                : QKnxNetIpServerDiscoveryAgent::tr("Unknown socket error happened.")
                       );
    finish();
}

void Discoverer::setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State state)
{
    m_state = state;
    emit stateChanged(m_state);

    if (m_state == QKnxNetIpServerDiscoveryAgent::State::Starting)
        emit started();
    if (m_state == QKnxNetIpServerDiscoveryAgent::State::Stopping)
        emit finished(this);
}

QKnxNetIpServerDiscoveryAgentPrivate::QKnxNetIpServerDiscoveryAgentPrivate(const QHostAddress &addr,
                                                                           quint16 prt)
    : port(prt)
    , address(addr)
{}

void QKnxNetIpServerDiscoveryAgentPrivate::setupSocket()
{
    qDebug()<<"####Feature Info :this is QKnxNetIpServerDiscoveryAgentPrivate::setupSocket!!!!!";
    usedPort = port;
    usedAddress = address;
    QKnxPrivate::clearSocket(&socket);
    qDebug()<<"usedPort:"<<port<<"usedAddress:"<<address<<"   address type is :"<<typeid(address).name();

    Q_Q(QKnxNetIpServerDiscoveryAgent);
    socket = new QUdpSocket(q);
    qDebug()<<"Socket is :"<<socket->isValid();
    qDebug()<<"q is the typeof "<<typeid(q).name();
    // qDebug()<<"q status is : "<<q->state();

    adapter = QKnxPrivate::adaptersForAddresses(QVector<QHostAddress> { address }).value(0);
    QObject::connect(socket, &QUdpSocket::stateChanged, q, [&](QUdpSocket::SocketState s) {
        Q_Q(QKnxNetIpServerDiscoveryAgent);
        switch (s) {
        case QUdpSocket::BoundState:
            setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Running);
            socket->setSocketOption(QUdpSocket::SocketOption::MulticastTtlOption, ttl);

            if (type == QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast) {
                QNetworkInterface mni;
                const auto interfaces = QNetworkInterface::allInterfaces();
                for (const auto &iface : interfaces) {
                    if (!iface.flags().testFlag(QNetworkInterface::CanMulticast))
                        continue;

                    const auto entries = iface.addressEntries();
                    for (const auto &entry : entries) {
                        auto ip = entry.ip();
                        if (ip.protocol() != QAbstractSocket::NetworkLayerProtocol::IPv4Protocol)
                            continue;
                        if (ip != address)
                            continue;
                        mni = iface;
                        break;
                    }
                }

                if (mni.isValid())
                    socket->setMulticastInterface(mni);

                if (socket->joinMulticastGroup(multicastAddress, mni)) {
                    usedPort = multicastPort;
                    usedAddress = multicastAddress;
                } else {
                    setAndEmitErrorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
                                            QKnxNetIpServerDiscoveryAgent::tr("Could not join multicast group."));
                    q->stop();
                }
            } else {
                usedPort = socket->localPort();
                usedAddress = socket->localAddress();
            }

            if (q->state() == QKnxNetIpServerDiscoveryAgent::State::Running) {
                servers.clear();

                const QFlags<QKnxNetIpServerDiscoveryAgent::DiscoveryMode> flags(mode);
                if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV1)) {
                    auto frame = QKnxNetIpSearchRequestProxy::builder()
                    .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::builder()
                                              .setHostAddress(nat ? QHostAddress::AnyIPv4 : usedAddress)
                                              .setPort(nat ? quint16(0u) : usedPort).create()
                                          ).create();
                    socket->writeDatagram(frame.bytes().toByteArray(), multicastAddress,
                                          multicastPort);
                }

                if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV2)) {
                    auto frame = QKnxNetIpSearchRequestProxy::extendedBuilder()
                    .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::builder()
                                              .setHostAddress(nat ? QHostAddress::AnyIPv4 : usedAddress)
                                              .setPort(nat ? quint16(0u) : usedPort).create()
                                          )
                        .setExtendedParameters(srps).create();
                    socket->writeDatagram(frame.bytes().toByteArray(), multicastAddress,
                                          multicastPort);
                }

                setupAndStartReceiveTimer();
                setupAndStartFrequencyTimer();
            }
            break;
        default:
            break;
        }
    });

    // using overload = void (QUdpSocket::*)(QUdpSocket::SocketError);
    // QObject::connect(socket,static_cast<overload>(&QUdpSocket::error), q, [&](QUdpSocket::SocketError) {
    //                      setAndEmitErrorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
    //                                              socket->errorString());

    //                      Q_Q(QKnxNetIpServerDiscoveryAgent);
    //                      q->stop();
    //                  });
    QObject::connect(socket,&QUdpSocket::errorOccurred, q, [&](QUdpSocket::SocketError) {
        setAndEmitErrorOccurred(QKnxNetIpServerDiscoveryAgent::Error::Network,
                                socket->errorString());

        Q_Q(QKnxNetIpServerDiscoveryAgent);
        q->stop();
    });

    QObject::connect(socket, &QUdpSocket::readyRead, q, [&]() {
        Q_Q(QKnxNetIpServerDiscoveryAgent);
        while (socket->hasPendingDatagrams()) {
            qDebug()<<"socket->hasPendingDatagrams().now Socket is :"<<socket->isValid();

            if (q->state() != QKnxNetIpServerDiscoveryAgent::State::Running)
                break;

            auto datagram = socket->receiveDatagram();
            auto data = QKnxByteArray::fromByteArray(datagram.data());
            const auto header = QKnxNetIpFrameHeader::fromBytes(data, 0);
            if (!header.isValid())
                continue;

            if (header.serviceType() != QKnxNetIp::ServiceType::SearchResponse &&
                header.serviceType() != QKnxNetIp::ServiceType::ExtendedSearchResponse) {
                continue;
            }
            qDebug()<<"header is :"<<header<<"header type is :"<<typeid(header).name();

            if (header.serviceType() == QKnxNetIp::ServiceType::SearchResponse )
            {
                qDebug()<<"header.serviceType() is QKnxNetIp::ServiceType::SearchResponse :";
            }else if(                header.serviceType() == QKnxNetIp::ServiceType::ExtendedSearchResponse)
            {
                qDebug()<<"header.serviceType() is QKnxNetIp::ServiceType::ExtendedSearchResponse :";
            }
            qDebug()<<"header.serviceType() is :"<<header.serviceType()<<"header.serviceType() type is :"<<typeid(header.serviceType()).name();
            auto frame = QKnxNetIpFrame::fromBytes(data);

            auto response = QKnxNetIpSearchResponseProxy(frame);//response.controlEndpoint()是返回的控制端的HPAI，IP和端口
            if (!response.isValid())
                continue;
            qDebug()<<"frame is :"<<frame;
            qDebug()<<"frame type is :"<<typeid(frame).name();
            qDebug()<<"frame.data() is :"<<frame.data();
            qDebug()<<"datagram.senderAddress() is :"<<datagram.senderAddress();
            qDebug()<<"datagram.senderPort() is :"<<datagram.senderPort();
            qDebug()<<"response.controlEndpoint() is :"<<response.controlEndpoint()<<"response.controlEndpoint() type is :"<<typeid(response.controlEndpoint()).name();
            qDebug()<<"response.controlEndpoint().code is :"<<response.controlEndpoint().code()<<"response.controlEndpoint().code type is :"<<typeid(response.controlEndpoint().code()).name();
            qDebug()<<"response.isExtended() is : "<<response.isExtended();
            qDebug()<<"adapter.address is : "<<typeid(adapter.address).name();
            qDebug()<<"adapter.iface is : "<<adapter.iface;




            const QFlags<QKnxNetIpServerDiscoveryAgent::DiscoveryMode> flags(mode);
            if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV1)
                && !response.isExtended()) {
                qDebug()<<"_________________CoreV1_OK_________________";
                qDebug()<<"____setAndEmitDeviceDiscovered___";
                setAndEmitDeviceDiscovered({
                    (nat ? QKnxNetIpHpaiProxy::builder()
                               .setHostAddress(datagram.senderAddress())
                               .setPort(datagram.senderPort()).create()
                         : response.controlEndpoint()
                     ), response.deviceHardware(), response.supportedFamilies(),
                        adapter.address, adapter.iface
                });
            }

            if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV2)
                && response.isExtended()) {
                const auto optionalDibs = response.optionalDibs();
                setAndEmitDeviceDiscovered({
                    (nat ? QKnxNetIpHpaiProxy::builder()
                               .setHostAddress(datagram.senderAddress())
                               .setPort(datagram.senderPort()).create()
                         : response.controlEndpoint()
                     ), response.deviceHardware(), response.supportedFamilies(),
                        adapter.address, adapter.iface,
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
                });
            }
        }
    });
}

void QKnxNetIpServerDiscoveryAgentPrivate::setupAndStartReceiveTimer()
{
    Q_Q(QKnxNetIpServerDiscoveryAgent);

    QKnxPrivate::clearTimer(&receiveTimer);
    if (timeout >= 0) {
        receiveTimer = new QTimer(q);
        receiveTimer->setSingleShot(true);
        receiveTimer->start(timeout);
        QObject::connect(receiveTimer, &QTimer::timeout, q, &QKnxNetIpServerDiscoveryAgent::stop);
    }
}

void QKnxNetIpServerDiscoveryAgentPrivate::setupAndStartFrequencyTimer()
{
    Q_Q(QKnxNetIpServerDiscoveryAgent);

    QKnxPrivate::clearTimer(&frequencyTimer);
    if (frequency > 0) {
        frequencyTimer = new QTimer(q);
        frequencyTimer->setSingleShot(false);
        frequencyTimer->start(60000 / frequency);

        QObject::connect(frequencyTimer, &QTimer::timeout, q, [&]() {
            Q_Q(QKnxNetIpServerDiscoveryAgent);
            if (q->state() == QKnxNetIpServerDiscoveryAgent::State::Running) {
                servers.clear();

                const QFlags<QKnxNetIpServerDiscoveryAgent::DiscoveryMode> flags(mode);
                if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV1)) {
                    auto frame = QKnxNetIpSearchRequestProxy::builder()
                    .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::builder()
                                              .setHostAddress(nat ? QHostAddress::AnyIPv4 : usedAddress)
                                              .setPort(nat ? quint16(0u) : usedPort).create()
                                          ).create();
                    socket->writeDatagram(frame.bytes().toByteArray(), multicastAddress,
                                          multicastPort);
                }

                if (flags.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV2)) {
                    auto frame = QKnxNetIpSearchRequestProxy::extendedBuilder()
                    .setDiscoveryEndpoint(QKnxNetIpHpaiProxy::builder()
                                              .setHostAddress(nat ? QHostAddress::AnyIPv4 : usedAddress)
                                              .setPort(nat ? quint16(0u) : usedPort).create()
                                          )
                        .setExtendedParameters(srps).create();
                    socket->writeDatagram(frame.bytes().toByteArray(), multicastAddress,
                                          multicastPort);
                }
            }
        });
    }
}

void QKnxNetIpServerDiscoveryAgentPrivate::setAndEmitStateChanged(
    QKnxNetIpServerDiscoveryAgent::State newState)
{
    state = newState;

    Q_Q(QKnxNetIpServerDiscoveryAgent);
    emit q->stateChanged(newState);

    if (state == QKnxNetIpServerDiscoveryAgent::State::Running)
        emit q->started();
    else if (state == QKnxNetIpServerDiscoveryAgent::State::NotRunning)
        emit q->finished();
}

void QKnxNetIpServerDiscoveryAgentPrivate::setAndEmitDeviceDiscovered(
    const QKnxNetIpServerInfo &discoveryInfo)
{
    qDebug()<<"####Feature Info :this is QKnxNetIpServerDiscoveryAgentPrivate::setAndEmitDeviceDiscovered!!!!!";

    servers.append(discoveryInfo);

    qDebug()<<"discoveryInfo type is :"<<typeid(discoveryInfo).name();
    qDebug()<<"discoveryInfo.deviceName()"<<discoveryInfo.deviceName();
    //遍历discoveryInfo
    qDebug()<<"discoveryInfo.individualAddress()"<<discoveryInfo.individualAddress();
   qDebug()<<"discoveryInfo.tunnelingInfo()"<<discoveryInfo.tunnelingInfo();
    qDebug()<<"discoveryInfo.hardware()"<<discoveryInfo.hardware();
   qDebug()<<"discoveryInfo.tunnelingSlotInfos()"<<typeid(discoveryInfo.tunnelingSlotInfos()).name();

    Q_Q(QKnxNetIpServerDiscoveryAgent);
    emit q->deviceDiscovered(discoveryInfo);
}

void QKnxNetIpServerDiscoveryAgentPrivate::setAndEmitErrorOccurred(
    QKnxNetIpServerDiscoveryAgent::Error newError, const QString &message)
{
    error = newError;
    errorString = message;

    Q_Q(QKnxNetIpServerDiscoveryAgent);
    emit q->errorOccurred(error, errorString);
}

void QKnxNetIpServerDiscoveryAgentPrivate::start()
{
    qDebug()<<"####Feature Info :this is QKnxNetIpServerDiscoveryAgentPrivate::start!!!!!";

    if (state != QKnxNetIpServerDiscoveryAgent::State::NotRunning)
        return;
    qDebug()<<"state is : "<<state;
    if (address.isNull())
        address = QHostAddress(QHostAddress::AnyIPv4);
    qDebug()<<"address is : "<<address;

    auto isIPv4 = true;
    address.toIPv4Address(&isIPv4);
    if (isIPv4) {
        setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Starting);
        setupSocket();
        qDebug()<<type;
        if (type == QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast) {
            qDebug()<<"type == QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast";

            socket->bind(QHostAddress::AnyIPv4, multicastPort, QUdpSocket::ShareAddress
                                                                   | QAbstractSocket::ReuseAddressHint);
        } else {
            qDebug()<<"type != QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast";
            socket->bind(address, port);
        }
    } else {
        setAndEmitErrorOccurred(QKnxNetIpServerDiscoveryAgent::Error::NotIPv4,
                                QKnxNetIpServerDiscoveryAgent::tr("Only IPv4 local address supported."));
    }
}

void QKnxNetIpServerDiscoveryAgentPrivate::start(const QVector<Adapter> &adapters)
{
    if (state != QKnxNetIpServerDiscoveryAgent::State::NotRunning)
        return;

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Starting);

    if (adapters.isEmpty())
        return stop();

    Q_Q(QKnxNetIpServerDiscoveryAgent);
    const QFlags<QKnxNetIpServerDiscoveryAgent::ResponseType> rtype(type);
    const QFlags<QKnxNetIpServerDiscoveryAgent::DiscoveryMode> dmode(mode);
    const DiscovererConfig config {
        ttl,
        timeout,
        nat,
        rtype.testFlag(QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast),
        dmode.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV1),
        dmode.testFlag(QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV2),
        srps
    };

    bool connectError = (adapters.count() == 1);
    for (const auto &adapter : adapters) {
        auto discoverer = new Discoverer(adapter.address, adapter.iface, config);
        discoveries.append(discoverer);

        auto thread = new QThread;
        discoverer->moveToThread(thread);
        QObject::connect(thread, &QThread::started, discoverer, &Discoverer::start);
        QObject::connect(thread, &QThread::finished, discoverer, &Discoverer::deleteLater);

        QObject::connect(thread, &QThread::started, discoverer, &Discoverer::start);
        QObject::connect(thread, &QThread::finished, discoverer, &Discoverer::finish);

        QObject::connect(discoverer, &Discoverer::finished, q, [&](Discoverer *ds) {
            discoveries.removeAll(ds);
            if (discoveries.isEmpty())
                stop();
        });
        QObject::connect(discoverer, &Discoverer::finished, thread, &QThread::quit);
        QObject::connect(discoverer, &Discoverer::deviceDiscovered, q, [&](const QKnxNetIpServerInfo &server) {
            setAndEmitDeviceDiscovered(server);
        });
        if (connectError) {
            QObject::connect(discoverer, &Discoverer::errorOccurred, q,
                             [&](QKnxNetIpServerDiscoveryAgent::Error error, const QString &errorString) {
                                 setAndEmitErrorOccurred(error, errorString);
                             });
        }
        thread->start();
    }

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Running);
}

void QKnxNetIpServerDiscoveryAgentPrivate::start(const QVector<QHostAddress> addresses)
{
    start(QKnxPrivate::adaptersForAddresses(addresses));
}

void QKnxNetIpServerDiscoveryAgentPrivate::start(QKnxNetIpServerDiscoveryAgent::InterfaceTypes types)
{
    QVector<Adapter> adapters;
    auto all = QNetworkInterface::allInterfaces();
    for (const auto &iface : all) {
        const auto flags = iface.flags();
        if (!flags.testFlag(QNetworkInterface::IsUp)
            && !flags.testFlag(QNetworkInterface::IsRunning)
            && !flags.testFlag(QNetworkInterface::CanMulticast)) continue;

        const bool wifi = types.testFlag(QKnxNetIpServerDiscoveryAgent::InterfaceType::Wifi);
        const bool loopback = types.testFlag(QKnxNetIpServerDiscoveryAgent::InterfaceType::Loopback);
        const bool ethernet = types.testFlag(QKnxNetIpServerDiscoveryAgent::InterfaceType::Ethernet);

        const auto type = iface.type();
        if (!(wifi && type == QNetworkInterface::Wifi)
            && !(loopback && type == QNetworkInterface::Loopback)
            && !(ethernet && type == QNetworkInterface::Ethernet)) continue;

        const auto entries = iface.addressEntries();
        for (const auto &entry : entries) {
            const auto address = entry.ip();
            if (address.protocol() != QAbstractSocket::IPv4Protocol)
                continue;
            adapters.append({ address, iface });
            break;
        }
    }
    start(adapters);
}

void QKnxNetIpServerDiscoveryAgentPrivate::stop()
{
    if (state == QKnxNetIpServerDiscoveryAgent::State::Stopping
        || state == QKnxNetIpServerDiscoveryAgent::State::NotRunning) {
        return;
    }

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::Stopping);

    if (socket) {
        if (type == QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast
            && socket->state() == QUdpSocket::BoundState) {
            socket->leaveMulticastGroup(multicastAddress);
        }
        socket->close();
    }

    QKnxPrivate::clearSocket(&(socket));
    QKnxPrivate::clearTimer(&(receiveTimer));
    QKnxPrivate::clearTimer(&(frequencyTimer));

    setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State::NotRunning);
}

QT_END_NAMESPACE
