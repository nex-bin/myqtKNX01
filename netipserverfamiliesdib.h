#ifndef NETIPSERVICEFAMILIESDIB_H
#define NETIPSERVICEFAMILIESDIB_H

#include <netipstruct.h>
#include <qmyknx.h>

QT_BEGIN_NAMESPACE

struct Q_KNX_EXPORT QKnxServiceInfo
{
    QKnxNetIp::ServiceFamily ServiceFamily;
    quint8 ServiceFamilyVersion;

    bool operator==(const QKnxServiceInfo &other) const;
    bool operator!=(const QKnxServiceInfo &other) const;
};

class Q_KNX_EXPORT QKnxNetIpServiceFamiliesDibProxy final
{
public:
    QKnxNetIpServiceFamiliesDibProxy() = delete;
    ~QKnxNetIpServiceFamiliesDibProxy() = default;

    QKnxNetIpServiceFamiliesDibProxy(const QKnxNetIpDib &&) = delete;
    explicit QKnxNetIpServiceFamiliesDibProxy(const QKnxNetIpDib &dib);

    bool isValid() const;

    QKnxNetIp::DescriptionType descriptionType() const;
    QVector<QKnxServiceInfo> serviceInfos() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder &setServiceInfos(const QVector<QKnxServiceInfo> &infos);

        QKnxNetIpDib create() const;

    private:
        QVector<QKnxServiceInfo> m_infos;
    };
    static QKnxNetIpServiceFamiliesDibProxy::Builder builder();

private:
    const QKnxNetIpDib &m_dib;
};

QT_END_NAMESPACE

#endif
