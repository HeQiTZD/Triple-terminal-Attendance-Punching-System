#include "AttendanceConfigSyncHandler.h"

#include "AttendanceConfigParser.h"
#include "AttendanceConfigValidator.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>

AttendanceConfigSyncHandler* AttendanceConfigSyncHandler::instance()
{
    static AttendanceConfigSyncHandler handler;
    return &handler;
}

AttendanceConfigSyncHandler::AttendanceConfigSyncHandler(QObject *parent)
    : QObject(parent)
{
}

void AttendanceConfigSyncHandler::requestConfig()
{
}

bool AttendanceConfigSyncHandler::initialize()
{
    const QJsonObject savedConfig = loadFromFile();
    if (!savedConfig.isEmpty()) {
        return applyConfig(savedConfig);
    }

    const QJsonObject defaultConfig = normalizedConfigWithChecksum();
    if (!saveToFile(defaultConfig)) {
        emit configApplyFailed(QStringLiteral("默认考勤规则文件保存失败"));
        return false;
    }

    m_currentChecksum = defaultConfig.value(QStringLiteral("checksum")).toString();
    m_configLoaded = true;
    return true;
}

bool AttendanceConfigSyncHandler::applyConfig(const QJsonObject &config)
{
    emit configReceived(config);

    if (!verifyChecksum(config)) {
        const QString reason = QStringLiteral("考勤规则 checksum 校验失败");
        emit configApplyFailed(reason);
        return false;
    }

    if (!verifySignature(config)) {
        const QString reason = QStringLiteral("考勤规则 signature 校验失败");
        emit configApplyFailed(reason);
        return false;
    }

    const AttendanceValidationResult validation = AttendanceConfigValidator::instance()->validate(config);
    if (!validation.isValid) {
        emit configApplyFailed(validation.errorMessage);
        return false;
    }

    if (!AttendanceConfigParser::instance()->parseAttendanceConfig(config)) {
        emit configApplyFailed(QStringLiteral("考勤规则解析失败"));
        return false;
    }

    const QJsonObject normalized = normalizedConfigWithChecksum();
    if (!saveToFile(normalized)) {
        emit configApplyFailed(QStringLiteral("考勤规则文件保存失败"));
        return false;
    }

    m_currentChecksum = normalized.value(QStringLiteral("checksum")).toString();
    m_configLoaded = true;
    emit configApplied();
    return true;
}

QString AttendanceConfigSyncHandler::getCurrentChecksum() const
{
    return m_currentChecksum;
}

bool AttendanceConfigSyncHandler::verifyChecksum(const QJsonObject &config) const
{
    const QString receivedChecksum = config.value(QStringLiteral("checksum")).toString();
    if (receivedChecksum.isEmpty()) {
        return true;
    }

    const QJsonObject configObject = config.value(QStringLiteral("config")).toObject();
    if (configObject.isEmpty()) {
        return false;
    }

    return receivedChecksum == calculateChecksum(configObject);
}

bool AttendanceConfigSyncHandler::verifySignature(const QJsonObject &config) const
{
    Q_UNUSED(config)
    return true;
}

bool AttendanceConfigSyncHandler::saveToFile(const QJsonObject &config)
{
    const QString filePath = getConfigFilePath();
    QDir dir = QFileInfo(filePath).dir();
    if (!dir.exists() && !dir.mkpath(QStringLiteral("."))) {
        return false;
    }

    const QString backupPath = filePath + QStringLiteral(".bak");
    if (QFile::exists(filePath)) {
        QFile::remove(backupPath);
        QFile::copy(filePath, backupPath);
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        return false;
    }

    file.write(QJsonDocument(config).toJson(QJsonDocument::Indented));
    return true;
}

QJsonObject AttendanceConfigSyncHandler::loadFromFile() const
{
    QFile file(getConfigFilePath());
    if (!file.open(QIODevice::ReadOnly)) {
        return {};
    }

    QJsonParseError error;
    const QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError || !doc.isObject()) {
        return {};
    }

    return doc.object();
}

QString AttendanceConfigSyncHandler::getConfigFilePath() const
{
    return QCoreApplication::applicationDirPath() + QStringLiteral("/Config/attendance_rule.json");
}

QString AttendanceConfigSyncHandler::calculateChecksum(const QJsonObject &configObject) const
{
    const QByteArray data = QJsonDocument(configObject).toJson(QJsonDocument::Compact);
    const QByteArray digest = QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
    return QStringLiteral("sha256:%1").arg(QString::fromLatin1(digest));
}

QJsonObject AttendanceConfigSyncHandler::normalizedConfigWithChecksum() const
{
    QJsonObject root = AttendanceConfigParser::instance()->exportCurrentConfig();
    root[QStringLiteral("generatedAt")] = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);

    const QJsonObject configObject = root.value(QStringLiteral("config")).toObject();
    root[QStringLiteral("checksum")] = calculateChecksum(configObject);
    return root;
}
