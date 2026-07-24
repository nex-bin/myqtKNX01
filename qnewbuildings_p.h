#ifndef QNEWBUILDINGS_P_H
#define QNEWBUILDINGS_P_H


#include <QtCore/qstring.h>
#include <QtCore/qvector.h>
#include <qnewprojectutils_p.h>

QT_BEGIN_NAMESPACE

struct Q_KNX_EXPORT QKnxGroupAddressRef
{
    Q_DECLARE_TR_FUNCTIONS(QKnxGroupAddressRef)

public:
    QString Id; // non-colonized name, pattern [\i-[:]][\c-[:]]*
    QString Name; // 255 character max.
    QString RefId; // non-colonized name, pattern [\i-[:]][\c-[:]]*
    QString Role; // optional, 255 character max.
    qint32 Puid { 0 };

    bool parseElement(QXmlStreamReader *reader, bool pedantic);
};

struct Q_KNX_EXPORT QKnxFunction
{
    Q_DECLARE_TR_FUNCTIONS(QKnxFunction)

public:
    QString Id; // non-colonized name, pattern [\i-[:]][\c-[:]]*
    QString Name; // 255 character max.
    QString Type; // optional, Building, BuildingPart, Floor, Room, DistributionBoard, Stairway, Corridor
    QString Number; // optional, 255 character max.
    QString Comment; // optional
    QString Description; // optional

    // Undefined, Editing, FinishedDesign, FinishedCommissioning, Tested, Accepted, Locked
    QString CompletionStatus { QLatin1String("Undefined") }; // optional

    QString DefaultGroupRange; // optional, non-colonized name, pattern [\i-[:]][\c-[:]]*
    qint32 Puid { 0 };

    QVector<QKnxGroupAddressRef> GroupAddressRef; // 0..n

    bool parseElement(QXmlStreamReader *reader, bool pedantic);
};

struct Q_KNX_EXPORT QKnxBuildingPart
{
    Q_DECLARE_TR_FUNCTIONS(QKnxBuildingPart)

public:
    QString Id; // non-colonized name, pattern [\i-[:]][\c-[:]]*
    QString Name; // 255 character max.
    QString Type; // Building, BuildingPart, Floor, Room, DistributionBoard, Stairway, Corridor
    QString Usage; // optional
    QString Number; // optional, 255 character max.

    QString Comment; // optional
    QString Description; // optional

    // Undefined, Editing, FinishedDesign, FinishedCommissioning, Tested, Accepted, Locked
    QString CompletionStatus { QLatin1String("Undefined") }; // optional

    QString DefaultLine; // optional
    qint32 Puid { 0 };

    QVector<QKnxBuildingPart> BuildingPart; // 0..n
    QVector<QString> DeviceInstanceRef; // 0..n, non-colonized name, pattern [\i-[:]][\c-[:]]*
    QVector<QKnxFunction> Function; // 0..n

    bool parseElement(QXmlStreamReader *reader, bool pedantic);
};

struct Q_KNX_EXPORT QKnxBuildings
{
    Q_DECLARE_TR_FUNCTIONS(QKnxBuildings)

public:
    QVector<QKnxBuildingPart> BuildingPart; // 0..n

    bool parseElement(QXmlStreamReader *reader, bool pedantic);
};

QT_END_NAMESPACE

#endif
