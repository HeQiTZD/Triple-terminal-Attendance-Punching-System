#pragma once

#include <QObject>
#include <QDateTime>
#include "../DataManager/datamanager.h"

class ExportManager : public QObject {
	Q_OBJECT
	Q_PROPERTY(QString lastError READ lastError NOTIFY lastErrorChanged)

public:
	explicit ExportManager(DataManager* dataManager, QObject* parent = nullptr);

	QString lastError() const { return m_lastError; }

	// 导出：考勤记录（按时间范围）
	Q_INVOKABLE bool exportAttendanceRecordsCvs(const QString& filePath,
												const QDateTime& startTime,
												const QDateTime& endTime);

	// 导出：人员列表
	Q_INVOKABLE bool exportPersonsCsv(const QString& filePath);

	// 导出：设备列表
	Q_INVOKABLE bool exportDeviceCsv(const QString& filePath);

signals:
	void lastErrorChanged();

private:
	DataManager* m_dataManager = nullptr;
	QString m_lastError;

	void setError(const QString& err);

	static QString csvEscape(const QString& s);
	static bool ensurePersontDir(const QString& filePath, QString* err);
	static bool writeUht8CsvFile(const QString& filePath,
								 const QString& csvText,
								 QString* err,
							 	 bool withUtf8Bom = true);
};