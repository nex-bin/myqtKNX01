#ifndef BASICDATATYPE_H
#define BASICDATATYPE_H

#include "qmyknx.h"
#include "QString"
#include "QHostAddress"
#include <QMetaType>

namespace comBasicDateType{


}

struct MYLIB_EXPORT DeviceInfo{
    QString deviceStatus_text;
    QHostAddress address;
    qint16 port;
};//用来传递发现的设备的信息。
struct MYLIB_EXPORT ips{
    QString ip;
    QString iplocal;
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

Q_DECLARE_METATYPE(Appstate)
Q_DECLARE_METATYPE(MessageLevel)


#endif // BASICDATATYPE_H
