#pragma once

#include <QJsonObject>
#include <QObject>
#include <QString>

class AttendanceConfigSyncHandler : public QObject {
    Q_OBJECT

public:
    static AttendanceConfigSyncHandler* instance();

    void requestConfig();
    bool initialize();
    bool applyConfig(const QJsonObject &config);

    QString getCurrentChecksum() const;
    bool isConfigLoaded() const { return m_configLoaded; }

signals:
    void configReceived(const QJsonObject &config);
    void configApplied();
    void configApplyFailed(const QString &reason);

private:
    explicit AttendanceConfigSyncHandler(QObject *parent = nullptr);

    bool verifyChecksum(const QJsonObject &config) const;
    bool verifySignature(const QJsonObject &config) const;
    bool saveToFile(const QJsonObject &config);
    QJsonObject loadFromFile() const;
    QString getConfigFilePath() const;
    QString calculateChecksum(const QJsonObject &configObject) const;
    QJsonObject normalizedConfigWithChecksum() const;

    bool m_configLoaded = false;
    QString m_currentChecksum;
};
