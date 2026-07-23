#ifndef NETIPFRAME_H
#define NETIPFRAME_H

#include <QtCore/qshareddata.h>

#include <qnewbytearray.h>
#include <netip01.h>
#include <netipconnectionheader.h>
#include <netipframeheader.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpFramePrivate;
class QKnxNetIpFrame
{
public:
    QKnxNetIpFrame();
    ~QKnxNetIpFrame();

    QKnxNetIpFrame(QKnxNetIp::ServiceType type, const QKnxByteArray &data = {});
    QKnxNetIpFrame(QKnxNetIp::ServiceType type,
                   const QKnxNetIpConnectionHeader &connectionHeader, const QKnxByteArray &data = {});
    QKnxNetIpFrame(const QKnxNetIpFrameHeader &header,
                   const QKnxNetIpConnectionHeader &connectionHeader, const QKnxByteArray &data = {});

    bool isNull()const;
    bool isValid() const;

    quint16 size() const;
    quint16 dataSize() const;

    QKnxNetIpFrameHeader header() const;
    void setHeader(const QKnxNetIpFrameHeader &header);

    quint8 protocolVersion() const;

    QKnxNetIp::ServiceType serviceType() const;
    void setServiceType(QKnxNetIp::ServiceType type);

    QKnxNetIpConnectionHeader connectionHeader() const;
    void setConnectionHeader(const QKnxNetIpConnectionHeader &header);

    quint8 channelId() const;
    quint8 sequenceNumber() const;
    quint8 serviceTypeSpecificValue() const;
    QKnxByteArray connectionTypeSpecificHeaderItems() const;

    QKnxByteArray data() const;
    const QKnxByteArray &constData() const;
    void setData(const QKnxByteArray &data);

    QKnxByteArray bytes() const;
    static QKnxNetIpFrame fromBytes(const QKnxByteArray &bytes, quint16 index = 0);

    QKnxNetIpFrame(const QKnxNetIpFrame &other);
    QKnxNetIpFrame &operator=(const QKnxNetIpFrame &other);

    QKnxNetIpFrame(QKnxNetIpFrame &&other) Q_DECL_NOTHROW;
    QKnxNetIpFrame &operator=(QKnxNetIpFrame &&other) Q_DECL_NOTHROW;

    void swap(QKnxNetIpFrame &other) Q_DECL_NOTHROW;

    bool operator==(const QKnxNetIpFrame &other) const;
    bool operator!=(const QKnxNetIpFrame &other) const;

private:
    explicit QKnxNetIpFrame(QKnxNetIpFramePrivate &dd);

private:
    QSharedDataPointer<QKnxNetIpFramePrivate> d_ptr;
};
QDebug operator<<(QDebug debug, const QKnxNetIpFrame &frame);

QT_END_NAMESPACE

#endif
