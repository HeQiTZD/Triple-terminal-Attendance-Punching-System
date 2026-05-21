/****************************************************************************
** Meta object code from reading C++ file 'SessionManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Auth/SessionManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SessionManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN14SessionManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SessionManager::qt_create_metaobjectdata<qt_meta_tag_ZN14SessionManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SessionManager",
        "loggedInChanged",
        "",
        "sessionTokenChanged",
        "rolesChanged",
        "permissionsChanged",
        "currentUsernameChanged",
        "connectionStateChanged",
        "loggedIn",
        "sessionToken",
        "roles",
        "permissions",
        "loggedOut",
        "loginFailed",
        "code",
        "message",
        "permissionsRefreshed",
        "errorOccurred",
        "error",
        "onTcpStateChanged",
        "oldState",
        "newState",
        "onTcpAuthenticated",
        "token",
        "roleList",
        "permList",
        "onTcpAuthFailed",
        "msg",
        "onTcpMessageReceived",
        "QJsonObject",
        "onTcpError",
        "login",
        "host",
        "port",
        "username",
        "password",
        "clientId",
        "logout",
        "hasPermission",
        "permKey",
        "hasRole",
        "roleKey",
        "refreshPermissions",
        "isLoggedIn",
        "currentUsername",
        "connectionState"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'loggedInChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sessionTokenChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'rolesChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'permissionsChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'currentUsernameChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loggedIn'
        QtMocHelpers::SignalData<void(const QString &, const QStringList &, const QStringList &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 }, { QMetaType::QStringList, 10 }, { QMetaType::QStringList, 11 },
        }}),
        // Signal 'loggedOut'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'loginFailed'
        QtMocHelpers::SignalData<void(int, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::QString, 15 },
        }}),
        // Signal 'permissionsRefreshed'
        QtMocHelpers::SignalData<void(const QStringList &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 11 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Slot 'onTcpStateChanged'
        QtMocHelpers::SlotData<void(int, int)>(19, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 20 }, { QMetaType::Int, 21 },
        }}),
        // Slot 'onTcpAuthenticated'
        QtMocHelpers::SlotData<void(const QString &, const QStringList &, const QStringList &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 23 }, { QMetaType::QStringList, 24 }, { QMetaType::QStringList, 25 },
        }}),
        // Slot 'onTcpAuthFailed'
        QtMocHelpers::SlotData<void(int, const QString &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Int, 14 }, { QMetaType::QString, 27 },
        }}),
        // Slot 'onTcpMessageReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(28, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 29, 15 },
        }}),
        // Slot 'onTcpError'
        QtMocHelpers::SlotData<void(const QString &)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'login'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &, const QString &, const QString &)>(31, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 32 }, { QMetaType::Int, 33 }, { QMetaType::QString, 34 }, { QMetaType::QString, 35 },
            { QMetaType::QString, 36 },
        }}),
        // Method 'login'
        QtMocHelpers::MethodData<void(const QString &, int, const QString &, const QString &)>(31, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Void, {{
            { QMetaType::QString, 32 }, { QMetaType::Int, 33 }, { QMetaType::QString, 34 }, { QMetaType::QString, 35 },
        }}),
        // Method 'logout'
        QtMocHelpers::MethodData<void()>(37, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'hasPermission'
        QtMocHelpers::MethodData<bool(const QString &) const>(38, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 39 },
        }}),
        // Method 'hasRole'
        QtMocHelpers::MethodData<bool(const QString &) const>(40, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 41 },
        }}),
        // Method 'refreshPermissions'
        QtMocHelpers::MethodData<void()>(42, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isLoggedIn'
        QtMocHelpers::PropertyData<bool>(43, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'sessionToken'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'roles'
        QtMocHelpers::PropertyData<QStringList>(10, QMetaType::QStringList, QMC::DefaultPropertyFlags, 2),
        // property 'permissions'
        QtMocHelpers::PropertyData<QStringList>(11, QMetaType::QStringList, QMC::DefaultPropertyFlags, 3),
        // property 'currentUsername'
        QtMocHelpers::PropertyData<QString>(44, QMetaType::QString, QMC::DefaultPropertyFlags, 4),
        // property 'connectionState'
        QtMocHelpers::PropertyData<int>(45, QMetaType::Int, QMC::DefaultPropertyFlags, 5),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SessionManager, qt_meta_tag_ZN14SessionManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SessionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SessionManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SessionManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN14SessionManagerE_t>.metaTypes,
    nullptr
} };

void SessionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SessionManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->loggedInChanged(); break;
        case 1: _t->sessionTokenChanged(); break;
        case 2: _t->rolesChanged(); break;
        case 3: _t->permissionsChanged(); break;
        case 4: _t->currentUsernameChanged(); break;
        case 5: _t->connectionStateChanged(); break;
        case 6: _t->loggedIn((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3]))); break;
        case 7: _t->loggedOut(); break;
        case 8: _t->loginFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->permissionsRefreshed((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1]))); break;
        case 10: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 11: _t->onTcpStateChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2]))); break;
        case 12: _t->onTcpAuthenticated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3]))); break;
        case 13: _t->onTcpAuthFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 14: _t->onTcpMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 15: _t->onTcpError((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->login((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[5]))); break;
        case 17: _t->login((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        case 18: _t->logout(); break;
        case 19: { bool _r = _t->hasPermission((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 20: { bool _r = _t->hasRole((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->refreshPermissions(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::loggedInChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::sessionTokenChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::rolesChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::permissionsChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::currentUsernameChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::connectionStateChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)(const QString & , const QStringList & , const QStringList & )>(_a, &SessionManager::loggedIn, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)()>(_a, &SessionManager::loggedOut, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)(int , const QString & )>(_a, &SessionManager::loginFailed, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)(const QStringList & )>(_a, &SessionManager::permissionsRefreshed, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (SessionManager::*)(const QString & )>(_a, &SessionManager::errorOccurred, 10))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isLoggedIn(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->sessionToken(); break;
        case 2: *reinterpret_cast<QStringList*>(_v) = _t->roles(); break;
        case 3: *reinterpret_cast<QStringList*>(_v) = _t->permissions(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->currentUsername(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->connectionState(); break;
        default: break;
        }
    }
}

const QMetaObject *SessionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SessionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN14SessionManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SessionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 22)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 22;
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
void SessionManager::loggedInChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SessionManager::sessionTokenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SessionManager::rolesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void SessionManager::permissionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void SessionManager::currentUsernameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void SessionManager::connectionStateChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void SessionManager::loggedIn(const QString & _t1, const QStringList & _t2, const QStringList & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2, _t3);
}

// SIGNAL 7
void SessionManager::loggedOut()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void SessionManager::loginFailed(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void SessionManager::permissionsRefreshed(const QStringList & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void SessionManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}
QT_WARNING_POP
