/****************************************************************************
** Meta object code from reading C++ file 'datamanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/DataManager/datamanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'datamanager.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.10.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN11DataManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto DataManager::qt_create_metaobjectdata<qt_meta_tag_ZN11DataManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DataManager",
        "connectionStateChanged",
        "",
        "operationResult",
        "ok",
        "message",
        "operationTip",
        "personAdded",
        "id",
        "personDeleted",
        "attendanceRecordAdded",
        "deviceStatusChanged",
        "devicdId",
        "status",
        "deviceRecordChanged",
        "deviceId",
        "faceDataAdded",
        "employeeId",
        "faceDataUpdated",
        "faceDataDeleted",
        "addPerson",
        "name",
        "department",
        "position",
        "deletePerson",
        "getAllPerson",
        "QList<QObject*>",
        "getPersonById",
        "getPersonByEmployeeId",
        "selectPersons",
        "addAttendanceRecord",
        "checkTime",
        "selectAttendanceRecord",
        "employee_id",
        "startTime",
        "endTime",
        "personName",
        "addOrUpdateDevice",
        "deviceName",
        "ipAddress",
        "updateDeviceStatus",
        "getAllDevices",
        "getDeviceById",
        "selectDevice",
        "addFaceDataByEmployeeId",
        "featureVector",
        "updateFaceDataByEmployeeId",
        "deleteFaceDataByEmployeeId",
        "getFaceDataByEmployeeId",
        "getAllFaceData",
        "getAllFaceDataWithFeature",
        "selectFaceData",
        "createdStart",
        "createdEnd",
        "updatedStart",
        "updatedEnd",
        "isConnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'operationResult'
        QtMocHelpers::SignalData<void(bool, const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 4 }, { QMetaType::QString, 5 },
        }}),
        // Signal 'operationTip'
        QtMocHelpers::SignalData<void(const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'personAdded'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'personDeleted'
        QtMocHelpers::SignalData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'attendanceRecordAdded'
        QtMocHelpers::SignalData<void(int)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Signal 'deviceStatusChanged'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 }, { QMetaType::QString, 13 },
        }}),
        // Signal 'deviceRecordChanged'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Signal 'faceDataAdded'
        QtMocHelpers::SignalData<void(int, const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::QString, 17 },
        }}),
        // Signal 'faceDataUpdated'
        QtMocHelpers::SignalData<void(int, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::QString, 17 },
        }}),
        // Signal 'faceDataDeleted'
        QtMocHelpers::SignalData<void(int)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 },
        }}),
        // Method 'addPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 21 }, { QMetaType::QString, 17 }, { QMetaType::QString, 22 }, { QMetaType::QString, 23 },
        }}),
        // Method 'deletePerson'
        QtMocHelpers::MethodData<bool(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 17 },
        }}),
        // Method 'getAllPerson'
        QtMocHelpers::MethodData<QList<QObject*>()>(25, 2, QMC::AccessPublic, 0x80000000 | 26),
        // Method 'getPersonById'
        QtMocHelpers::MethodData<QObject *(int)>(27, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::Int, 8 },
        }}),
        // Method 'getPersonByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(28, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 17 },
        }}),
        // Method 'selectPersons'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(29, 2, QMC::AccessPublic, 0x80000000 | 26, {{
            { QMetaType::QString, 21 }, { QMetaType::QString, 17 }, { QMetaType::QString, 22 }, { QMetaType::QString, 23 },
        }}),
        // Method 'addAttendanceRecord'
        QtMocHelpers::MethodData<bool(const QString &, const QDateTime &, const QString &, const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 17 }, { QMetaType::QDateTime, 31 }, { QMetaType::QString, 15 }, { QMetaType::QString, 13 },
        }}),
        // Method 'selectAttendanceRecord'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QString &, const QString &, const QString &)>(32, 2, QMC::AccessPublic, 0x80000000 | 26, {{
            { QMetaType::QString, 33 }, { QMetaType::QDateTime, 34 }, { QMetaType::QDateTime, 35 }, { QMetaType::QString, 15 },
            { QMetaType::QString, 13 }, { QMetaType::QString, 36 },
        }}),
        // Method 'addOrUpdateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(37, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QString, 38 }, { QMetaType::QString, 39 }, { QMetaType::QString, 13 },
        }}),
        // Method 'updateDeviceStatus'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(40, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QString, 13 },
        }}),
        // Method 'getAllDevices'
        QtMocHelpers::MethodData<QList<QObject*>()>(41, 2, QMC::AccessPublic, 0x80000000 | 26),
        // Method 'getDeviceById'
        QtMocHelpers::MethodData<QObject *(const QString &)>(42, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'selectDevice'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(43, 2, QMC::AccessPublic, 0x80000000 | 26, {{
            { QMetaType::QString, 15 }, { QMetaType::QString, 38 }, { QMetaType::QString, 39 }, { QMetaType::QString, 13 },
        }}),
        // Method 'addFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(44, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 17 }, { QMetaType::QByteArray, 45 },
        }}),
        // Method 'updateFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(46, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 17 }, { QMetaType::QByteArray, 45 },
        }}),
        // Method 'deleteFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &)>(47, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 17 },
        }}),
        // Method 'getFaceDataByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(48, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 17 },
        }}),
        // Method 'getAllFaceData'
        QtMocHelpers::MethodData<QList<QObject*>()>(49, 2, QMC::AccessPublic, 0x80000000 | 26),
        // Method 'getAllFaceDataWithFeature'
        QtMocHelpers::MethodData<QList<QObject*>()>(50, 2, QMC::AccessPublic, 0x80000000 | 26),
        // Method 'selectFaceData'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QDateTime &, const QDateTime &)>(51, 2, QMC::AccessPublic, 0x80000000 | 26, {{
            { QMetaType::QString, 17 }, { QMetaType::QDateTime, 52 }, { QMetaType::QDateTime, 53 }, { QMetaType::QDateTime, 54 },
            { QMetaType::QDateTime, 55 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(56, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DataManager, qt_meta_tag_ZN11DataManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DataManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11DataManagerE_t>.metaTypes,
    nullptr
} };

void DataManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DataManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectionStateChanged(); break;
        case 1: _t->operationResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->operationTip((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->personAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->personDeleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->attendanceRecordAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->deviceStatusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->deviceRecordChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->faceDataAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->faceDataUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->faceDataDeleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 11: { bool _r = _t->addPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->deletePerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { QList<QObject*> _r = _t->getAllPerson();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 14: { QObject* _r = _t->getPersonById((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 15: { QObject* _r = _t->getPersonByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 16: { QList<QObject*> _r = _t->selectPersons((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->addAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { QList<QObject*> _r = _t->selectAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[6])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 19: { bool _r = _t->addOrUpdateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 20: { bool _r = _t->updateDeviceStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 21: { QList<QObject*> _r = _t->getAllDevices();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 22: { QObject* _r = _t->getDeviceById((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 23: { QList<QObject*> _r = _t->selectDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 24: { bool _r = _t->addFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 25: { bool _r = _t->updateFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 26: { bool _r = _t->deleteFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 27: { QObject* _r = _t->getFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 28: { QList<QObject*> _r = _t->getAllFaceData();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 29: { QList<QObject*> _r = _t->getAllFaceDataWithFeature();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 30: { QList<QObject*> _r = _t->selectFaceData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[5])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::connectionStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(bool , const QString & )>(_a, &DataManager::operationResult, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(const QString & )>(_a, &DataManager::operationTip, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::personAdded, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::personDeleted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::attendanceRecordAdded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(const QString & , const QString & )>(_a, &DataManager::deviceStatusChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(const QString & )>(_a, &DataManager::deviceRecordChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int , const QString & )>(_a, &DataManager::faceDataAdded, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int , const QString & )>(_a, &DataManager::faceDataUpdated, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::faceDataDeleted, 10))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isConnected(); break;
        default: break;
        }
    }
}

const QMetaObject *DataManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 31)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 31;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 31)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 31;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void DataManager::connectionStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DataManager::operationResult(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DataManager::operationTip(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DataManager::personAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void DataManager::personDeleted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DataManager::attendanceRecordAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void DataManager::deviceStatusChanged(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void DataManager::deviceRecordChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void DataManager::faceDataAdded(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void DataManager::faceDataUpdated(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}

// SIGNAL 10
void DataManager::faceDataDeleted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}
QT_WARNING_POP
