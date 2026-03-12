#include "messagequeue.h"

Messagequeue::Messagequeue(QObject *parent): QObject(parent) {
    //队列和互斥锁自动初始化
    qDebug()<<"MessageQueue:消息队列已创建";
}

void Messagequeue::enqueue(const QJsonObject &message)
{
    //QMutexLocker 自动加锁，函数结束时自动解锁
    //防止多线程同时操作队列导致数据混乱
    QMutexLocker locker(&m_mutex);

    m_queue.enqueue(message);

    qDebug()<<"MessageQueue:消息已入队，当前队列大小："<<m_queue.size();
}

QJsonObject Messagequeue::dequeue()
{
    QMutexLocker locker(&m_mutex);
    if(m_queue.isEmpty()){
        qWarning()<<"MessageQueue:队列为空，无法出队";
        return QJsonObject();
    }

    QJsonObject message = m_queue.dequeue();
    qDebug()<<"MessageQueue:消息已出队，当前队列大小："<<m_queue.size();

    return message;
}

QVector<QJsonObject> Messagequeue::dequeueAll()
{
    QMutexLocker locker(&m_mutex);
    QVector<QJsonObject> allMessage;

    //一次性取出所有的消息
    while(!m_queue.isEmpty()){
        allMessage.append(m_queue.dequeue());
    }

    return allMessage;
}

bool Messagequeue::isEmpty() const
{
    QMutexLocker locker(&m_mutex);
    return m_queue.isEmpty();
}

int Messagequeue::size()
{
    QMutexLocker locker(&m_mutex);
    return m_queue.size();
}

void Messagequeue::clear()
{
    QMutexLocker locker(&m_mutex);

    int oldSize = m_queue.size();
    m_queue.clear();

    qDebug()<<"MessageQueue:d队列已清空，清除"<<oldSize<<"条消息";
}



