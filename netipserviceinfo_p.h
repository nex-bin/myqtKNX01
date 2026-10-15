#ifndef NETIPSERVERINFO_P_H
#define NETIPSERVERINFO_P_H


#include <QtCore/qshareddata.h>

#include <netipdevicedib.h>
#include <netiphpai.h>
#include <netipserverfamiliesdib.h>

#include <QtNetwork/qhostaddress.h>
#include <QtNetwork/qnetworkinterface.h>

QT_BEGIN_NAMESPACE

struct MYLIB_EXPORT QKnxNetIpServerInfoPrivate final : public QSharedData
{
    QKnxNetIpServerInfoPrivate() = default;
    ~QKnxNetIpServerInfoPrivate() = default;

    QKnxNetIpHpai hpai;
    QKnxNetIpDib hardware;
    QKnxNetIpDib services;
    QKnxNetIpDib tunnelingInfo;
    QKnxNetIpDib extendedHardware;
    QHostAddress address;
    QNetworkInterface iinterface;
};

QT_END_NAMESPACE

#endif
