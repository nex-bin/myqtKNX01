#ifndef NETIPCONNECTIONHEADER_H
#define NETIPCONNECTIONHEADER_H

#include <qnewbytearray.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpConnectionHeader final
{
public:
    QKnxNetIpConnectionHeader() = default;
    ~QKnxNetIpConnectionHeader() = default;

    QKnxNetIpConnectionHeader(quint8 channelId, quint8 seqNumber);
    QKnxNetIpConnectionHeader(quint8 channelId, quint8 seqNumber, quint8 serviceTypeSpecificValue);

    bool isNull() const;
    bool isValid() const;

    quint8 size() const;

    quint8 channelId() const;
    void setChannelId(quint8 channelId);

    quint8 sequenceNumber() const;
    void setSequenceNumber(quint8 seqNumber);

    quint8 serviceTypeSpecificValue() const;
    void setServiceTypeSpecificValue(quint8 value);

    QKnxByteArray connectionTypeSpecificHeaderItems() const;
    void setConnectionTypeSpecificHeaderItems(const QKnxByteArray &items);

    quint8 byte(quint8 index) const;
    QKnxByteArray bytes() const;

    static QKnxNetIpConnectionHeader fromBytes(const QKnxByteArray &bytes, quint16 index = 0);

    bool operator==(const QKnxNetIpConnectionHeader &other) const;
    bool operator!=(const QKnxNetIpConnectionHeader &other) const;

private:
    QKnxByteArray m_bytes { 0x00, 0x00, 0x00, 0x00 };
};
QDebug operator<<(QDebug debug, const QKnxNetIpConnectionHeader &header);

QT_END_NAMESPACE

#endif
