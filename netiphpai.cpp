#include "netiphpai.h"
#include "qnewutils.h"

QT_BEGIN_NAMESPACE

/*!
    \class QKnxNetIpHpaiProxy

    \inmodule QtKnx
    \ingroup qtknx-netip

    \brief The QKnxNetIpHpaiProxy class provides the means to read the
    KNXnet/IP host address protocol information (HPAI) from the generic
    QKnxNetIpHpai class and to create such a structure.

    A KNXnet/IP HPAI structure contains the information that is necessary to
    uniquely identify an KNXnet/IP transport connection endpoint.

    The information needed to identify an KNXnet/IP transport connection
    endpoint includes the \l{hostProtocol()}{protocol}, \l{hostAddress()}
    {IP address}, and \l port number.

    \note When using QKnxNetIpHpaiProxy care must be taken to ensure that the
    referenced KNXnet/IP HPAI structure outlives the QKnxNetIpHpaiProxy on all
    code paths, lest the proxy ends up referencing deleted data.

    Reading the host address and port number can be achieved like this:
    \code
        auto hpai = QKnxNetIpHpai::fromBytes(...);

        QKnxNetIpHpaiProxy proxy(hpai);
        if (!proxy.isValid())
            return;

        if (proxy.hostProtocol() == QKnxNetIp::HostProtocol::TCP_IPv4)
            return; // TCP support not implemented yet

        // read the host protocol address information
        auto address = proxy.hostAddress();
        auto port = proxy.port();
    \endcode

    \sa builder(), {Qt KNXnet/IP Connection Classes}
*/

/*!
    \internal
    \fn QKnxNetIpHpaiProxy::QKnxNetIpHpaiProxy()
*/

/*!
    \internal
    \fn QKnxNetIpHpaiProxy::~QKnxNetIpHpaiProxy()
*/

/*!
    \internal
    \fn QKnxNetIpHpaiProxy::QKnxNetIpHpaiProxy(const QKnxNetIpHpai &&)
*/

/*!
    Constructs a proxy object with the given a KNXnet/IP structure \a hpai to
    read the host protocol address information (HPAI).
*/
QKnxNetIpHpaiProxy::QKnxNetIpHpaiProxy(const QKnxNetIpHpai &hpai)
    : m_hpai(hpai)
{}

/*!
    Returns \c true if the KNXnet/IP structure to create the object is a valid
    KNXnet/IP HPAI structure, returns \c false otherwise.
*/
bool QKnxNetIpHpaiProxy::isValid() const
{
    bool validHostProtocolCode = m_hpai.code() == QKnxNetIp::HostProtocol::UDP_IPv4
                                 || m_hpai.code() == QKnxNetIp::HostProtocol::TCP_IPv4;
    return m_hpai.isValid() && m_hpai.size() == 8 && validHostProtocolCode;
}

/*!
    Return the host protocol from KNXnet/IP structure if the object passed
    during construction was valid, otherwise returns QKnxNetIp::Unknown.
*/
QKnxNetIp::HostProtocol QKnxNetIpHpaiProxy::hostProtocol() const
{
    if (isValid())
        return m_hpai.code();
    return QKnxNetIp::HostProtocol::Unknown;
}

/*!
    Returns the host address from KNXnet/IP structure if the object passed
    during construction was valid, otherwise returns a \e {default-constructed}
    QHostAddress.
*/
QHostAddress QKnxNetIpHpaiProxy::hostAddress() const
{
    if (isValid())
        return QKnxUtils::HostAddress::fromBytes(m_hpai.constData());
    return {};
}

/*!
    Returns the port number carried inside the KNXnet/IP host protocol address
    information structure.
*/
quint16 QKnxNetIpHpaiProxy::port() const
{
    return QKnxUtils::QUint16::fromBytes(m_hpai.constData(), 4);
}

/*!
    Returns a builder object to create a KNXnet/IP host protocol address
    information structure.
*/
QKnxNetIpHpaiProxy::Builder QKnxNetIpHpaiProxy::builder()
{
    return QKnxNetIpHpaiProxy::Builder();
}


/*!
    \class QKnxNetIpHpaiProxy::Builder

    \inmodule QtKnx
    \inheaderfile QKnxNetIpHpaiProxy

    \brief The QKnxNetIpHpaiProxy::Builder class provides the means to create a
    KNXnet/IP host address protocol information.

    A KNXnet/IP HPAI structure contains the information that is necessary to
    uniquely identify an KNXnet/IP transport connection endpoint.

    The information needed to identify an KNXnet/IP transport connection
    endpoint includes the \l{hostProtocol()}{protocol}, \l{hostAddress()}
    {IP address}, and \l port number.

    The common way to create such a HPAI structure is:
    \code
        auto hpai = QKnxNetIpHpaiProxy::builder()
            .setHostProtocol(QKnxNetIp::HostProtocol::TCP_IPv4
            .setHostAddress(QHostAddress::AnyIPv4)
            .setPort(2013)
            .create();
    \endcode

    By default setting the host protocol can be omitted if you want to target a
    KNXnet/IP server using a UDP/IP connection.
*/

/*!
    Sets the host protocol to \a code and returns a reference to the builder.
*/
QKnxNetIpHpaiProxy::Builder &QKnxNetIpHpaiProxy::Builder::setHostProtocol(QKnxNetIp::HostProtocol code)
{
    m_code = code;
    return *this;
}

/*!
    Sets the host address to \a address and returns a reference to the builder.
*/
QKnxNetIpHpaiProxy::Builder &QKnxNetIpHpaiProxy::Builder::setHostAddress(const QHostAddress &address)
{
    m_address = address;
    return *this;
}

/*!
    Sets the host port to \a port and returns a reference to the builder.
*/
QKnxNetIpHpaiProxy::Builder &QKnxNetIpHpaiProxy::Builder::setPort(quint16 port)
{
    m_port = port;
    return *this;
}

/*!
    Creates and returns a QKnxNetIpHpai.

    \note The returned structure may be invalid depending on the values used
    during setup.

    \sa isValid()
*/
QKnxNetIpHpai QKnxNetIpHpaiProxy::Builder::create() const
{
    return { m_code, QKnxUtils::HostAddress::bytes(m_address) + QKnxUtils::QUint16::bytes(m_port) };
}

QT_END_NAMESPACE
