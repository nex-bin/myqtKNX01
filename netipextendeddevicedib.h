#ifndef NETIPEXTENDEDDEVICEDIB_H
#define NETIPEXTENDEDDEVICEDIB_H

#include <netipstruct.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpExtendedDeviceDibBuilderPrivate;
class Q_KNX_EXPORT QKnxNetIpExtendedDeviceDibProxy final
{
public:
    QKnxNetIpExtendedDeviceDibProxy() = delete;
    ~QKnxNetIpExtendedDeviceDibProxy() = default;

    QKnxNetIpExtendedDeviceDibProxy(const QKnxNetIpDib &&) = delete;
    explicit QKnxNetIpExtendedDeviceDibProxy(const QKnxNetIpDib &dib);

    bool isValid() const;
    QKnxNetIp::DescriptionType descriptionType() const;

    MediumType mediumStatus() const;
    quint16 maximumLocalApduLength() const;
    quint16 deviceDescriptorType0() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder();
        ~Builder();

        Builder &setMediumStatus(MediumType status);
        Builder &setMaximumLocalApduLength(quint16 length);
        Builder &setDeviceDescriptorType0(quint16 type);

        QKnxNetIpDib create() const;

        Builder(const Builder &other);
        Builder &operator=(const Builder &other);

    private:
        QSharedDataPointer<QKnxNetIpExtendedDeviceDibBuilderPrivate> d_ptr;
    };
    static QKnxNetIpExtendedDeviceDibProxy::Builder builder();

private:
    const QKnxNetIpDib &m_dib;
};

QT_END_NAMESPACE

#endif
