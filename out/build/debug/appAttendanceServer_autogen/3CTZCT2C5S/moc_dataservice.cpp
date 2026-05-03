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
        // Signal 'Data_Validation'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Method 'addPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(12, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 },
        }}),
        // Method 'updatedPerson'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 },
        }}),
        // Method 'deletePerson'
        QtMocHelpers::MethodData<bool(const QString &)>(18, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'getAllPerson'
        QtMocHelpers::MethodData<QList<QObject*>()>(19, 2, QMC::AccessPublic, 0x80000000 | 20),
        // Method 'getPersonById'
        QtMocHelpers::MethodData<QObject *(int)>(21, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::Int, 22 },
        }}),
        // Method 'getPersonByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(23, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'selectPersons'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(24, 2, QMC::AccessPublic, 0x80000000 | 20, {{
            { QMetaType::QString, 13 }, { QMetaType::QString, 14 }, { QMetaType::QString, 15 }, { QMetaType::QString, 16 },
        }}),
        // Method 'addAttendanceRecord'
        QtMocHelpers::MethodData<bool(const QString &, const QDateTime &, const QString &, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QDateTime, 26 }, { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Method 'selectAttendanceRecord'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QString &, const QString &, const QString &)>(27, 2, QMC::AccessPublic, 0x80000000 | 20, {{
            { QMetaType::QString, 28 }, { QMetaType::QDateTime, 29 }, { QMetaType::QDateTime, 30 }, { QMetaType::QString, 8 },
            { QMetaType::QString, 9 }, { QMetaType::QString, 31 },
        }}),
        // Method 'addOrUpdateDevice'
        QtMocHelpers::MethodData<bool(const QString &, const QString &, const QString &, const QString &)>(32, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 33 }, { QMetaType::QString, 34 }, { QMetaType::QString, 9 },
        }}),
        // Method 'updateDeviceStatus'
        QtMocHelpers::MethodData<bool(const QString &, const QString &)>(35, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Method 'getAllDevices'
        QtMocHelpers::MethodData<QList<QObject*>()>(36, 2, QMC::AccessPublic, 0x80000000 | 20),
        // Method 'getDeviceById'
        QtMocHelpers::MethodData<QObject *(const QString &)>(37, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 8 },
        }}),
        // Method 'selectDevice'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QString &, const QString &, const QString &)>(38, 2, QMC::AccessPublic, 0x80000000 | 20, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 33 }, { QMetaType::QString, 34 }, { QMetaType::QString, 9 },
        }}),
        // Method 'addFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(39, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QByteArray, 40 },
        }}),
        // Method 'updateFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &, const QByteArray &)>(41, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 }, { QMetaType::QByteArray, 40 },
        }}),
        // Method 'deleteFaceDataByEmployeeId'
        QtMocHelpers::MethodData<bool(const QString &)>(42, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'getFaceDataByEmployeeId'
        QtMocHelpers::MethodData<QObject *(const QString &)>(43, 2, QMC::AccessPublic, QMetaType::QObjectStar, {{
            { QMetaType::QString, 14 },
        }}),
        // Method 'getAllFaceData'
        QtMocHelpers::MethodData<QList<QObject*>()>(44, 2, QMC::AccessPublic, 0x80000000 | 20),
        // Method 'selectFaceData'
        QtMocHelpers::MethodData<QList<QObject*>(const QString &, const QDateTime &, const QDateTime &, const QDateTime &, const QDateTime &)>(45, 2, QMC::AccessPublic, 0x80000000 | 20, {{
            { QMetaType::QString, 14 }, { QMetaType::QDateTime, 46 }, { QMetaType::QDateTime, 47 }, { QMetaType::QDateTime, 48 },
            { QMetaType::QDateTime, 49 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isConnected'
        QtMocHelpers::PropertyData<bool>(50, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
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
        case 4: _t->Data_Validation((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: { bool _r = _t->addPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 6: { bool _r = _t->updatedPerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 7: { bool _r = _t->deletePerson((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 8: { QList<QObject*> _r = _t->getAllPerson();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 9: { QObject* _r = _t->getPersonById((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 10: { QObject* _r = _t->getPersonByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 11: { QList<QObject*> _r = _t->selectPersons((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->addAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { QList<QObject*> _r = _t->selectAttendanceRecord((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[6])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->addOrUpdateDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 15: { bool _r = _t->updateDeviceStatus((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 16: { QList<QObject*> _r = _t->getAllDevices();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 17: { QObject* _r = _t->getDeviceById((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 18: { QList<QObject*> _r = _t->selectDevice((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 19: { bool _r = _t->addFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 20: { bool _r = _t->updateFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 21: { bool _r = _t->deleteFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 22: { QObject* _r = _t->getFaceDataByEmployeeId((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QObject**>(_a[0]) = std::move(_r); }  break;
        case 23: { QList<QObject*> _r = _t->getAllFaceData();
            if (_a[0]) *reinterpret_cast<QList<QObject*>*>(_a[0]) = std::move(_r); }  break;
        case 24: { QList<QObject*> _r = _t->selectFaceData((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[5])));
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
        if (QtMocHelpers::indexOfMethod<void (DataService::*)(const QString & )>(_a, &DataService::Data_Validation, 4))
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
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 25;
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
void DataService::Data_Validation(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
