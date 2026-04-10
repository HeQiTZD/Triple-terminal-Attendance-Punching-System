/****************************************************************************
** Meta object code from reading C++ file 'datamanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/DataManager/datamanager.h"
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
        "personId",
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
        // Method 'addPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString, const QString &, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Method 'updatedPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Method 'deletePerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 },
        }}),
        // Method 'getAllPerson'
        QtMocHelpers::MethodData<QList<QObject*>()>(20, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'getPersonById'
        QtMocHelpers::MethodData<QObject *(int)>(22, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::Int, 6 },
        }}),
        // Method 'getPersonByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'updatePersonFaceFeature'
        QtMocHelpers::MethodData<bool(int, const QByteArray &)>(24, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 6 }, { QMetaType::QByteArray, 25 },
        }}),
        // Method 'addAttendanceRecore'
        QtMocHelpers::MethodData<bool(int, const QDateTime &, const QString &, const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::Int, 27 }, { QMetaType::QDateTime, 28 }, { QMetaType::QString, 29 }, { QMetaType::QString, 12 },
        }}),
        // Method 'getAttendanceRecords'
        QtMocHelpers::MethodData<QList<QObject*>(const QDateTime &, const QDateTime &)>(30, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::QDateTime, 31 }, { QMetaType::QDateTime, 32 },
        }}),
        // Method 'getAttendanceRecordsByPerson'
        QtMocHelpers::MethodData<QList<QObject*>(int, const QDateTime &, const QDateTime &)>(33, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::Int, 27 }, { QMetaType::QDateTime, 31 }, { QMetaType::QDateTime, 32 },
        }}),
        // Method 'addOrUpdateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(34, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 29 }, { QMetaType::QString, 35 }, { QMetaType::QString, 36 }, { QMetaType::QString, 12 },
        }}),
        // Method 'updateDeviceStatus'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(37, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 29 }, { QMetaType::QString, 12 },
        }}),
        // Method 'getAllDevices'
        QtMocHelpers::MethodData<QList<QObject*>()>(38, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'getDeviceById'
        QtMocHelpers::MethodData<QObject *(const QString &)>(39, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 29 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(40, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
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
        case 7: { bool _r = _t->addPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->updatedPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->deletePerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { QList<QObject*> _r = _t->getAllPerson();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 11: { QObject* _r = _t->getPersonById((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 12: { QObject* _r = _t->getPersonByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 13: { bool _r = _t->updatePersonFaceFeature((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->addAttendanceRecore((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 15: { QList<QObject*> _r = _t->getAttendanceRecords((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 16: { QList<QObject*> _r = _t->getAttendanceRecordsByPerson((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->addOrUpdateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { bool _r = _t->updateDeviceStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 19: { QList<QObject*> _r = _t->getAllDevices();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 20: { QObject* _r = _t->getDeviceById((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
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
        if (_id < 21)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 21;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 21)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 21;
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
QT_WARNING_POP
