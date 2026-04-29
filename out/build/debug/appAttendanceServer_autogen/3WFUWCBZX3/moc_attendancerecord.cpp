/****************************************************************************
** Meta object code from reading C++ file 'attendancerecord.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Models/attendancerecord.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'attendancerecord.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN16AttendanceRecordE_t {};
} // unnamed namespace

template <> constexpr inline auto AttendanceRecord::qt_create_metaobjectdata<qt_meta_tag_ZN16AttendanceRecordE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AttendanceRecord",
        "idChanged",
        "",
        "personIdChanged",
        "checkTimeChanged",
        "deviceIdChanged",
        "statusChanged",
        "receivedTimeChanged",
        "id",
        "personId",
        "checkTime",
        "deviceId",
        "status",
        "receivedTime"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'idChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'personIdChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'checkTimeChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'deviceIdChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'receivedTimeChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'id'
        QtMocHelpers::PropertyData<int>(8, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'personId'
        QtMocHelpers::PropertyData<int>(9, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'checkTime'
        QtMocHelpers::PropertyData<QDateTime>(10, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'deviceId'
        QtMocHelpers::PropertyData<QString>(11, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'status'
        QtMocHelpers::PropertyData<QString>(12, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'receivedTime'
        QtMocHelpers::PropertyData<QDateTime>(13, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AttendanceRecord, qt_meta_tag_ZN16AttendanceRecordE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AttendanceRecord::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AttendanceRecordE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AttendanceRecordE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN16AttendanceRecordE_t>.metaTypes,
    nullptr
} };

void AttendanceRecord::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AttendanceRecord *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->idChanged(); break;
        case 1: _t->personIdChanged(); break;
        case 2: _t->checkTimeChanged(); break;
        case 3: _t->deviceIdChanged(); break;
        case 4: _t->statusChanged(); break;
        case 5: _t->receivedTimeChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::idChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::personIdChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::checkTimeChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::deviceIdChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::statusChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (AttendanceRecord::*)()>(_a, &AttendanceRecord::receivedTimeChanged, 5))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->id(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->personId(); break;
        case 2: *reinterpret_cast<QDateTime*>(_v) = _t->checkTime(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->deviceId(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->status(); break;
        case 5: *reinterpret_cast<QDateTime*>(_v) = _t->receivedTime(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setId(*reinterpret_cast<int*>(_v)); break;
        case 1: _t->setPersonId(*reinterpret_cast<int*>(_v)); break;
        case 2: _t->setCheckTime(*reinterpret_cast<QDateTime*>(_v)); break;
        case 3: _t->setDeviceId(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setStatus(*reinterpret_cast<QString*>(_v)); break;
        case 5: _t->setReceivedTime(*reinterpret_cast<QDateTime*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *AttendanceRecord::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AttendanceRecord::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN16AttendanceRecordE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AttendanceRecord::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void AttendanceRecord::idChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void AttendanceRecord::personIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void AttendanceRecord::checkTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void AttendanceRecord::deviceIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void AttendanceRecord::statusChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void AttendanceRecord::receivedTimeChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}
QT_WARNING_POP
