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
        "errorOccurred",
        "errorString",
        "personAdded",
        "id",
        "personUpdated",
        "personDeleted",
        "attendanceRecordAdded",
        "deviceStatusChanged",
        "devicdId",
        "status",
        "faceDataAdded",
        "personId",
        "faceDataUpdated",
        "faceDataDeleted",
        "addPerson",
        "name",
        "employeeId",
        "department",
        "position",
        "updatedPerson",
        "deletePerson",
        "getAllPerson",
        "QList<QObject*>",
        "getPersonById",
        "getPersonByEmployeeId",
        "updatePersonFaceFeature",
        "faceFeature",
        "addAttendanceRecore",
        "checkTime",
        "deviceId",
        "getAttendanceRecords",
        "startTime",
        "endTime",
        "getAttendanceRecordsByPerson",
        "addOrUpdateDevice",
        "deviceName",
        "ipAddress",
        "updateDeviceStatus",
        "getAllDevices",
        "getDeviceById",
        "addFaceDataByEmployeeId",
        "featureVector",
        "updateFaceDataByEmployeeId",
        "deleteFaceDataByEmployeeId",
        "getFaceDataByEmployeeId",
        "getAllFaceData",
        "isConnected"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(3, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 4 },
        }}),
        // Signal 'personAdded'
        QtMocHelpers::SignalData<void(int)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'personUpdated'
        QtMocHelpers::SignalData<void(int)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'personDeleted'
        QtMocHelpers::SignalData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'attendanceRecordAdded'
        QtMocHelpers::SignalData<void(int)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Signal 'deviceStatusChanged'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 }, { QMetaType::QString, 12 },
        }}),
        // Signal 'faceDataAdded'
        QtMocHelpers::SignalData<void(int, int)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::Int, 14 },
        }}),
        // Signal 'faceDataUpdated'
        QtMocHelpers::SignalData<void(int, int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 }, { QMetaType::Int, 14 },
        }}),
        // Signal 'faceDataDeleted'
        QtMocHelpers::SignalData<void(int)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 6 },
        }}),
        // Method 'addPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString, const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 19 }, { QMetaType::QString, 20 }, { QMetaType::QString, 21 },
        }}),
        // Method 'updatedPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 19 }, { QMetaType::QString, 20 }, { QMetaType::QString, 21 },
        }}),
        // Method 'deletePerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(23, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 19 },
        }}),
        // Method 'getAllPerson'
        QtMocHelpers::MethodData<QList<QObject*>()>(24, 2, QMC::AccessPublic, 0x80000000 | 25),
        // Method 'getPersonById'
        QtMocHelpers::MethodData<QObject *(int)>(26, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::Int, 6 },
        }}),
        // Method 'getPersonByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(27, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 19 },
        }}),
        // Method 'updatePersonFaceFeature'
        QtMocHelpers::MethodData<bool(int, const QByteArray &)>(28, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 6 }, { QMetaType::QByteArray, 29 },
        }}),
        // Method 'addAttendanceRecore'
        QtMocHelpers::MethodData<bool(int, const QDateTime &, const QString &, const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 14 }, { QMetaType::QDateTime, 31 }, { QMetaType::QString, 32 }, { QMetaType::QString, 12 },
        }}),
        // Method 'getAttendanceRecords'
        QtMocHelpers::MethodData<QList<QObject*>(const QDateTime &, const QDateTime &)>(33, 2, QMC::AccessPublic, 0x80000000 | 25, {{
            { QMetaType::QDateTime, 34 }, { QMetaType::QDateTime, 35 },
        }}),
        // Method 'getAttendanceRecordsByPerson'
        QtMocHelpers::MethodData<QList<QObject*>(int, const QDateTime &, const QDateTime &)>(36, 2, QMC::AccessPublic, 0x80000000 | 25, {{
            { QMetaType::Int, 14 }, { QMetaType::QDateTime, 34 }, { QMetaType::QDateTime, 35 },
        }}),
        // Method 'addOrUpdateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(37, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 32 }, { QMetaType::QString, 38 }, { QMetaType::QString, 39 }, { QMetaType::QString, 12 },
        }}),
        // Method 'updateDeviceStatus'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(40, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 32 }, { QMetaType::QString, 12 },
        }}),
        // Method 'getAllDevices'
        QtMocHelpers::MethodData<QList<QObject*>()>(41, 2, QMC::AccessPublic, 0x80000000 | 25),
        // Method 'getDeviceById'
        QtMocHelpers::MethodData<QObject *(const QString &)>(42, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 32 },
        }}),
        // Method 'addFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(43, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 19 }, { QMetaType::QByteArray, 44 },
        }}),
        // Method 'updateFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(45, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 19 }, { QMetaType::QByteArray, 44 },
        }}),
        // Method 'deleteFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 19 },
        }}),
        // Method 'getFaceDataByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(47, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 19 },
        }}),
        // Method 'getAllFaceData'
        QtMocHelpers::MethodData<QList<QObject*>()>(48, 2, QMC::AccessPublic, 0x80000000 | 25),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(49, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
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
        case 1: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->personAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 3: _t->personUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->personDeleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->attendanceRecordAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 6: _t->deviceStatusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->faceDataAdded((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 8: _t->faceDataUpdated((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 9: _t->faceDataDeleted((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 10: { bool _r = _t->addPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->updatedPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->deletePerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { QList<QObject*> _r = _t->getAllPerson();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 14: { QObject* _r = _t->getPersonById((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 15: { QObject* _r = _t->getPersonByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->updatePersonFaceFeature((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->addAttendanceRecore((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { QList<QObject*> _r = _t->getAttendanceRecords((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 19: { QList<QObject*> _r = _t->getAttendanceRecordsByPerson((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 20: { bool _r = _t->addOrUpdateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 21: { bool _r = _t->updateDeviceStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { QList<QObject*> _r = _t->getAllDevices();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 23: { QObject* _r = _t->getDeviceById((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
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
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)()>(_a, &DataManager::connectionStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(const QString & )>(_a, &DataManager::errorOccurred, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::personAdded, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::personUpdated, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::personDeleted, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::attendanceRecordAdded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(const QString & , const QString & )>(_a, &DataManager::deviceStatusChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int , int )>(_a, &DataManager::faceDataAdded, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int , int )>(_a, &DataManager::faceDataUpdated, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataManager::*)(int )>(_a, &DataManager::faceDataDeleted, 9))
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
        if (_id < 29)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 29;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 29)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 29;
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
void DataManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void DataManager::personAdded(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DataManager::personUpdated(int _t1)
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
void DataManager::faceDataAdded(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void DataManager::faceDataUpdated(int _t1, int _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void DataManager::faceDataDeleted(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}
QT_WARNING_POP
