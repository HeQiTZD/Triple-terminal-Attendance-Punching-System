/****************************************************************************
** Meta object code from reading C++ file 'RbacServer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Rbac/RbacServer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RbacServer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN10RbacServerE_t {};
} // unnamed namespace

template <> constexpr inline auto RbacServer::qt_create_metaobjectdata<qt_meta_tag_ZN10RbacServerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "RbacServer",
        "busyChanged",
        "",
        "roleRecordsChanged",
        "permissionRecordsChanged",
        "userRoleKeysChanged",
        "selfPermissionsChanged",
        "operationSucceeded",
        "apiType",
        "message",
        "operationFailed",
        "code",
        "queryRoles",
        "queryPermissions",
        "querySelfPermissions",
        "queryUserRoles",
        "userId",
        "createRole",
        "roleKey",
        "roleName",
        "description",
        "updateRole",
        "QVariantMap",
        "fields",
        "deleteRole",
        "assignUserRole",
        "revokeUserRole",
        "busy",
        "roleRecords",
        "QVariantList",
        "permissionRecords",
        "userRoleKeys"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'roleRecordsChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'permissionRecordsChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'userRoleKeysChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'selfPermissionsChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'operationSucceeded'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'operationFailed'
        QtMocHelpers::SignalData<void(const QString &, int, const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 8 }, { QMetaType::Int, 11 }, { QMetaType::QString, 9 },
        }}),
        // Method 'queryRoles'
        QtMocHelpers::MethodData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'queryPermissions'
        QtMocHelpers::MethodData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'querySelfPermissions'
        QtMocHelpers::MethodData<void()>(14, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'queryUserRoles'
        QtMocHelpers::MethodData<void(int)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 },
        }}),
        // Method 'createRole'
        QtMocHelpers::MethodData<void(const QString &, const QString &, const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { QMetaType::QString, 19 }, { QMetaType::QString, 20 },
        }}),
        // Method 'updateRole'
        QtMocHelpers::MethodData<void(const QString &, const QVariantMap &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 }, { 0x80000000 | 22, 23 },
        }}),
        // Method 'deleteRole'
        QtMocHelpers::MethodData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'assignUserRole'
        QtMocHelpers::MethodData<void(int, const QString &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 }, { QMetaType::QString, 18 },
        }}),
        // Method 'revokeUserRole'
        QtMocHelpers::MethodData<void(int, const QString &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 16 }, { QMetaType::QString, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(27, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'roleRecords'
        QtMocHelpers::PropertyData<QVariantList>(28, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 1),
        // property 'permissionRecords'
        QtMocHelpers::PropertyData<QVariantList>(30, 0x80000000 | 29, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 2),
        // property 'userRoleKeys'
        QtMocHelpers::PropertyData<QStringList>(31, QMetaType::QStringList, QMC::DefaultPropertyFlags, 3),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<RbacServer, qt_meta_tag_ZN10RbacServerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject RbacServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RbacServerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RbacServerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN10RbacServerE_t>.metaTypes,
    nullptr
} };

void RbacServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<RbacServer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->busyChanged(); break;
        case 1: _t->roleRecordsChanged(); break;
        case 2: _t->permissionRecordsChanged(); break;
        case 3: _t->userRoleKeysChanged(); break;
        case 4: _t->selfPermissionsChanged(); break;
        case 5: _t->operationSucceeded((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 6: _t->operationFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 7: _t->queryRoles(); break;
        case 8: _t->queryPermissions(); break;
        case 9: _t->querySelfPermissions(); break;
        case 10: _t->queryUserRoles((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 11: _t->createRole((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 12: _t->updateRole((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QVariantMap>>(_a[2]))); break;
        case 13: _t->deleteRole((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 14: _t->assignUserRole((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 15: _t->revokeUserRole((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)()>(_a, &RbacServer::busyChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)()>(_a, &RbacServer::roleRecordsChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)()>(_a, &RbacServer::permissionRecordsChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)()>(_a, &RbacServer::userRoleKeysChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)()>(_a, &RbacServer::selfPermissionsChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)(const QString & , const QString & )>(_a, &RbacServer::operationSucceeded, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (RbacServer::*)(const QString & , int , const QString & )>(_a, &RbacServer::operationFailed, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        case 1: *reinterpret_cast<QVariantList*>(_v) = _t->roleRecords(); break;
        case 2: *reinterpret_cast<QVariantList*>(_v) = _t->permissionRecords(); break;
        case 3: *reinterpret_cast<QStringList*>(_v) = _t->userRoleKeys(); break;
        default: break;
        }
    }
}

const QMetaObject *RbacServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *RbacServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN10RbacServerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int RbacServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
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
void RbacServer::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void RbacServer::roleRecordsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void RbacServer::permissionRecordsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void RbacServer::userRoleKeysChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void RbacServer::selfPermissionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void RbacServer::operationSucceeded(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2);
}

// SIGNAL 6
void RbacServer::operationFailed(const QString & _t1, int _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
