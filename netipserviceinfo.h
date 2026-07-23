#ifndef NETIPSERVERINFO_H
#define NETIPSERVERINFO_H

#include <QtCore/qmetatype.h>
#include <QtCore/qshareddata.h>

#include <qnewaddress.h>
#include <netipserverfamiliesdib.h>
#include <netiptunnelinginfodib.h>

#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_NAMESPACE

struct QKnxNetIpServerInfoPrivate;

class Q_KNX_EXPORT QKnxNetIpServerInfo final
{
    friend class Discoverer;
    friend class QKnxNetIpServerDiscoveryAgentPrivate;
    friend class QKnxNetIpServerDescriptionAgentPrivate;

public:
    QKnxNetIpServerInfo();
    ~QKnxNetIpServerInfo();

    QString deviceName() const;
    QKnxAddress individualAddress() const;

    quint16 controlEndpointPort() const;
    QHostAddress controlEndpointAddress() const;

    QVector<QKnxServiceInfo> supportedServices() const;
    QVector<QKnxNetIpTunnelingSlotInfo> tunnelingSlotInfos() const;

    MediumType mediumStatus() const;
    // QKnx::MediumStatus mediumStatus() const;
    quint16 maximumLocalApduLength() const;
    quint16 maskVersion() const;

    QKnxNetIpHpai endpoint() const;
    QKnxNetIpDib hardware() const;
    QKnxNetIpDib services() const;
    QKnxNetIpDib tunnelingInfo() const;
    QKnxNetIpDib extendedHardware() const;

    QHostAddress hostAddress() const;
    QNetworkInterface networkInterface() const;

    QKnxNetIpServerInfo(const QKnxNetIpServerInfo &other);
    QKnxNetIpServerInfo &operator=(const QKnxNetIpServerInfo &other);

    QKnxNetIpServerInfo(QKnxNetIpServerInfo &&other) Q_DECL_NOTHROW;
    QKnxNetIpServerInfo &operator=(QKnxNetIpServerInfo &&other) Q_DECL_NOTHROW;

    bool operator==(const QKnxNetIpServerInfo &other) const;
    bool operator!=(const QKnxNetIpServerInfo &other) const;

    void swap(QKnxNetIpServerInfo &other) Q_DECL_NOTHROW;

private:
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QKnxNetIpServerInfo(const QKnxNetIpHpai &hpai, const QKnxNetIpDib &hardware,
                        QKnxNetIpDib services);

    QKnxNetIpServerInfo(const QKnxNetIpHpai &hpai, const QKnxNetIpDib &hardware,
                        const QKnxNetIpDib &services, const QKnxNetIpDib &tunnelingInfo,
                        const QKnxNetIpDib &extendedHardware);
#endif

    QKnxNetIpServerInfo(const QKnxNetIpHpai &hpai,
                        const QKnxNetIpDib &hardware,
                        const QKnxNetIpDib &services,
                        const QHostAddress &hostAddress,
                        const QNetworkInterface &iinterface,
                        const QKnxNetIpDib &tunnelingInfo = {},
                        const QKnxNetIpDib &extendedHardware = {});

    explicit QKnxNetIpServerInfo(QKnxNetIpServerInfoPrivate &dd);

private:
    QExplicitlySharedDataPointer<QKnxNetIpServerInfoPrivate> d_ptr;
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QKnxNetIpServerInfo)

#endif
