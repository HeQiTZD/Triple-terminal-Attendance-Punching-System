/****************************************************************************
** Meta object code from reading C++ file 'dataservice.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Services/dataservice.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dataservice.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11DataServiceE_t {};
} // unnamed namespace

template <> constexpr inline auto DataService::qt_create_metaobjectdata<qt_meta_tag_ZN11DataServiceE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DataService",
        "connectionStateChanged",
        "",
        "operationResult",
        "ok",
        "message",
        "operationTip",
        "deviceStatusChanged",
        "deviceId",
        "status",
        "deviceRecordChanged",
        "Data_Validation",
        "data_Validation",
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
        "id",
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
        "updateDevice",
        "QVariantMap",
        "updates",
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
        // Signal 'deviceStatusChanged'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'deviceRecordChanged'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 },
        }}),
        // Signal 'Data_Validation'
        QtMocHelpers::SignalData<void(const QString &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 12 },
        }}),
        // Method 'addPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Method 'updatedPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Method 'deletePerson'
        QtMocHelpers::MethodData<bool(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'getAllPerson'
        QtMocHelpers::MethodData<QList<QObject*>()>(20, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'getPersonById'
        QtMocHelpers::MethodData<QObject *(int)>(22, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::Int, 23 },
        }}),
        // Method 'getPersonByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'selectPersons'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(25, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 },
        }}),
        // Method 'addAttendanceRecord'
        QtMocHelpers::MethodData<bool(const QString &, const QDateTime &, const QString &, const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QDateTime, 27 }, { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Method 'selectAttendanceRecord'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QString &, const QString &, const QString &)>(28, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::QString, 29 }, { QMetaType::QDateTime, 30 }, { QMetaType::QDateTime, 31 }, { QMetaType::QString, 8 },
            { QMetaType::QString, 9 }, { QMetaType::QString, 32 },
        }}),
        // Method 'addOrUpdateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(33, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 34 }, { QMetaType::QString, 35 }, { QMetaType::QString, 9 },
        }}),
        // Method 'updateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QVariantMap &)>(36, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { 0x80000000 | 37, 38 },
        }}),
        // Method 'updateDeviceStatus'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(39, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Method 'getAllDevices'
        QtMocHelpers::MethodData<QList<QObject*>()>(40, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'getDeviceById'
        QtMocHelpers::MethodData<QObject *(const QString &)>(41, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 8 },
        }}),
        // Method 'selectDevice'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(42, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 34 }, { QMetaType::QString, 35 }, { QMetaType::QString, 9 },
        }}),
        // Method 'addFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(43, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QByteArray, 44 },
        }}),
        // Method 'updateFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(45, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 }, { QMetaType::QByteArray, 44 },
        }}),
        // Method 'deleteFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &)>(46, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'getFaceDataByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(47, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 15 },
        }}),
        // Method 'getAllFaceData'
        QtMocHelpers::MethodData<QList<QObject*>()>(48, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'getAllFaceDataWithFeature'
        QtMocHelpers::MethodData<QList<QObject*>()>(49, 2, QMC::AccessPublic, 0x80000000 | 21),
        // Method 'selectFaceData'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QDateTime &, const QDateTime &)>(50, 2, QMC::AccessPublic, 0x80000000 | 21, {{
            { QMetaType::QString, 15 }, { QMetaType::QDateTime, 51 }, { QMetaType::QDateTime, 52 }, { QMetaType::QDateTime, 53 },
            { QMetaType::QDateTime, 54 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(55, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DataService, qt_meta_tag_ZN11DataServiceE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DataService::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataServiceE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataServiceE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11DataServiceE_t>.metaTypes,
    nullptr
} };

void DataService::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DataService *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connectionStateChanged(); break;
        case 1: _t->operationResult((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 2: _t->operationTip((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->deviceStatusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->deviceRecordChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->Data_Validation((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 6: { bool _r = _t->addPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { bool _r = _t->updatedPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { bool _r = _t->deletePerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 9: { QList<QObject*> _r = _t->getAllPerson();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 10: { QObject* _r = _t->getPersonById((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 11: { QObject* _r = _t->getPersonByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 12: { QList<QObject*> _r = _t->selectPersons((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 13: { bool _r = _t->addAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 14: { QList<QObject*> _r = _t->selectAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[6])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->addOrUpdateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { bool _r = _t->updateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 17: { bool _r = _t->updateDeviceStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 18: { QList<QObject*> _r = _t->getAllDevices();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 19: { QObject* _r = _t->getDeviceById((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 20: { QList<QObject*> _r = _t->selectDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 21: { bool _r = _t->addFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { bool _r = _t->updateFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 23: { bool _r = _t->deleteFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 24: { QObject* _r = _t->getFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 25: { QList<QObject*> _r = _t->getAllFaceData();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 26: { QList<QObject*> _r = _t->getAllFaceDataWithFeature();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 27: { QList<QObject*> _r = _t->selectFaceData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[5])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DataService::*)()>(_a, &DataService::connectionStateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(bool , const QString & )>(_a, &DataService::operationResult, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(const QString & )>(_a, &DataService::operationTip, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(const QString & , const QString & )>(_a, &DataService::deviceStatusChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(const QString & )>(_a, &DataService::deviceRecordChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(const QString & )>(_a, &DataService::Data_Validation, 5))
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

const QMetaObject *DataService::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DataService::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11DataServiceE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DataService::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 28;
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
void DataService::connectionStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DataService::operationResult(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1, _t2);
}

// SIGNAL 2
void DataService::operationTip(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void DataService::deviceStatusChanged(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void DataService::deviceRecordChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void DataService::Data_Validation(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}
QT_WARNING_POP
