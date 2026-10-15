#ifndef NETIPDEVICEDIB_H
#define NETIPDEVICEDIB_H

#include <qnewaddress.h>
#include <netipstruct.h>

#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

class MYLIB_EXPORT QKnxNetIpDeviceDibProxy final
{
public:
    QKnxNetIpDeviceDibProxy() = delete;
    ~QKnxNetIpDeviceDibProxy() = default;

    QKnxNetIpDeviceDibProxy(const QKnxNetIpDib &&) = delete;
    explicit QKnxNetIpDeviceDibProxy(const QKnxNetIpDib &dib);

    bool isValid() const;
    QKnxNetIp::DescriptionType descriptionType() const;

    MediumType mediumStatus() const;
    // mediumType
    // QKnx::MediumType mediumType() const;
    QKnxNetIp::ProgrammingMode deviceStatus() const;
    QKnxAddress individualAddress() const;
    quint16 projectInstallationId() const;
    QKnxByteArray serialNumber() const;
    QHostAddress multicastAddress() const;
    QKnxByteArray macAddress() const;
    QByteArray deviceName() const;

    class MYLIB_EXPORT Builder final
    {
    public:
        Builder &setMediumType(MediumType mediumType);
        Builder &setDeviceStatus(QKnxNetIp::ProgrammingMode mode);
        Builder &setIndividualAddress(const QKnxAddress &address);
        Builder &setProjectInstallationId(quint16 projectId);
        Builder &setSerialNumber(const QKnxByteArray &serialNumber);
        Builder &setMulticastAddress(const QHostAddress &multicastAddress);
        Builder &setMacAddress(const QKnxByteArray &macAddress);
        Builder &setDeviceName(const QByteArray &deviceName);

        QKnxNetIpDib create() const;

    private:
        MediumType m_mediumType { MediumType::Unknown };
        QKnxNetIp::ProgrammingMode m_progMode { QKnxNetIp::ProgrammingMode::Inactive };
        QKnxAddress m_address;
        quint16 m_projectId { 0 };
        QKnxByteArray m_serialNumber;
        QHostAddress m_multicastAddress { QLatin1String(QKnxNetIp::Constants::MulticastAddress) };
        QKnxByteArray m_macAddress;
        QByteArray m_deviceName;
    };
    static QKnxNetIpDeviceDibProxy::Builder builder();

private:
    const QKnxNetIpDib &m_dib;
};

QT_END_NAMESPACE

#endif
