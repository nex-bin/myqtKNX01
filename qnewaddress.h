#ifndef QKNXADDRESS_H
#define QKNXADDRESS_H

#include <QtCore/qdebug.h>
#include <QtCore/qstring.h>
#include <qmyknx.h>
#include <qnewutils.h>

QT_BEGIN_NAMESPACE

class Q_KNX_EXPORT QKnxAddress final
{
public:
    enum class Type : quint8
    {
        Group = 0x01,
        Individual = 0x00
    };
    QKnxAddress::Type type() const;

    enum class Notation : quint8
    {
        TwoLevel = 0x00,
        ThreeLevel = 0x01,
    };

    QKnxAddress() = default;
    QKnxAddress(QKnxAddress::Type type, quint16 address);
    QKnxAddress(QKnxAddress::Type type, const QString &address);
    QKnxAddress(QKnxAddress::Type type, const QKnxByteArray &address);

    static QKnxAddress createGroup(quint8 mainGroup, quint16 subGroup);
    static QKnxAddress createGroup(quint8 mainGroup, quint16 middleGroup, quint8 subGroup);

    static QKnxAddress createIndividual(quint8 area, quint16 line, quint8 sequentialNumber);

    quint8 mainOrAreaSection() const;
    quint8 middleOrLineSection() const;
    quint16 subOrDeviceSection(Notation notation = Notation::ThreeLevel) const;

    bool isValid() const;
    bool isBroadcast() const;

    bool isUnregistered() const;
    bool isCouplerOrRouter() const;

    struct Q_KNX_EXPORT Group final { static QKnxAddress Broadcast; };
    struct Q_KNX_EXPORT Individual final { static QKnxAddress Unregistered; };


    quint8 size() const { return 2; }
    QKnxByteArray bytes() const
    {
        if (!isValid())
            return {};
        return QKnxUtils::QUint16::bytes(quint16(m_address));
    }

    QString toString(Notation notation = Notation::ThreeLevel) const;

    bool operator==(const QKnxAddress &other) const;
    bool operator!=(const QKnxAddress &other) const;

private:
    QKnxAddress(QKnxAddress::Type type, quint16 sec1, quint16 *sec2, quint16 sec3);

private:
    qint32 m_address = -1;
    QKnxAddress::Type m_type = static_cast<QKnxAddress::Type>(0xff);
};
Q_KNX_EXPORT QDebug operator<<(QDebug debug, const QKnxAddress &address);
Q_KNX_EXPORT Q_DECL_PURE_FUNCTION uint qHash(const QKnxAddress &key, uint seed = 0) Q_DECL_NOTHROW;

Q_DECLARE_TYPEINFO(QKnxAddress::Type, Q_PRIMITIVE_TYPE);
Q_DECLARE_TYPEINFO(QKnxAddress::Notation, Q_PRIMITIVE_TYPE);

QT_END_NAMESPACE

#endif
