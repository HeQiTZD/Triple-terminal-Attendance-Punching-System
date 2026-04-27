#include "exportmanager.h"

ExportManager::ExportManager(DataManager* dataManager, QObject* parent)
	: QObject(parent),m_dataManager(dataManager)
{
}

Q_INVOKABLE bool ExportManager::exportAttendanceRecordsCsv(const QString& filePath, const QDateTime& startTime, const QDateTime& endTime)
{
	if (!m_dataManager) {
		setError("DataManager 未初始化");
		return false;
	}
	if (!m_dataManager->isConnected()) {
		setError("数据库未连接");
		return false;
	}

	const QList<QObject*> attendaceRecords = m_dataManager->getAttendanceRecords(startTime,endTime);

	QString csv;
	csv += "id,person_id,employee_id,name,department,check_time,device_id,status,received_time\n";
	for (QObject* obj : attendaceRecords) {
		auto* attendaceRecord = qobject_cast<AttendanceRecord*>(obj);
		if (!attendaceRecord) continue;

		// 尽可能补全人员信息（不强依赖：查不到也能导出）
		QString employeeId, name, department;
		if (QObject* personObj = m_dataManager->getPersonById(attendaceRecord->personId())) {
			employeeId = personObj->property("employeeId").toString();
			name = personObj->property("name").toString();
			department = personObj->property("department").toString();
		}
		csv += csvEscape(QString::number(attendaceRecord->id())) + ",";
		csv += csvEscape(QString::number(attendaceRecord->personId())) + ",";
		csv += csvEscape(employeeId) + ",";
		csv += csvEscape(name) + ",";
		csv += csvEscape(department) + ",";
		csv += csvEscape(attendaceRecord->checkTime().toString(Qt::ISODate)) + ",";
		csv += csvEscape(attendaceRecord->deviceId()) + ",";
		csv += csvEscape(attendaceRecord->status()) + ",";
		csv += csvEscape(attendaceRecord->receivedTime().toString(Qt::ISODate)) + "\n";
	}

	QString err;
	if (!writeUht8CsvFile(filePath, csv, &err, true)) {
		setError(err);
		return false;
	}

	setError(QString());
	return true;
}

Q_INVOKABLE bool ExportManager::exportPersonsCsv(const QString& filePath)
{
	//检查数据库是否初始化并正在连接
	if (!m_dataManager) {
		setError("DataManager 未初始化"); 
		return false;
	}
	if (!m_dataManager->isConnected()) {
		setError("数据库未连接");
		return false;
	}

	//获取所有人员数据，人员数据列表
	const QList<QObject*> persons = m_dataManager->getAllPerson();

	/*
		把人员数据拼成一份 CSV 文本
		先写入表头：id,name,employee_id,department,position,created_at,updated_at
		遍历每个对象，qobject_cast<Person*> 转成 Person*（不是 Person 就跳过）
		依次取出 id/name/employeeId/department/position/createdAt/updatedAt
		用 csvEscape(...) 做 CSV 字段转义（避免逗号、引号、换行导致格式错误）
		时间用 Qt::ISODate 输出成 ISO 格式字符串
		每个人一行（最后加 \n）
	*/
	QString csv;
	csv += "id,name,employee_id,department,position,created_at,updated_at\n";

	for (QObject* obj : persons) {
		auto* person = qobject_cast<Person*>(obj);
		if (!person) continue;

		csv += csvEscape(QString::number(person->id())) + ",";
		csv += csvEscape(person->name()) + ",";
		csv += csvEscape(person->employeeId()) + ",";
		csv += csvEscape(person->department()) + ",";
		csv += csvEscape(person->position()) + ",";
		csv += csvEscape(person->createdAt().toString(Qt::ISODate)) + ",";
		csv += csvEscape(person->updatedAt().toString(Qt::ISODate)) + "\n";
	}

	//把 CSV 以 UTF-8（带 BOM）写入到 filePath 指定的文件
	QString err;
	if (!writeUht8CsvFile(filePath, csv, &err, true)) {
		setError(err);
		return false;
	}

	setError(QString());//写入成功，清空内部错误状态
	return true;
}

Q_INVOKABLE bool ExportManager::exportDeviceCsv(const QString& filePath)
{
	if (!m_dataManager) {
		setError("DataManager 未初始化");
		return false;
	}
	if (!m_dataManager->isConnected()) {
		setError("数据库未连接");
		return false;
	}

	const QList<QObject*> Devices = m_dataManager->getAllDevices();

	QString csv;
	csv += "id,device_id,device_name,ip_address,last_online,status\n";

	for (QObject* obj : Devices) {
		auto* device = qobject_cast<Device*>(obj);
		if (!device) continue;

		csv += csvEscape(QString::number(device->id())) + ",";
		csv += csvEscape(device->deviceId()) + ",";
		csv += csvEscape(device->deviceName()) + ",";
		csv += csvEscape(device->ipAddress()) + ",";
		csv += csvEscape(device->lastOnline().toString(Qt::ISODate)) + ",";
		csv += csvEscape(device->status()) + "\n";
	}

	QString err;
	if (!writeUht8CsvFile(filePath, csv, &err, true)) {
		setError(err);
		return false;
	}

	setError(QString());
	return true;
}

void ExportManager::setError(const QString& err)
{
	if (m_lastError == err) return;
	m_lastError = err;
	emit lastErrorChanged();
}

QString ExportManager::csvEscape(const QString& s)
{
	//CSV 规则：包含引号/逗号/换行 -> 用双引号包裹，内部 " 变成 ""
	const bool needQuote = s.contains(u'"') || s.contains(u',') || s.contains(u'\n') || s.contains(u'\r') || s.contains(u'\t');

	QString out = s;

	out.replace(u'"', "\"\"");
	return needQuote ? QString("\"%1\"").arg(out) : out;
}

bool ExportManager::ensurePersontDir(const QString& filePath, QString* err)
{
	QFileInfo fi(filePath);//构造 QFileInfo：用 filePath 初始化 fi，方便后续获取路径、文件名、所在目录等信息。
	const QString dirPath = fi.absolutePath();//获取filePath所在目录的绝对目录路径
	QDir d(dirPath);//构造目录对象：用 dirPath 初始化一个 QDir 对象 d，代表这个目录。
	if (d.exists()) return true;//如果目录已存在就直接成功返回：不需要创建。
	if(!d.mkpath(".")) {//mkpath(".") 表示“确保 d 当前指向的这个目录存在”。
		if (err) {
			*err = QString("无法创建目录：%1").arg(dirPath);
			return false;
		}
		return false;
	}
	return true;
}

bool ExportManager::writeUht8CsvFile(const QString& filePath, const QString& csvText, QString* err, bool withUtf8Bom)
{
	if (!ensurePersontDir(filePath, err)) return false;//确保目录存在
	
	QSaveFile f(filePath);//创建安全写文件对象：QSaveFile 会先写到临时文件，最后 commit() 再原子性替换目标文件，减少写到一半损坏的风险。
	if (!f.open(QIODevice::WriteOnly)) {//以只写模式打开文件
		if (err) {
			*err = QString("无法写入文件：%1").arg(filePath);
			return false;
		}
		return false;
	}

	if (withUtf8Bom) {
		/*
			定义了一个名为 bom 的常量无符号字符数组。
			数组的内容是三个字节：0xEF, 0xBB, 0xBF。这三个字节序列正是 UTF-8 编码的 BOM 标记。
		*/
		const unsigned char bom[] = { 0xEF,0xBB,0xBF };

		/*
			 将 bom 这个 unsigned char 类型的指针强制转换为 const char* 类型。
			 这是因为大多数文件写入函数（例如 QFile::write 或 std::ofstream::write，
			 期望接收 char* 类型的数据指针。
		*/
		f.write(reinterpret_cast<const char*>(bom), 3);//（写入的数据，写入的字节数）
	}

	f.write(csvText.toUtf8());//把 QString 转成 UTF-8 编码的字节序列（QByteArray）。


	/*
		提交保存
		commit() 会把临时文件内容“落盘”并替换到目标路径（安全写入的关键一步）。
		如果失败进入 if（比如磁盘满、权限问题、替换失败等）。
	*/
	if (!f.commit()) {
		if (err) *err = QString("保存失败：%1").arg(filePath);
		return false;
	}
	return true;
}