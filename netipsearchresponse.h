#ifndef NETIPSEARCHRESPONSE_H
#define NETIPSEARCHRESPONSE_H

#include <netipdevicedib.h>
#include <netipframe.h>
#include <netiphpai.h>
#include <netipserverfamiliesdib.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpSearchResponseExtendedBuilderPrivate;
class Q_KNX_EXPORT QKnxNetIpSearchResponseProxy final
{
public:
    QKnxNetIpSearchResponseProxy() = delete;
    ~QKnxNetIpSearchResponseProxy() = default;

    QKnxNetIpSearchResponseProxy(const QKnxNetIpFrame &&) = delete;
    explicit QKnxNetIpSearchResponseProxy(const QKnxNetIpFrame &frame);

    bool isValid() const;
    bool isExtended() const;

    QKnxNetIpHpai controlEndpoint() const;
    QKnxNetIpDib deviceHardware() const;
    QKnxNetIpDib supportedFamilies() const;

    QVector<QKnxNetIpDib> optionalDibs() const;
    QVector<QKnxNetIpDib> variableDibs() const;

    class Q_KNX_EXPORT Builder final
    {
    public:
        Builder &setControlEndpoint(const QKnxNetIpHpai &hpai);
        Builder &setDeviceHardware(const QKnxNetIpDib &ddib);
        Builder &setSupportedFamilies(const QKnxNetIpDib &sdib);

        QKnxNetIpFrame create() const;

    private:
        QKnxNetIpHpai m_hpai;
        QKnxNetIpDib m_ddib;
        QKnxNetIpDib m_sdib;
    };
    static QKnxNetIpSearchResponseProxy::Builder builder();

    class Q_KNX_EXPORT ExtendedBuilder final
    {
    public:
        ExtendedBuilder();
        ~ExtendedBuilder();

        ExtendedBuilder &setControlEndpoint(const QKnxNetIpHpai &hpai);
        ExtendedBuilder &setDeviceHardware(const QKnxNetIpDib &ddib);
        ExtendedBuilder &setSupportedFamilies(const QKnxNetIpDib &sdib);
        ExtendedBuilder &setOptionalDibs(const QSet<QKnxNetIpDib> &dibs);

        QKnxNetIpFrame create() const;

        ExtendedBuilder(const ExtendedBuilder &other);
        ExtendedBuilder &operator=(const ExtendedBuilder &other);

    private:
        QSharedDataPointer<QKnxNetIpSearchResponseExtendedBuilderPrivate> d_ptr;
    };
    static QKnxNetIpSearchResponseProxy::ExtendedBuilder extendedBuilder();

private:
    const QKnxNetIpFrame &m_frame;
};

QT_END_NAMESPACE

#endif
