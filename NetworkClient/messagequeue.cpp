#include "messagequeue.h"

Messagequeue::Messagequeue(QObject *parent): QObject(parent) {
    //队列和互斥锁自动初始化
}

void Messagequeue::enqueue(const QJsonObject &message)
{
    //QMutexLocker 自动加锁，函数结束时自动解锁
    //防止多线程同时操作队列导致数据混乱
    QMutexLocker locker(&m_mutex);

    m_queue.enqueue(message);
}

QJsonObject Messagequeue::dequeue()
{
    QMutexLocker locker(&m_mutex);
    if(m_queue.isEmpty()){
        return QJsonObject();
    }

    QJsonObject message = m_queue.dequeue();

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

    m_queue.clear();
}



