#ifndef BASICDATATYPE_H
#define BASICDATATYPE_H

#include "qmyknx.h"
#include "QString"
#include "QHostAddress"
#include <QMetaType>
#include <QDatetime>

namespace comBasicDateType{


}

struct MYLIB_EXPORT DeviceInfo{
    QString deviceStatus_text;
    QHostAddress address;
    qint16 port;
};
struct MYLIB_EXPORT PendingPacketInfo {
    QDateTime sendTime;       // 发送时间
    QByteArray cemiFrame;     // 发送的原始 cEMI 帧（可选，用于记录）
};
enum class KNXDPT {
    DPT_1bit,
    DPT_4bit,
    DPT_1byte,
    DPT_2bytes,
    DPT_3bytes,
    DPT_4bytes,
    DPT_8bytes,
    DPT_14bytes,
};
inline QString dptToString(KNXDPT dpt) {
    switch (dpt) {
    case KNXDPT::DPT_1bit:  return QStringLiteral("1-bit 开关");
    case KNXDPT::DPT_4bit:  return QStringLiteral("4-bit 调光");
    case KNXDPT::DPT_1byte: return QStringLiteral("1-byte 场景/百分比");
    case KNXDPT::DPT_2bytes: return QStringLiteral("2-byte");
    case KNXDPT::DPT_3bytes:  return QStringLiteral("3-byte");
    case KNXDPT::DPT_4bytes: return QStringLiteral("4-byte");
    case KNXDPT::DPT_8bytes: return QStringLiteral("8-byte");
    case KNXDPT::DPT_14bytes: return QStringLiteral("14-byte 字符串");

    default:            return QStringLiteral("未知");
    }
}
struct MYLIB_EXPORT KnxBusRecord {
    QDateTime timestamp;        // 捕获时间
    quint8    channelID;
    quint8    sequenceNumber;
    QString   sourceAddress;    // 源地址 (如 "1.0.6")
    QString   destAddress;      // 目的地址 (如 "2/0/11" 或 "1.0.11")
    bool      isGroupAddress;   // 目的地址是否为组地址
    bool      isStandardFormat;
    QString   messageCode;      // 消息类型 (如 L_Data.ind = 0x29)
    quint8    hopCount;         // 路由跳数 (0~6)
    KNXDPT    datalength;
    QString   dataType;         // 数据类型 (如 "DPT 1.001" 或 "Raw 2 Bytes")
    QVariant  dataValue;        // 解析后的实际值 (可以是 bool, int, float, 或 QByteArray)要结合数据和DPT表，来判断，DPT目前不提供维护。
    QString   dataHex;          // 原始数据十六进制字符串 (用于 Debug)
};



enum  class Appstate {
    Idle,//空闲
    Searching,
    Descripting,
    Connecting,
    Connected,
    Failure,
};
enum class ValueSendState{
    None,
    Success,
    Failure,
};
enum class MessageLevel{
    Normal,
    Warnning,
    Error,
    Message,
};

// ✅ 必须注册为元类型，才能在跨线程信号槽中使用
Q_DECLARE_METATYPE(KnxBusRecord)
Q_DECLARE_METATYPE(Appstate)
Q_DECLARE_METATYPE(MessageLevel)


#endif // BASICDATATYPE_H
