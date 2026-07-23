#ifndef NETIPFRAMEHEADER_H
#define NETIPFRAMEHEADER_H

#include <qnewbytearray.h>
#include <netip01.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpFrameHeader final
{
public:
    QKnxNetIpFrameHeader() = default;
    ~QKnxNetIpFrameHeader() = default;

    explicit QKnxNetIpFrameHeader(QKnxNetIp::ServiceType type);
    QKnxNetIpFrameHeader(QKnxNetIp::ServiceType type, quint16 dataSize);

    static constexpr const quint8 HeaderSize10 = 0x06;
    static constexpr const quint8 KnxNetIpVersion10 = 0x10;

    bool isNull() const;
    bool isValid() const;

    quint8 size() const;
    quint8 setSize(quint8 size);

    quint8 protocolVersion() const;
    void setProtocolVersion(quint8 version);

    quint16 totalSize() const;

    quint16 dataSize() const;
    void setDataSize(quint16 dataSize);

    QKnxNetIp::ServiceType serviceType() const;
    void setServiceType(QKnxNetIp::ServiceType type);

    quint8 byte(quint8 index) const;
    QKnxByteArray bytes() const;

    static QKnxNetIpFrameHeader fromBytes(const QKnxByteArray &bytes, quint16 index = 0);

    bool operator==(const QKnxNetIpFrameHeader &other) const;
    bool operator!=(const QKnxNetIpFrameHeader &other) const;

private:
    quint8 m_bytes[6] { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
};
QDebug operator<<(QDebug debug, const QKnxNetIpFrameHeader &header);

QT_END_NAMESPACE

#endif
