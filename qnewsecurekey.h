#ifndef QNEWSECUREKEY_H
#define QNEWSECUREKEY_H

#include <QtCore/qshareddata.h>

#include <qnewbytearray.h>
#include <qmyknx.h>

QT_BEGIN_NAMESPACE

class QKnxSecureKeyData;
class MYLIB_EXPORT QKnxSecureKey final
{
public:
    enum class Type : quint8 {
        Private,
        Public,
        Invalid
    };
    QKnxSecureKey::Type type() const;

    QKnxSecureKey();
    ~QKnxSecureKey();

    bool isNull() const;
    bool isValid() const;

    QKnxByteArray bytes() const;
    static QKnxSecureKey fromBytes(QKnxSecureKey::Type type,
                                   const QKnxByteArray &data,
                                   quint16 index = 0);

    static QKnxSecureKey generatePrivateKey();

    static QKnxSecureKey publicKeyFromPrivate(const QKnxSecureKey &privateKey);
    static QKnxSecureKey publicKeyFromPrivate(const QKnxByteArray &privateKey);

    static void generateKeys(QKnxSecureKey *privateKey, QKnxSecureKey *publicKey);

    static QKnxByteArray sharedSecret(const QKnxSecureKey &privateKey,
                                      const QKnxSecureKey &peerPublicKey);
    static QKnxByteArray sharedSecret(const QKnxByteArray &privateKey,
                                      const QKnxByteArray &peerPublicKey);

    QKnxSecureKey(const QKnxSecureKey &other);
    QKnxSecureKey &operator=(const QKnxSecureKey &other);

    bool operator==(const QKnxSecureKey &other) const;
    bool operator!=(const QKnxSecureKey &other) const;

private:
    QSharedDataPointer<QKnxSecureKeyData> d_ptr;
};

QT_END_NAMESPACE

#endif
