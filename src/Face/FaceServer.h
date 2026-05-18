#ifndef FACESERVER_H
#define FACESERVER_H

#include <QObject>
#include <QVariantList>
#include <QVariantMap>

class TcpConnectionManager;

class FaceServer :public QObject {
	Q_OBJECT
	Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
	Q_PROPERTY(QVariantList records READ records NOTIFY recordsChanged)
	Q_PROPERTY(bool lastFound READ lastFound NOTIFY lastFoundChanged)
	Q_PROPERTY(QVariantMap lastRecord READ lastRecord NOTIFY lastRecordChanged)

public:
	explicit FaceServer(QObject* parent = nullptr);
	void setTcpManager(TcpConnectionManager* tcp);

	bool busy() const { return m_busy; }
	QVariantList records() const { return m_records; }
	bool lastFound() const { return m_lastFound; }
	QVariantMap lastRecord() const { return m_lastRecord; }

	/** 按工号查询单条人脸元数据（不含特征向量） */
	Q_INVOKABLE void queryFace(const QString& employeeId);

	/** 删除指定工号的人脸特征 */
	Q_INVOKABLE void deleteFace(const QString& employeeId);

	/**
	 * 注册/更新人脸：photoBase64 为纯 Base64（不含 data: 前缀）
	 * overwrite=true 时覆盖已存在数据
	 */
	Q_INVOKABLE void registerFace(const QString& employeeId,
	                              const QString& photoBase64,
	                              bool overwrite = false);

	/** 从本地图片文件注册（QML FileDialog 的 file URL 或本地路径） */
	Q_INVOKABLE void registerFaceFromFile(const QString& employeeId,
										  const QString& filePathOrUrl,
										  bool overwrite = false);

	Q_INVOKABLE void clearRecords();

signals:
	void busyChanged();
	void recordsChanged();
	void lastFoundChanged();
	void lastRecordChanged();

	void operationSucceeded(const QString& apiType, const QString& message);
	void operationFailed(const QString& apiType, int code, const QString& message);

	/** 查询完成：found=false 时 record 为空 */
	void queryCompleted(bool found, const QVariantMap& record);

	/** 注册成功：含 featureSize、faceRect、isNew */
	void registerCompleted(const QVariantMap& result);

private:
	void setBusy(bool v);
	void setLastResult(bool found, const QVariantMap& record);
	void upsertRecord(const QVariantMap& row);
	void removeRecordByEmployeeId(const QString& employeeId);
	static QVariantMap parseQueryData(const QJsonObject& dataObj);
	static QVariantMap parseRegisterData(const QJsonObject& dataObj);
	static QString normalizeLocalPath(const QString& filePathOrUrl);
	static QString readFileAsBase64(const QString& localPath, QString* errorOut);
	static bool validateEmployeeId(const QString& employeeId, QString* errorOut);
	static bool validatePhotoBase64(const QString& photoBase64, QString* errorOut);

	TcpConnectionManager* m_tcp = nullptr;
	bool m_busy = false;
	bool m_lastFound = false;
	QVariantList m_records;
	QVariantMap m_lastRecord;
};

#endif // FACESERVER_H