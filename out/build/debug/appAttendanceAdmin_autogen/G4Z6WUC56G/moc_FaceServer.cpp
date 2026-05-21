/****************************************************************************
** Meta object code from reading C++ file 'FaceServer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Face/FaceServer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'FaceServer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10FaceServerE_t {};
} // unnamed namespace

template <> constexpr inline auto FaceServer::qt_create_metaobjectdata<qt_meta_tag_ZN10FaceServerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FaceServer",
        "busyChanged",
        "",
        "recordsChanged",
        "lastFoundChanged",
        "lastRecordChanged",
        "operationSucceeded",
        "apiType",
        "message",
        "operationFailed",
        "code",
        "queryCompleted",
        "found",
        "QVariantMap",
        "record",
        "registerCompleted",
        "result",
        "queryFace",
        "employeeId",
        "deleteFace",
        "registerFace",
        "photoBase64",
        "overwrite",
        "registerFaceFromFile",
        "filePathOrUrl",
        "clearRecords",
        "busy",
        "records",
        "QVariantList",
        "lastFound",
        "lastRecord"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'recordsChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastFoundChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastRecordChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'operationSucceeded'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'operationFailed'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 7 }, { QMetaType::Int, 10 }, { QMetaType::QString, 8 },
        }}),
        // Signal 'queryCompleted'
        QtMocHelpers::SignalData<void(bool, const QVariantMap &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 12 }, { 0x80000000 | 13, 14 },
        }}),
        // Signal 'registerCompleted'
        QtMocHelpers::SignalData<void(const QVariantMap &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 13, 16 },
        }}),
        // Method 'queryFace'
        QtMocHelpers::MethodData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'deleteFace'
        QtMocHelpers::MethodData<void(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'registerFace'
        QtMocHelpers::MethodData<void(const QString &, const QString &, bool)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 21 }, { QMetaType::Bool, 22 },
        }}),
        // Method 'registerFace'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(20, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 21 },
        }}),
        // Method 'registerFaceFromFile'
        QtMocHelpers::MethodData<void(const QString &, const QString &, bool)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 24 }, { QMetaType::Bool, 22 },
        }}),
        // Method 'registerFaceFromFile'
        QtMocHelpers::MethodData<void(const QString &, const QString &)>(23, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 24 },
        }}),
        // Method 'clearRecords'
        QtMocHelpers::MethodData<void()>(25, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(26, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'records'
        QtMocHelpers::PropertyData<QVariantList>(27, 0x80000000 | 28, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 1),
        // property 'lastFound'
        QtMocHelpers::PropertyData<bool>(29, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
        // property 'lastRecord'
        QtMocHelpers::PropertyData<QVariantMap>(30, 0x80000000 | 13, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 3),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FaceServer, qt_meta_tag_ZN10FaceServerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FaceServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10FaceServerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10FaceServerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10FaceServerE_t>.metaTypes,
    nullptr
} };

void FaceServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FaceServer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->busyChanged(); break;
        case 1: _t->recordsChanged(); break;
        case 2: _t->lastFoundChanged(); break;
        case 3: _t->lastRecordChanged(); break;
        case 4: _t->operationSucceeded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->operationFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 6: _t->queryCompleted((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 7: _t->registerCompleted((*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[1]))); break;
        case 8: _t->queryFace((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->deleteFace((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 10: _t->registerFace((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 11: _t->registerFace((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 12: _t->registerFaceFromFile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3]))); break;
        case 13: _t->registerFaceFromFile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->clearRecords(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)()>(_a, &FaceServer::busyChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)()>(_a, &FaceServer::recordsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)()>(_a, &FaceServer::lastFoundChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)()>(_a, &FaceServer::lastRecordChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)(const QString & , const QString & )>(_a, &FaceServer::operationSucceeded, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)(const QString & , int , const QString & )>(_a, &FaceServer::operationFailed, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)(bool , const QVariantMap & )>(_a, &FaceServer::queryCompleted, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceServer::*)(const QVariantMap & )>(_a, &FaceServer::registerCompleted, 7))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        case 1: *reinterpret_cast<QVariantList*>(_v) = _t->records(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->lastFound(); break;
        case 3: *reinterpret_cast<QVariantMap*>(_v) = _t->lastRecord(); break;
        default: break;
        }
    }
}

const QMetaObject *FaceServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FaceServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10FaceServerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FaceServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 15)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 15;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 15)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 15;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void FaceServer::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void FaceServer::recordsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void FaceServer::lastFoundChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void FaceServer::lastRecordChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void FaceServer::operationSucceeded(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void FaceServer::operationFailed(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void FaceServer::queryCompleted(bool _t1, const QVariantMap & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void FaceServer::registerCompleted(const QVariantMap & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}
QT_WARNING_POP
