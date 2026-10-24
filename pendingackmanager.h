#ifndef PENDINGACKMANAGER_H
#define PENDINGACKMANAGER_H

#include <QObject>
#include <basicdatatype.h>
#include <QTimer>

class PendingAckManager : public QObject
{
    Q_OBJECT
public:
    explicit PendingAckManager(QObject *parent = nullptr);

    // 添加待确认记录（发送时调用）
    void addPending(quint8 sequenceNumber, const QByteArray &cemiFrame = QByteArray());

    // 收到 ACK 时移除记录
    void removePending(quint8 sequenceNumber);

    // 清空所有记录（比如断开连接时）
    void clearAll();

    // 设置超时时间（毫秒），默认 3000ms
    void setTimeout(int ms) { m_timeoutMs = ms; }

    //用于判断hush表中是否有sequenceNumber
    bool contains(quint8 sequenceNumber);

    //开始定时器
    void startScanTimer();
    void startScanTimer(int ms);
    //停止定时器
    void stopScanTimer();

signals:
    // 当某个序列号超时未收到 ACK 时，发出此信号
    void packetTimeout(quint8 sequenceNumber);

    // 当收到 ACK 确认成功时，发出此信号（可选）
    void packetAcknowledged(quint8 sequenceNumber);

private slots:

    void checkTimeouts();     // 定时器槽函数，用于扫描超时

private:
    int m_timeoutMs = 3000;
    QHash<quint8, PendingPacketInfo> m_pendingMap;
    QTimer *m_scanTimer;
};

#endif // PENDINGACKMANAGER_H
