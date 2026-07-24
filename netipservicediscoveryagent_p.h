#ifndef NETIPSERVICEDISCOVERYAGENT_P_H
#define NETIPSERVICEDISCOVERYAGENT_P_H


#include <QtCore/qtimer.h>
#include <netip01.h>
#include <netipsearchrequest.h>
#include <netipsearchresponse.h>
#include <netipservicediscoveryagent.h>

#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkdatagram.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qudpsocket.h>

#include <qnewobject_p.h>

QT_BEGIN_NAMESPACE

struct Adapter
{
    QHostAddress address;
    QNetworkInterface iface;
};

struct DiscovererConfig
{
    DiscovererConfig() = default;
    ~DiscovererConfig() = default;

    DiscovererConfig(int ttl, int timeout, bool nat, bool multicast, bool coreV1, bool coreV2,
                     const QVector<QKnxNetIpSrp> &srps)
        : Ttl(ttl)
        , Timeout(timeout)
        , Nat(nat)
        , Multicast(multicast)
        , DiscoveryCoreV1(coreV1)
        , DiscoveryCoreV2(coreV2)
        , ExtendedSearchParameters(srps)
    {}

    int Ttl { 64 };
    int Timeout { 3000 };

    bool Nat { false };
    bool Multicast { true };

    bool DiscoveryCoreV1 { true };
    bool DiscoveryCoreV2 { true };

    QVector<QKnxNetIpSrp> ExtendedSearchParameters;
};

class Discoverer : public QObject
{
    Q_OBJECT

public:
    Discoverer(const QHostAddress &address, const QNetworkInterface iface, const DiscovererConfig &config);
    ~Discoverer() override;

public slots:
    void start();
    void finish();

signals:
    void started();
    void timeout();
    void finished(Discoverer *iam);

    void stateChanged(QKnxNetIpServerDiscoveryAgent::State state);
    void errorOccurred(QKnxNetIpServerDiscoveryAgent::Error error, const QString &errorString);

    void deviceDiscovered(const QKnxNetIpServerInfo &discoveryInfo);

private slots:
    void onTimeout();
    void onReadyRead();
    void onError(QUdpSocket::SocketError error);

private:
    void setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State state);

private:
    QHostAddress m_address;
    QNetworkInterface m_iface;

    QTimer *m_timer { nullptr };
    QUdpSocket *m_socket { nullptr };

    DiscovererConfig m_config;
    QVector<QByteArray> m_devices;

    QHostAddress m_multicast { QStringLiteral("224.0.23.12") };
    QKnxNetIpServerDiscoveryAgent::State m_state { QKnxNetIpServerDiscoveryAgent::State::NotRunning };
};

class Q_KNX_EXPORT QKnxNetIpServerDiscoveryAgentPrivate final : public QObjectPrivate
{
    Q_DECLARE_PUBLIC(QKnxNetIpServerDiscoveryAgent)


public:
    QKnxNetIpServerDiscoveryAgentPrivate(const QHostAddress &addr, quint16 prt);
    ~QKnxNetIpServerDiscoveryAgentPrivate()  = default;
    // ~QKnxNetIpServerDiscoveryAgentPrivate() override = default;

    void setupSocket();

    void setupAndStartReceiveTimer();
    void setupAndStartFrequencyTimer();

    void setAndEmitStateChanged(QKnxNetIpServerDiscoveryAgent::State newState);
    void setAndEmitDeviceDiscovered(const QKnxNetIpServerInfo &discoveryInfo);
    void setAndEmitErrorOccurred(QKnxNetIpServerDiscoveryAgent::Error newError, const QString &message);

    void start();
    void start(const QVector<Adapter> &adapters);
    void start(const QVector<QHostAddress> addresses);
    void start(QKnxNetIpServerDiscoveryAgent::InterfaceTypes types);
    void stop();

private:
    QUdpSocket *socket { nullptr };
    QTimer *receiveTimer { nullptr };
    QTimer *frequencyTimer { nullptr };

    quint16 port { 0 }, usedPort { 0 };
    QHostAddress address { QHostAddress::Null }, usedAddress;

    const quint16 multicastPort { QKnxNetIp::Constants::DefaultPort };
    const QHostAddress multicastAddress { QLatin1String(QKnxNetIp::Constants::MulticastAddress) };

    quint8 ttl { 64 };
    bool nat { false };
    int frequency { 0 };
    int timeout { QKnxNetIp::Timeout::SearchTimeout };

    QString errorString;
    QVector<QKnxNetIpServerInfo> servers;

    QKnxNetIpServerDiscoveryAgent::Error error { QKnxNetIpServerDiscoveryAgent::Error::None };
    QKnxNetIpServerDiscoveryAgent::State state { QKnxNetIpServerDiscoveryAgent::State::NotRunning };
    QKnxNetIpServerDiscoveryAgent::ResponseType
        type { QKnxNetIpServerDiscoveryAgent::ResponseType::Multicast };

    QKnxNetIpServerDiscoveryAgent::DiscoveryModes mode
        { QKnxNetIpServerDiscoveryAgent::DiscoveryMode::CoreV1 };
    QVector<QKnxNetIpSrp> srps;

    Adapter adapter;
    QVector<Discoverer*> discoveries;
};

QT_END_NAMESPACE

#endif
