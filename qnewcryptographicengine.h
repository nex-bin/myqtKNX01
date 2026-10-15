#ifndef QNEWCRYPTOGRAPHICENGINE_H
#define QNEWCRYPTOGRAPHICENGINE_H

#include <QtCore/qshareddata.h>

#include <qnewbytearray.h>
#include <netipframe.h>
#include <qnewsecurekey.h>

QT_BEGIN_NAMESPACE

class MYLIB_EXPORT QKnxCryptographicEngine final
{
public:
    QKnxCryptographicEngine() = delete;
    ~QKnxCryptographicEngine() = default;

    static bool supportsCryptography();
    static long sslLibraryVersionNumber();

    static QKnxByteArray sessionKey(const QKnxSecureKey &privateKey,
                                    const QKnxSecureKey &peerPublicKey);
    static QKnxByteArray sessionKey(const QKnxByteArray &privateKey,
                                    const QKnxByteArray &peerPublicKey);
    static QKnxByteArray sessionKey(const QKnxByteArray &sharedSecret);

    static QKnxByteArray userPasswordHash(const QByteArray &password);
    static QKnxByteArray keyringPasswordHash(const QByteArray &password);
    static QKnxByteArray deviceAuthenticationCodeHash(const QByteArray &password);

    static QKnxByteArray hashSha256(const QByteArray &data);
    static QKnxByteArray XOR(const QKnxByteArray &l, const QKnxByteArray &r, bool adjust = true);

    static QKnxByteArray computeMessageAuthenticationCode(const QKnxByteArray &key,
                                                          const QKnxNetIpFrameHeader &header,
                                                          quint16 id,
                                                          const QKnxByteArray &data,
                                                          quint48 sequenceNumber = 0,
                                                          const QKnxByteArray &serialNumber = {},
                                                          quint16 messageTag = 0);

    static QKnxByteArray decrypt(const QKnxByteArray &key, const QKnxByteArray &iv,
                                 const QKnxByteArray &data);
    static QKnxByteArray encrypt(const QKnxByteArray &key, const QKnxByteArray &iv,
                                 const QKnxByteArray &data);

    static QKnxByteArray encryptSecureWrapperPayload(const QKnxByteArray &key,
                                                     const QKnxNetIpFrame &frame,
                                                     quint48 sequenceNumber,
                                                     const QKnxByteArray &serialNumber,
                                                     quint16 messageTag);

    static QKnxByteArray decryptSecureWrapperPayload(const QKnxByteArray &key,
                                                     const QKnxByteArray &frame,
                                                     quint48 sequenceNumber,
                                                     const QKnxByteArray &serialNumber,
                                                     quint16 messageTag);

    static QKnxByteArray encryptMessageAuthenticationCode(const QKnxByteArray &key,
                                                          const QKnxByteArray &mac,
                                                          quint48 sequenceNumber = 0,
                                                          const QKnxByteArray &serialNumber = {},
                                                          quint16 messageTag = 0);

    static QKnxByteArray decryptMessageAuthenticationCode(const QKnxByteArray &key,
                                                          const QKnxByteArray &data,
                                                          quint48 sequenceNumber = 0,
                                                          const QKnxByteArray &serialNumber = {},
                                                          quint16 messageTag = 0);

    static QKnxByteArray decodeAndDecryptToolKey(const QKnxByteArray &passwordHash,
                                                 const QKnxByteArray &createdHash,
                                                 const QByteArray &toolKey);
    static QKnxByteArray decodeAndDecryptPassword(const QKnxByteArray &passwordHash,
                                                  const QKnxByteArray &createdHash,
                                                  const QByteArray &password);
};

QT_END_NAMESPACE

#endif
