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
        "rolesChanged",
        "permissionsChanged",
        "authenticated",
        "sessionToken",
        "roles",
        "permissions",
        "authFailed",
        "code",
        "msg",
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
        // Signal 'rolesChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'permissionsChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'authenticated'
        QtMocHelpers::SignalData<void(const QString &, const QStringList &, const QStringList &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 }, { QMetaType::QStringList, 12 }, { QMetaType::QStringList, 13 },
        }}),
        // Signal 'authFailed'
        QtMocHelpers::SignalData<void(int, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 }, { QMetaType::QString, 16 },
        }}),
        // Signal 'messageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(17, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Signal 'jsonMessageSent'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Signal 'jsonMessageReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(21, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Signal 'binaryFrameReceived'
        QtMocHelpers::SignalData<void(const QByteArray &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 23 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 25 },
        }}),
        // Signal 'heartbeatAcknowledged'
        QtMocHelpers::SignalData<void()>(26, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'connectToServer'
        QtMocHelpers::SlotData<void(const TcpConnectionManager::ConnectionConfig &)>(27, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 28, 29 },
        }}),
        // Slot 'disconnectFromServer'
        QtMocHelpers::SlotData<void()>(30, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'sendMessage'
        QtMocHelpers::SlotData<QString(const QJsonObject &, ResponseCallback)>(31, 2, QMC::AccessPublic, QMetaType::QString, {{
            { 0x80000000 | 18, 19 }, { 0x80000000 | 32, 33 },
        }}),
        // Slot 'sendMessage'
        QtMocHelpers::SlotData<QString(const QJsonObject &)>(31, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::QString, {{
            { 0x80000000 | 18, 19 },
        }}),
        // Slot 'sendBinaryFrame'
        QtMocHelpers::SlotData<void(const QByteArray &)>(34, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 23 },
        }}),
        // Slot 'onSocketConnected'
        QtMocHelpers::SlotData<void()>(35, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(36, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketReadyRead'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketErrorOccurred'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(38, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 39, 25 },
        }}),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(40, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onReconnectTimeout'
        QtMocHelpers::SlotData<void()>(41, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onRequestTimeout'
        QtMocHelpers::SlotData<void(const QString &)>(42, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QString, 43 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'connectionState'
        QtMocHelpers::PropertyData<enum ConnectionState>(44, 0x80000000 | 45, QMC::DefaultPropertyFlags | QMC::EnumOrFlag, 0),
        // property 'isAuthenticated'
        QtMocHelpers::PropertyData<bool>(46, QMetaType::Bool, QMC::DefaultPropertyFlags, 1),
        // property 'sessionToken'
        QtMocHelpers::PropertyData<QString>(11, QMetaType::QString, QMC::DefaultPropertyFlags, 2),
        // property 'roles'
        QtMocHelpers::PropertyData<QStringList>(12, QMetaType::QStringList, QMC::DefaultPropertyFlags, 3),
        // property 'permissions'
        QtMocHelpers::PropertyData<QStringList>(13, QMetaType::QStringList, QMC::DefaultPropertyFlags, 4),
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ConnectionState'
        QtMocHelpers::EnumData<enum ConnectionState>(45, 45, QMC::EnumIsScoped).add({
            {   47, ConnectionState::Disconnected },
            {   48, ConnectionState::Connecting },
            {   49, ConnectionState::Connected },
            {   50, ConnectionState::Authenticated },
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
        case 3: _t->rolesChanged(); break;
        case 4: _t->permissionsChanged(); break;
        case 5: _t->authenticated((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[3]))); break;
        case 6: _t->authFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->messageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 8: _t->jsonMessageSent((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 9: _t->jsonMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 10: _t->binaryFrameReceived((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 11: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 12: _t->heartbeatAcknowledged(); break;
        case 13: _t->connectToServer((*reinterpret_cast<std::add_pointer_t<TcpConnectionManager::ConnectionConfig>>(_a[1]))); break;
        case 14: _t->disconnectFromServer(); break;
        case 15: { QString _r = _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<ResponseCallback>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 16: { QString _r = _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->sendBinaryFrame((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 18: _t->onSocketConnected(); break;
        case 19: _t->onSocketDisconnected(); break;
        case 20: _t->onSocketReadyRead(); break;
        case 21: _t->onSocketErrorOccurred((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 22: _t->onHeartbeatTimeout(); break;
        case 23: _t->onReconnectTimeout(); break;
        case 24: _t->onRequestTimeout((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 21:
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
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::rolesChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::permissionsChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QString & , const QStringList & , const QStringList & )>(_a, &TcpConnectionManager::authenticated, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(int , const QString & )>(_a, &TcpConnectionManager::authFailed, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::messageReceived, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::jsonMessageSent, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QJsonObject & )>(_a, &TcpConnectionManager::jsonMessageReceived, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QByteArray & )>(_a, &TcpConnectionManager::binaryFrameReceived, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)(const QString & )>(_a, &TcpConnectionManager::errorOccurred, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpConnectionManager::*)()>(_a, &TcpConnectionManager::heartbeatAcknowledged, 12))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<enum ConnectionState*>(_v) = _t->connectionState(); break;
        case 1: *reinterpret_cast<bool*>(_v) = _t->isAuthenticated(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->sessionToken(); break;
        case 3: *reinterpret_cast<QStringList*>(_v) = _t->roles(); break;
        case 4: *reinterpret_cast<QStringList*>(_v) = _t->permissions(); break;
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
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 25)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 25;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
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
void TcpConnectionManager::rolesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void TcpConnectionManager::permissionsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void TcpConnectionManager::authenticated(const QString & _t1, const QStringList & _t2, const QStringList & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1, _t2, _t3);
}

// SIGNAL 6
void TcpConnectionManager::authFailed(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void TcpConnectionManager::messageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void TcpConnectionManager::jsonMessageSent(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}

// SIGNAL 9
void TcpConnectionManager::jsonMessageReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1);
}

// SIGNAL 10
void TcpConnectionManager::binaryFrameReceived(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void TcpConnectionManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void TcpConnectionManager::heartbeatAcknowledged()
{
    QMetaObject::activate(this, &staticMetaObject, 12, nullptr);
}
QT_WARNING_POP
