#pragma once

#include <QByteArray>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVector>

#include "../LocalStorage/localstorage.h"
#include "../NetworkClient/serverprotocol.h"

/// 管理全量同步流程：sync.request → person.sync → face.sync.* → sync.ack
class SyncManager : public QObject
{
    Q_OBJECT

public:
    enum class State {
        Idle,           // 空闲，可发起同步
        SyncingPersons,  // 接收 person.sync 批次中
        SyncingFaces,   // 接收 face.sync.item.header + binary 中
        Committing      // face.sync.end 后正在提交代次
    };

    explicit SyncManager(QObject *parent = nullptr);

    State state() const { return m_state; }

public slots:
    /// 发起同步请求（需要 Networkclient 已连接且已认证）
    void requestSync();

    /// 接收 person.sync 消息
    void handlePersonSync(const QJsonObject &message);

    /// 接收 face.sync.begin
    void handleFaceSyncBegin(const QJsonObject &message);

    /// 接收 face.sync.item.header + 二进制特征
    void handleFaceItem(const QJsonObject &header, const QByteArray &payload);

    /// 接收 face.sync.end
    void handleFaceSyncEnd(const QJsonObject &message);

signals:
    void syncStarted();
    void syncCompleted();
    void syncFailed(const QString &reason);

    void personSyncProgress(int count);
    void faceSyncProgress(int count);

    /// 需要发送 JSON 消息（连接到 Networkclient::send）
    void sendMessage(const QJsonObject &message);

    /// 同步完成后触发人脸库重载
    void requestFaceDbReload();

private:
    void setState(State s);
    void doCommit();
    void sendSyncAck(const QString &status, const QString &message = {});

    State  m_state = State::Idle;

    QString m_syncRequestMsgId;   // 本次 sync.request 的 msgId
    int     m_stagingGeneration = 0;
    int     m_personCount       = 0;
    int     m_faceCount         = 0;
    int     m_faceWritten       = 0;
};
