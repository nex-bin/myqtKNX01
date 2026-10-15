#ifndef NETIPHPAI_H
#define NETIPHPAI_H

#include <netipstruct.h>
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

class MYLIB_EXPORT QKnxNetIpHpaiProxy final
{
public:
    QKnxNetIpHpaiProxy() = delete;
    ~QKnxNetIpHpaiProxy() = default;

    QKnxNetIpHpaiProxy(const QKnxNetIpHpai &&) = delete;
    explicit QKnxNetIpHpaiProxy(const QKnxNetIpHpai &hpai);

    bool isValid() const;

    QKnxNetIp::HostProtocol hostProtocol() const;
    QHostAddress hostAddress() const;
    quint16 port() const;

    class MYLIB_EXPORT Builder final
    {
    public:
        Builder &setHostProtocol(QKnxNetIp::HostProtocol code);
        Builder &setHostAddress(const QHostAddress &address);
        Builder &setPort(quint16 port);

        QKnxNetIpHpai create() const;

    private:
        QKnxNetIp::HostProtocol m_code { QKnxNetIp::HostProtocol::UDP_IPv4 };
        QHostAddress m_address { QHostAddress::Null };
        quint16 m_port { 0 };
    };
    static QKnxNetIpHpaiProxy::Builder builder();

private:
    const QKnxNetIpHpai &m_hpai;
};

QT_END_NAMESPACE

#endif
