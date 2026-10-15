
#ifndef QKNXNETIPDISCOVERYAGENT_H
#define QKNXNETIPDISCOVERYAGENT_H

#include <QtCore/qobject.h>
#include <QtCore/qvector.h>
#include <netipserviceinfo.h>
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpServerDiscoveryAgentPrivate;
class MYLIB_EXPORT QKnxNetIpServerDiscoveryAgent final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QKnxNetIpServerDiscoveryAgent)
    Q_DECLARE_PRIVATE(QKnxNetIpServerDiscoveryAgent)

public:
    enum class State : quint8
    {
        NotRunning,
        Starting,
        Running,
        Stopping
    };
    Q_ENUM(State)

    enum class Error : quint8
    {
        None,
        Network,
        NotIPv4,
        Timeout,
        Unknown = 0x80
    };
    Q_ENUM(Error)

    enum class ResponseType : quint8
    {
        Unicast = 0x00,
        Multicast = 0x01
    };
    Q_ENUM(ResponseType)

    enum DiscoveryMode : quint8
    {
        CoreV1 = 0x01,
        CoreV2 = 0x02
    };
    Q_ENUM(DiscoveryMode)
    Q_DECLARE_FLAGS(DiscoveryModes, DiscoveryMode)

    enum class InterfaceType : quint8
    {
        Loopback = 0x01,
        Ethernet = 0x02,
        Wifi = 0x04
    };
    Q_ENUM(InterfaceType)
    Q_DECLARE_FLAGS(InterfaceTypes, InterfaceType)

    QKnxNetIpServerDiscoveryAgent(QObject *parent = nullptr);
    ~QKnxNetIpServerDiscoveryAgent() override;

    explicit QKnxNetIpServerDiscoveryAgent(const QHostAddress &localAddress,
                                           QObject *parent = nullptr);
    QKnxNetIpServerDiscoveryAgent(const QHostAddress &localAddress, quint16 localPort,
                                  QObject *parent = nullptr);

    QKnxNetIpServerDiscoveryAgent::State state() const;
    QKnxNetIpServerDiscoveryAgent::Error error() const;

    QString errorString() const;
    QVector<QKnxNetIpServerInfo> discoveredServers() const;

    quint16 localPort() const;
    void setLocalPort(quint16 port);

    QHostAddress localAddress() const;
    void setLocalAddress(const QHostAddress &address);

    int timeout() const;
    void setTimeout(int msec);

    int searchFrequency() const;
    void setSearchFrequency(int timesPerMinute);

    bool natAware() const;
    void setNatAware(bool useNat);

    quint8 multicastTtl() const;
    void setMulticastTtl(quint8 ttl);

    QKnxNetIpServerDiscoveryAgent::ResponseType responseType() const;
    void setResponseType(QKnxNetIpServerDiscoveryAgent::ResponseType type);

    QKnxNetIpServerDiscoveryAgent::DiscoveryModes discoveryMode() const;
    void setDiscoveryMode(QKnxNetIpServerDiscoveryAgent::DiscoveryModes mode);

    QVector<QKnxNetIpSrp> extendedSearchParameters() const;
    void setExtendedSearchParameters(const QVector<QKnxNetIpSrp> &srps);

public Q_SLOTS:
    void start();
    void stop();

    void start(int timeout);
    void start(const QVector<QHostAddress> &localAddresses);
    void start(QKnxNetIpServerDiscoveryAgent::InterfaceTypes types);

Q_SIGNALS:
    void started();
    void finished();

    void deviceDiscovered(QKnxNetIpServerInfo server);
    void stateChanged(QKnxNetIpServerDiscoveryAgent::State state);
    void errorOccurred(QKnxNetIpServerDiscoveryAgent::Error error, QString errorString);

private:
    QKnxNetIpServerDiscoveryAgent(QKnxNetIpServerDiscoveryAgentPrivate &dd, QObject *parent);
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QKnxNetIpServerDiscoveryAgent::DiscoveryModes)
Q_DECLARE_OPERATORS_FOR_FLAGS(QKnxNetIpServerDiscoveryAgent::InterfaceTypes)

QT_END_NAMESPACE

#endif
