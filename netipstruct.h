#ifndef NETIPSTRUCT_H
#define NETIPSTRUCT_H

#include <qnewbytearray.h>
#include <netip01.h>
#include <qnewtraits.h>
#include <netipstructheader.h>

QT_BEGIN_NAMESPACE

template <typename CodeType> class QKnxNetIpStruct final
{
    static_assert(is_type<CodeType, QKnxNetIp::HostProtocol, QKnxNetIp::ConnectionType,
                          QKnxNetIp::DescriptionType, QKnxNetIp::SearchParameterType>::value, "Type not supported.");

public:
    QKnxNetIpStruct() = default;
    ~QKnxNetIpStruct() = default;

    QKnxNetIpStruct(CodeType codeType, const QKnxByteArray &dataField = {})
        : m_header(codeType)
    {
        setData(dataField);
    }

    QKnxNetIpStruct(const QKnxNetIpStructHeader<CodeType> &headerField,
                    const QKnxByteArray &dataField = {})
        : m_header(headerField)
        , m_data(dataField)
    {}

    bool isNull() const
    {
        return m_header.isNull() && m_data.isNull();
    }

    bool isValid() const
    {
        return m_header.isValid() && size() == (m_header.size() + m_data.size());
    }

    quint16 size() const
    {
        return m_header.totalSize();
    }

    quint16 dataSize() const
    {
        return m_header.dataSize();
    }

    QKnxNetIpStructHeader<CodeType> header() const
    {
        return m_header;
    }

    void setHeader(const QKnxNetIpStructHeader<CodeType> &headerField)
    {
        m_header = headerField;
    }

    CodeType code() const
    {
        return m_header.code();
    }

    QKnxByteArray data() const
    {
        return m_data;
    }

    const QKnxByteArray &constData() const
    {
        return m_data;
    }

    void setData(const QKnxByteArray &dataField)
    {
        m_data = dataField;
        m_header.setDataSize(dataField.size());
    }

    QKnxByteArray bytes() const
    {
        return m_header.bytes() + m_data;
    }

    static QKnxNetIpStruct fromBytes(const QKnxByteArray &bytes, quint16 index = 0)
    {
        auto header = QKnxNetIpStructHeader<CodeType>::fromBytes(bytes, index);
        if (!header.isValid())
            return {};
        return { header, bytes.mid(index + header.size(), header.dataSize()) };
    }

    bool operator==(const QKnxNetIpStruct &other) const
    {
        return (m_header == other.m_header) && (m_data == other.m_data);
    }

    bool operator!=(const QKnxNetIpStruct &other) const
    {
        return !operator==(other);
    }

private:
    QKnxNetIpStructHeader<CodeType> m_header;
    QKnxByteArray m_data;
};

using QKnxNetIpHpai = QKnxNetIpStruct<QKnxNetIp::HostProtocol>;
using QKnxNetIpCri = QKnxNetIpStruct<QKnxNetIp::ConnectionType>;
using QKnxNetIpCrd = QKnxNetIpStruct<QKnxNetIp::ConnectionType>;
using QKnxNetIpDib = QKnxNetIpStruct<QKnxNetIp::DescriptionType>;
using QKnxNetIpSrp = QKnxNetIpStruct<QKnxNetIp::SearchParameterType>;


MYLIB_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::HostProtocol> &hpai);
MYLIB_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::ConnectionType> &cr);
MYLIB_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::DescriptionType> &dib);
MYLIB_EXPORT QDebug operator<<(QDebug debug, const QKnxNetIpStruct<QKnxNetIp::SearchParameterType> &srp);

template <typename CodeType>
Q_DECL_PURE_FUNCTION uint qHash(const QKnxNetIpStruct<CodeType> &key, uint seed = 0) Q_DECL_NOTHROW
{
    return qHash(key.bytes(), seed);
}

QT_END_NAMESPACE

#endif
