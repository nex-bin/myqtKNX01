#ifndef NETIPCRI_H
#define NETIPCRI_H

#include <qnewaddress.h>
#include <netipstruct.h>

QT_BEGIN_NAMESPACE

class MYLIB_EXPORT QKnxNetIpCriProxy final
{
public:
    QKnxNetIpCriProxy() = delete;
    ~QKnxNetIpCriProxy() = default;

    QKnxNetIpCriProxy(const QKnxNetIpCri &&) = delete;
    explicit QKnxNetIpCriProxy(const QKnxNetIpCri &cri);

    bool isValid() const;
    bool isExtended() const;

    QKnxNetIp::ConnectionType connectionType() const;
    QKnxNetIp::TunnelLayer tunnelLayer() const;
    QKnxAddress individualAddress() const;
    QKnxByteArray additionalData() const;

    class MYLIB_EXPORT Builder final
    {
    public:
        Builder &setConnectionType(QKnxNetIp::ConnectionType type);
        Builder &setTunnelLayer(QKnxNetIp::TunnelLayer layer);
        Builder &setIndividualAddress(const QKnxAddress &address);
        Builder &setAdditionalData(const QKnxByteArray &additionalData);

        QKnxNetIpCri create() const;

    private:
        QKnxNetIp::ConnectionType m_cType = QKnxNetIp::ConnectionType::Tunnel;
        QKnxByteArray m_additionalData;
    };
    static QKnxNetIpCriProxy::Builder builder();

private:
    const QKnxNetIpCri &m_cri;
};

QT_END_NAMESPACE

#endif
