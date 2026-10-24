#include "pendingackmanager.h"

PendingAckManager::PendingAckManager(QObject *parent)
    : QObject{parent}
{
    // 初始化定时器，每 3000 毫秒扫描一次
    m_scanTimer = new QTimer(this);
    connect(m_scanTimer, &QTimer::timeout, this, &PendingAckManager::checkTimeouts);
    m_scanTimer->start(m_timeoutMs);
}

void PendingAckManager::addPending(quint8 sequenceNumber, const QByteArray &cemiFrame)
{
    // 记录当前时间戳
    PendingPacketInfo info;
    info.sendTime = QDateTime::currentDateTime();
    info.cemiFrame = cemiFrame;

    m_pendingMap.insert(sequenceNumber, info);
    // qDebug() << "已添加待确认序列号:" << sequenceNumber;
}

void PendingAckManager::removePending(quint8 sequenceNumber)
{
    if (m_pendingMap.contains(sequenceNumber)) {
        m_pendingMap.remove(sequenceNumber);
        emit packetAcknowledged(sequenceNumber);
        qDebug() << "序列号" << sequenceNumber << "已确认，移除队列";
    } else {
        // 收到未记录的 ACK（可能是网关重发的旧ACK），忽略即可
    }
}

void PendingAckManager::clearAll()
{
    m_pendingMap.clear();
}

void PendingAckManager::checkTimeouts()
{
    QDateTime now = QDateTime::currentDateTime();
    QList<quint8> timeouts;

    // 遍历查找超时的序列号
    for (auto it = m_pendingMap.constBegin(); it != m_pendingMap.constEnd(); ++it) {
        qint64 elapsed = it.value().sendTime.msecsTo(now);
        if (elapsed > m_timeoutMs) {
            timeouts.append(it.key());
        }
    }

    // 统一处理超时，避免在遍历时修改容器
    for (quint8 seq : timeouts) {
        m_pendingMap.remove(seq);
        qWarning() << "!  序列号" << seq << "超时未收到 ACK!";
        emit packetTimeout(seq);   // 发出超时信号，让主程序决定是否重发
    }
}
bool PendingAckManager::contains(quint8 sequenceNumber){

    return m_pendingMap.contains(sequenceNumber);

}