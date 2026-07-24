#ifndef NETIPSEARCHREQUEST_H
#define NETIPSEARCHREQUEST_H

#include <netipframe.h>
#include <netiphpai.h>

#include <QtCore/qshareddata.h>
#include <QtCore/qvector.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpSearchRequestExtendedBuilderPrivate;
class Q_KNX_EXPORT QKnxNetIpSearchRequestProxy final
{
public:
    QKnxNetIpSearchRequestProxy() = delete;
    ~QKnxNetIpSearchRequestProxy() = default;

    QKnxNetIpSearchRequestProxy(const QKnxNetIpFrame &&) = delete;
    explicit QKnxNetIpSearchRequestProxy(const QKnxNetIpFrame &frame);

    bool isValid() const;
    bool isExtended() const;

    QKnxNetIpHpai discoveryEndpoint() const;
    QVector<QKnxNetIpSrp> extendedSearchParameters() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder &setDiscoveryEndpoint(const QKnxNetIpHpai &hpai);
        QKnxNetIpFrame create() const;

    private:
        QKnxNetIpHpai m_hpai;
    };
    static QKnxNetIpSearchRequestProxy::Builder builder();

    class Q_KNX_EXPORT ExtendedBuilder final
    {
    public:
        ExtendedBuilder();
        ~ExtendedBuilder();

        ExtendedBuilder &setDiscoveryEndpoint(const QKnxNetIpHpai &hpai);
        ExtendedBuilder &setExtendedParameters(const QVector<QKnxNetIpSrp> &srps);

        QKnxNetIpFrame create() const;

        ExtendedBuilder(const ExtendedBuilder &other);
        ExtendedBuilder &operator=(const ExtendedBuilder &other);

    private:
        QSharedDataPointer<QKnxNetIpSearchRequestExtendedBuilderPrivate> d_ptr;
    };
    static QKnxNetIpSearchRequestProxy::ExtendedBuilder extendedBuilder();

private:
    const QKnxNetIpFrame &m_frame;
};

QT_END_NAMESPACE

#endif
