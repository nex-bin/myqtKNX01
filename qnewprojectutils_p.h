#ifndef QKNXPROJECTUTILS_P_H
#define QKNXPROJECTUTILS_P_H

#include <QtCore/qcoreapplication.h>
#include <QtCore/qxmlstream.h>
#include <qmyknx.h>

QT_BEGIN_NAMESPACE

struct Q_KNX_EXPORT QKnxProjectUtils final
{
    Q_DECLARE_TR_FUNCTIONS(QKnxProjectUtils)

public:
    static bool isNCName(const QString &candidate);

    static bool setNCName(const QString &name, const QStringRef &attr, QString *field,
                          QXmlStreamReader *reader, bool pedantic);

    static bool fetchAttr(const QXmlStreamAttributes &attributes, const QString &attrName,
                          QStringRef *value, QXmlStreamReader *reader);

    static bool setString(const QString &name, const QStringRef &attr, int maxSize,
                          QString *field, QXmlStreamReader *reader, bool pedantic);

    static bool setString(const QString &name, const QStringRef &attr, const QStringList &list,
                          QString *field, QXmlStreamReader *reader, bool pedantic);

    template< typename T> static bool setNumber(const QString &name, const T &attr, T maxValue,
                          T *field, QXmlStreamReader *reader, bool pedantic)
    {
        if (!reader || !field)
            return false;

        if (pedantic && attr > maxValue) {
            reader->raiseError(tr("Pedantic error: Invalid value for attribute '%1', maximum "
                                  "value is %2 characters, got: '%3'.").arg(name).arg(maxValue).arg(attr));
        } else {
            *field = attr;
        }
        return !reader->hasError();
    }

    template<typename T>
    static bool parseChildElement(QXmlStreamReader *reader, bool pedantic, QVector<T> *container)
    {
        while (!reader->atEnd() && reader->readNextStartElement()) {
            T element;
            if (!element.parseElement(reader, pedantic))
                break;
            container->append(element);
        }
        return !reader->hasError();
    }
};

QT_END_NAMESPACE

#endif
