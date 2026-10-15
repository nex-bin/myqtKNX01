#ifndef QNEWBUILDERDATA_P_H
#define QNEWBUILDERDATA_P_H


#include <QtCore/qshareddata.h>

#include <qnewcryptographicengine.h>
#include <netipframe.h>
#include <netipserverfamiliesdib.h>
#include <netipsrp.h>

QT_BEGIN_NAMESPACE

class MYLIB_EXPORT QKnxFeatureBuilderPrivate : public QSharedData
{
public:
    QKnxFeatureBuilderPrivate() = default;
    ~QKnxFeatureBuilderPrivate() = default;

    quint8 m_channelId { 0 };//通道ID。
    quint8 m_sequenceNumber { 0 };//序列号   用于重传检测和报文排序
    //连接类型？
    // MediumType m_if { MediumType::Unknown };
    QKnx::InterfaceFeature m_if { QKnx::InterfaceFeature::Unknown };

    //接收成功的状态返回。
    QKnx::ReturnCode m_result { QKnx::ReturnCode::Success };
    QKnxByteArray m_featureValue;
};

class MYLIB_EXPORT QKnxNetIpSrpProxy::ProgrammingModePrivate : public QSharedData
{
public:
    bool m_mandatory { true };
};

class MYLIB_EXPORT QKnxNetIpSrpProxy::MacAddressPrivate : public QSharedData
{
public:
    QKnxByteArray m_macAddress;
    bool m_mandatory { true };
};

class QKnxNetIpSrpProxy::SupportedFamilyPrivate : public QSharedData
{
public:
    QKnxServiceInfo m_info;
    bool m_mandatory { true };
};

class QKnxNetIpSrpProxy::RequestDibsPrivate : public QSharedData
{
public:
    QVector<QKnxNetIp::DescriptionType> m_types;
    bool m_mandatory { true };
};

class QKnxNetIpSearchRequestExtendedBuilderPrivate : public QSharedData
{
public:
    QKnxNetIpSearchRequestExtendedBuilderPrivate() = default;
    ~QKnxNetIpSearchRequestExtendedBuilderPrivate() = default;

    QKnxNetIpHpai m_hpai = { QKnxNetIp::HostProtocol::Unknown};
    QVector<QKnxNetIpSrp> m_srps;
};

class QKnxNetIpSearchResponseExtendedBuilderPrivate : public QSharedData
{
public:
    QKnxNetIpSearchResponseExtendedBuilderPrivate() = default;
    ~QKnxNetIpSearchResponseExtendedBuilderPrivate() = default;

    QKnxNetIpHpai m_hpai = { QKnxNetIp::HostProtocol::Unknown};
    QSet<QKnxNetIpDib> m_optionalDibs;
    QKnxNetIpDib m_hardware;
    QKnxNetIpDib m_supFamilies;
};

class QKnxNetIpSecureWrapperPrivate : public QSharedData
{
public:
    QKnxNetIpSecureWrapperPrivate() = default;
    ~QKnxNetIpSecureWrapperPrivate() = default;

    quint16 m_sessionId { 0 };
    quint64 m_seqNumber { Q_UINT48_MAX + 1 };
    QKnxByteArray m_serial;
    QKnxByteArray m_encryptedFrame;
    QKnxNetIpFrame m_unencryptedFrame;
    quint16 m_tag { 0 };
    QKnxByteArray m_authCode;
};

class QKnxNetIpTimerNotifyBuilderPrivate : public QSharedData
{
public:
    QKnxNetIpTimerNotifyBuilderPrivate() = default;
    ~QKnxNetIpTimerNotifyBuilderPrivate() = default;

    quint64 m_timer { Q_UINT48_MAX + 1 };
    QKnxByteArray m_serial;
    qint32 m_tag { -1 };
    QKnxByteArray m_authCode;
};

class QKnxNetIpSessionResponseBuilderPrivate : public QSharedData
{
public:
    QKnxNetIpSessionResponseBuilderPrivate() = default;
    ~QKnxNetIpSessionResponseBuilderPrivate() = default;

    qint32 m_id { -1 };
    QKnxByteArray m_serverPublicKey;
    QKnxByteArray m_authCode;
};

class QKnxNetIpSessionAuthenticateBuilderPrivate : public QSharedData
{
public:
    QKnxNetIpSessionAuthenticateBuilderPrivate() = default;
    ~QKnxNetIpSessionAuthenticateBuilderPrivate() = default;

    QKnxNetIp::SecureUserId m_id { QKnxNetIp::SecureUserId::Invalid };
    QKnxByteArray m_authCode;
};

QT_END_NAMESPACE

#endif
