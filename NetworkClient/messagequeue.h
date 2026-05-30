#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H

#include <QObject>
#include <QJsonObject>
#include <QQueue>
#include <QMutex>
class MessageQueue : public QObject
{
    Q_OBJECT

public:
    explicit MessageQueue(QObject *parent = nullptr);

    //入队
    void enqueue(const QJsonObject &meaage);

    //出队
    QJsonObject dequeue();
    QVector<QJsonObject> dequeueAll();

    //查询
    bool isEmpty() const;
    int size();
    void clear();

private:
    QQueue<QJsonObject> m_queue;
    mutable QMutex m_mutex;
};

#endif // MESSAGEQUEUE_H
