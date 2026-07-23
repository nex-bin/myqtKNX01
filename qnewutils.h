#ifndef QNEWUTILS_H
#define QNEWUTILS_H

#include <qnewbytearray.h>
#include <qmyknx.h>
#include <QtNetwork/qhostaddress.h>

QT_BEGIN_NAMESPACE

struct QKnxUtils final
{
    struct QUint8 final
    {
        static QKnxByteArray bytes(quint8 integer)
        {
            return { quint8(integer) };
        }

        static quint8 fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 1)
                return {};
            return data.at(index);
        }
    };

    struct QUint16 final
    {
        static QKnxByteArray bytes(quint16 integer)
        {
            return { quint8(integer >> 8), quint8(integer) };
        }

        static quint16 fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 2)
                return {};
            return quint16(quint16(data.at(index)) << 8 | data.at(index + 1));
        }
    };

    struct QUint32 final
    {
        static QKnxByteArray bytes(quint32 integer)
        {
            return { quint8(integer >> 24), quint8(integer >> 16), quint8(integer >> 8),
                    quint8(integer) };
        }

        static quint32 fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 4)
                return {};
            return quint32(quint32(data.at(index)) << 24
                           | quint32(data.at(index + 1)) << 16
                           | quint32(data.at(index + 2)) << 8
                           | data.at(index + 3));
        }
    };

    struct QUint48 final

    //2026年7月18日，手动quint48更改。
    {
        static QKnxByteArray bytes(quint48 integer)
        {
            // integer &= 0xFFFFFFFFFFFFULL;

            return { quint8(integer >> 40), quint8(integer >> 32), quint8(integer >> 24),
                    quint8(integer >> 16), quint8(integer >> 8), quint8(integer) };
        }

        static quint48 fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 6)
                return {};
            return quint48(quint48(data.at(index + 0)) << 40
                           | quint48(data.at(index + 1)) << 32
                           | quint48(data.at(index + 2)) << 24
                           | quint48(data.at(index + 3)) << 16
                           | quint48(data.at(index + 4)) << 8
                           | data.at(index + 5));
        }
    };

    struct QUint64 final
    {
        static QKnxByteArray bytes(quint64 integer)
        {
            return { quint8(integer >> 56), quint8(integer >> 48), quint8(integer >> 40)
                , quint8(integer >> 32), quint8(integer >> 24), quint8(integer >> 16)
                    , quint8(integer >> 8), quint8(integer) };
        }

        static quint64 fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 8)
                return {};
            return quint64(quint64(data.at(index)) << 56
                           | quint64(data.at(index + 1)) << 48
                           | quint64(data.at(index + 2)) << 40
                           | quint64(data.at(index + 3)) << 32
                           | quint64(data.at(index + 4)) << 24
                           | quint64(data.at(index + 5)) << 16
                           | quint64(data.at(index + 6)) << 8
                           | data.at(index + 7));
        }
    };

    struct HostAddress final
    {
        static QKnxByteArray bytes(const QHostAddress &address)
        {
            return QUint32::bytes(address.toIPv4Address());
        }

        static QHostAddress fromBytes(const QKnxByteArray &data, quint16 index = 0)
        {
            if (data.size() - index < 4)
                return {};
            return QHostAddress(QUint32::fromBytes(data, index));
        }
    };
};

QT_END_NAMESPACE

#endif // QMYKNXUTILS_H
