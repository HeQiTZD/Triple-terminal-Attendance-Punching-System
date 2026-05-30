/****************************************************************************
** Meta object code from reading C++ file 'TcpConnectionManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Network/TcpConnectionManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'TcpConnectionManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN20TcpConnectionManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto TcpConnectionManager::qt_create_metaobjectdata<qt_meta_tag_ZN20TcpConnectionManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TcpConnectionManager",
        "stateChanged",
        "",
        "TcpConnectionManager::ConnectionState",
        "oldState",
        "newState",
        "authenticatedChanged",
        "sessionTokenChanged",
        "accessTokenChanged",
        "refreshTokenChanged",
        "rolesChanged",
        "permissionsChanged",
        "authenticated",
        "sessionToken",
        "roles",
        "permissions",
        "authFailed",
        "code",
        "msg",
        "tokenRefreshed",
        "accessToken",
        "refreshToken",
        "tokenRefreshFailed",
        "messageReceived",
        "QJsonObject",
        "message",
        "jsonMessageSent",
        "jsonMessageReceived",
        "binaryFrameReceived",
        "payload",
        "errorOccurred",
        "error",
        "heartbeatAcknowledged",
        "connectToServer",
        "TcpConnectionManager::ConnectionConfig",
        "config",
        "disconnectFromServer",
        "sendMessage",
        "ResponseCallback",
        "callback",
        "sendBinaryFrame",
        "refreshTokens",
        "onSocketConnected",
        "onSocketDisconnected",
        "onSocketReadyRead",
        "onSocketErrorOccurred",
        "QAbstractSocket::SocketError",
        "onHeartbeatTimeout",
        "onReconnectTimeout",
        "onRequestTimeout",
        "msgId",
        "connectionState",
        "ConnectionState",
        "isAuthenticated",
        "Disconnected",
        "Connecting",
        "Connected",
        "Authenticated"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'stateChanged'
        QtMocHelpers::SignalData<void(TcpConnectionManager::ConnectionState, TcpConnectionManager::ConnectionState)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 3, 4 }, { 0x80000000 | 3, 5 },
        }}),
        // Signal 'authenticatedChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'sessionTokenChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'accessTokenChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'refreshTokenChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'rolesChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'permissionsChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'authenticated'
        QtMocHelpers::SignalData<void(const QString &, const QStringList &, const QStringList &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 13 }, { QMetaType::QStringList, 14 }, { QMetaType::QStringList, 15 },
        }}),
        // Signal 'authFailed'
        QtMocHelpers::SignalData<void(int, const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 17 }, { QMetaType::QString, 18 },
        }}),
        // Signal 'tokenRefreshed'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 20 }, { QMetaType::QString, 21 },
        }}),
        // Signal 'tokenRefreshFailed'
        QtMocHelpers::SignalData<void(int, const QString &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 17 }, { QMetaType::QString, 18 },
        }}),
        // Signal 'messageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Signal 'jsonMessageSent'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Signal 'jsonMessageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Signal 'binaryFrameReceived'
        QtMocHelpers::SignalData<void(const QByteArray &)>(28, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 29 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(30, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 31 },
        }}),
        // Signal 'heartbeatAcknowledged'
        QtMocHelpers::SignalData<void()>(32, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'connectToServer'
        QtMocHelpers::SlotData<void(const TcpConnectionManager::ConnectionConfig &)>(33, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 34, 35 },
        }}),
        // Slot 'disconnectFromServer'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendMessage'
        QtMocHelpers::SlotData<QString(const QJsonObject &, ResponseCallback)>(37, 2, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 24, 25 }, { 0x80000000 | 38, 39 },
        }}),
        // Slot 'sendMessage'
        QtMocHelpers::SlotData<QString(const QJsonObject &)>(37, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QString, {{
            { 0x80000000 | 24, 25 },
        }}),
        // Slot 'sendBinaryFrame'
        QtMocHelpers::SlotData<void(const QByteArray &)>(40, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 29 },
        }}),
        // Slot 'refreshTokens'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onSocketConnected'
        QtMocHelpers::SlotData<void()>(42, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(43, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketReadyRead'
        QtMocHelpers::SlotData<void()>(44, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketErrorOccurred'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(45, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 46, 31 },
        }}),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(47, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReconnectTimeout'
        QtMocHelpers::SlotData<void()>(48, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRequestTimeout'
        QtMocHelpers::SlotData<void(const QString &)>(49, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 50 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'connectionState'
        QtMocHelpers::PropertyData<enum ConnectionState>(51, 0x80000000 | 52, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'isAuthenticated'
        QtMocHelpers::PropertyData<bool>(53, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'sessionToken'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'accessToken'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
        // property 'refreshToken'
        QtMocHelpers::PropertyData<QString>(21, QMetaType::QString, QMC::DefaultPropertyFlags, 4),
        // property 'roles'
        QtMocHelpers::PropertyData<QStringList>(14, QMetaType::QStringList, QMC::DefaultPropertyFlags, 5),
        // property 'permissions'
        QtMocHelpers::PropertyData<QStringList>(15, QMetaType::QStringList, QMC::DefaultPropertyFlags, 6),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ConnectionState'
        QtMocHelpers::EnumData<enum ConnectionState>(52, 52, QMC::EnumIsScoped).add({
            {   54, ConnectionState::Disconnected },
            {   55, ConnectionState::Connecting },
            {   56, ConnectionState::Connected },
            {   57, ConnectionState::Authenticated },
        }),
    };
    return QtMocHelpers::metaObjectData<TcpConnectionManager, qt_meta_tag_ZN20TcpConnectionManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TcpConnectionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20TcpConnectionManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20TcpConnectionManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN20TcpConnectionManagerE_t>.metaTypes,
    nullptr
} };

void TcpConnectionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TcpConnectionManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->stateChanged((*reinterpret_cast<std::add_pointer_t<TcpConnectionManager::ConnectionState>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<TcpConnectionManager::ConnectionState>>(_a[2]))); break;
        case 1: _t->authenticatedChanged(); break;
        case 2: _t->sessionTokenChanged(); break;
        case 3: _t->accessTokenChanged(); break;
        case 4: _t->refreshTokenChanged(); break;
        case 5: _t->rolesChanged(); break;
        case 6: _t->permissionsChanged(); break;
        case 7: _t->authenticated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3]))); break;
        case 8: _t->authFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 9: _t->tokenRefreshed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 10: _t->tokenRefreshFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 11: _t->messageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 12: _t->jsonMessageSent((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 13: _t->jsonMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 14: _t->binaryFrameReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 15: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 16: _t->heartbeatAcknowledged(); break;
        case 17: _t->connectToServer((*reinterpret_cast<std::add_pointer_t<TcpConnectionManager::ConnectionConfig>>(_a[1]))); break;
        case 18: _t->disconnectFromServer(); break;
        case 19: { QString _r = _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<ResponseCallback>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 20: { QString _r = _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 21: _t->sendBinaryFrame((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 22: _t->refreshTokens(); break;
        case 23: _t->onSocketConnected(); break;
        case 24: _t->onSocketDisconnected(); break;
        case 25: _t->onSocketReadyRead(); break;
        case 26: _t->onSocketErrorOccurred((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 27: _t->onHeartbeatTimeout(); break;
        case 28: _t->onReconnectTimeout(); break;
        case 29: _t->onRequestTimeout((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 26:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(TcpConnectionManager::ConnectionState , TcpConnectionManager::ConnectionState )>(_a, &TcpConnectionManager::stateChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::authenticatedChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::sessionTokenChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::accessTokenChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::refreshTokenChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::rolesChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::permissionsChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QString & , const QStringList & , const QStringList & )>(_a, &TcpConnectionManager::authenticated, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(int , const QString & )>(_a, &TcpConnectionManager::authFailed, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QString & , const QString & )>(_a, &TcpConnectionManager::tokenRefreshed, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(int , const QString & )>(_a, &TcpConnectionManager::tokenRefreshFailed, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::messageReceived, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::jsonMessageSent, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::jsonMessageReceived, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QByteArray & )>(_a, &TcpConnectionManager::binaryFrameReceived, 14))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QString & )>(_a, &TcpConnectionManager::errorOccurred, 15))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::heartbeatAcknowledged, 16))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<enum ConnectionState*>(_v) = _t->connectionState(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isAuthenticated(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->sessionToken(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->accessToken(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->refreshToken(); break;
        case 5: *reinterpret_cast<QStringList*>(_v) = _t->roles(); break;
        case 6: *reinterpret_cast<QStringList*>(_v) = _t->permissions(); break;
        default: break;
        }
    }
}

const QMetaObject *TcpConnectionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN20TcpConnectionManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TcpConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 30)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 30;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void TcpConnectionManager::stateChanged(TcpConnectionManager::ConnectionState _t1, TcpConnectionManager::ConnectionState _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2);
}

// SIGNAL 1
void TcpConnectionManager::authenticatedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TcpConnectionManager::sessionTokenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TcpConnectionManager::accessTokenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TcpConnectionManager::refreshTokenChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TcpConnectionManager::rolesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void TcpConnectionManager::permissionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void TcpConnectionManager::authenticated(const QString & _t1, const QStringList & _t2, const QStringList & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3);
}

// SIGNAL 8
void TcpConnectionManager::authFailed(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void TcpConnectionManager::tokenRefreshed(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2);
}

// SIGNAL 10
void TcpConnectionManager::tokenRefreshFailed(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1, _t2);
}

// SIGNAL 11
void TcpConnectionManager::messageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void TcpConnectionManager::jsonMessageSent(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void TcpConnectionManager::jsonMessageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void TcpConnectionManager::binaryFrameReceived(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}

// SIGNAL 15
void TcpConnectionManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 15, nullptr, _t1);
}

// SIGNAL 16
void TcpConnectionManager::heartbeatAcknowledged()
{
    QMetaObject::activate(this, &staticMetaObject, 16, nullptr);
}
QT_WARNING_POP
