#ifndef NETIPCONNECTREQUEST_H
#define NETIPCONNECTREQUEST_H

#include <netipcri.h>
#include <netipframe.h>
#include <netiphpai.h>

QT_BEGIN_NAMESPACE

class Q_KNX_EXPORT QKnxNetIpConnectRequestProxy final
{
public:
    QKnxNetIpConnectRequestProxy() = delete;
    ~QKnxNetIpConnectRequestProxy() = default;

    QKnxNetIpConnectRequestProxy(const QKnxNetIpFrame &&) = delete;
    explicit QKnxNetIpConnectRequestProxy(const QKnxNetIpFrame &frame);

    bool isValid() const;

    QKnxNetIpHpai controlEndpoint() const;
    QKnxNetIpHpai dataEndpoint() const;
    QKnxNetIpCri requestInformation() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder &setControlEndpoint(const QKnxNetIpHpai &hpai);
        Builder &setDataEndpoint(const QKnxNetIpHpai &hpai);
        Builder &setRequestInformation(const QKnxNetIpCri &cri);

        QKnxNetIpFrame create() const;

    private:
        QKnxNetIpHpai m_ceHpai;
        QKnxNetIpHpai m_deHpai;
        QKnxNetIpCri m_cri;
    };
    static QKnxNetIpConnectRequestProxy::Builder builder();

private:
    const QKnxNetIpFrame &m_frame;
};

QT_END_NAMESPACE

#endif
