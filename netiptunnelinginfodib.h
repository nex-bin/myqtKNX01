#ifndef NETIPTUNNELINGINFODIB_H
#define NETIPTUNNELINGINFODIB_H

#include <QtCore/qshareddata.h>
#include <qnewaddress.h>
#include <netipstruct.h>

QT_BEGIN_NAMESPACE

class QKnxNetIpTunnelingSlotInfoPrivate;
class MYLIB_EXPORT QKnxNetIpTunnelingSlotInfo final
{
    Q_GADGET

public:
    enum State : quint32
    {
        NotAvailable = 0x00000000,
        Free = 0x00000001,
        Authorized = 0x00000002,
        Usable = 0x00000004,
        Available = Free | Authorized | Usable,
        Invalid = 0xffffffff
    };
    Q_ENUM(State)
    Q_DECLARE_FLAGS(Status, State)

    QKnxNetIpTunnelingSlotInfo();
    ~QKnxNetIpTunnelingSlotInfo();

    explicit QKnxNetIpTunnelingSlotInfo(const QKnxAddress &ia);
    QKnxNetIpTunnelingSlotInfo(const QKnxAddress &is, QKnxNetIpTunnelingSlotInfo::Status status);

    bool isValid() const;

    QKnxAddress individualAddress() const;
    void setIndividualAddress(const QKnxAddress &ia);

    QKnxNetIpTunnelingSlotInfo::Status status() const;
    void setStatus(QKnxNetIpTunnelingSlotInfo::Status status);

    QKnxByteArray bytes() const;
    static QKnxNetIpTunnelingSlotInfo fromBytes(const QKnxByteArray &data, quint16 index);

    QKnxNetIpTunnelingSlotInfo(const QKnxNetIpTunnelingSlotInfo &other);
    QKnxNetIpTunnelingSlotInfo &operator=(const QKnxNetIpTunnelingSlotInfo &other);

    void swap(QKnxNetIpTunnelingSlotInfo &other) Q_DECL_NOTHROW;

    QKnxNetIpTunnelingSlotInfo(QKnxNetIpTunnelingSlotInfo &&other) Q_DECL_NOTHROW;
    QKnxNetIpTunnelingSlotInfo &operator=(QKnxNetIpTunnelingSlotInfo &&other) Q_DECL_NOTHROW;

    bool operator==(const QKnxNetIpTunnelingSlotInfo &other) const;
    bool operator!=(const QKnxNetIpTunnelingSlotInfo &other) const;

private:
    QSharedDataPointer<QKnxNetIpTunnelingSlotInfoPrivate> d_ptr;
};
Q_DECLARE_OPERATORS_FOR_FLAGS(QKnxNetIpTunnelingSlotInfo::Status)

class QKnxNetIpTunnelingInfoDibPrivate;
class MYLIB_EXPORT QKnxNetIpTunnelingInfoDibProxy final
{
public:
    QKnxNetIpTunnelingInfoDibProxy() = delete;
    ~QKnxNetIpTunnelingInfoDibProxy() = default;

    QKnxNetIpTunnelingInfoDibProxy(const QKnxNetIpDib &&) = delete;
    explicit QKnxNetIpTunnelingInfoDibProxy(const QKnxNetIpDib &dib);

    bool isValid() const;

    QKnxNetIp::DescriptionType descriptionType() const;

    quint16 maximumInterfaceApduLength() const;
    QKnxNetIpTunnelingSlotInfo tunnelingSlotInfo() const;
    QVector<QKnxNetIpTunnelingSlotInfo> optionalSlotInfos() const;

    class MYLIB_EXPORT Builder final
    {
    public:
        Builder();
        ~Builder();

        Builder &setMaximumInterfaceApduLength(quint16 length);
        Builder &setTunnelingSlotInfo(const QKnxNetIpTunnelingSlotInfo &info);
        Builder &setOptionalSlotInfos(const QVector<QKnxNetIpTunnelingSlotInfo> &infos);

        QKnxNetIpDib create() const;

        Builder(const Builder &other);
        Builder &operator=(const Builder &other);

    private:
        QSharedDataPointer<QKnxNetIpTunnelingInfoDibPrivate> d_ptr;
    };
    static QKnxNetIpTunnelingInfoDibProxy::Builder builder();

private:
    const QKnxNetIpDib &m_dib;
};

QT_END_NAMESPACE

#endif
