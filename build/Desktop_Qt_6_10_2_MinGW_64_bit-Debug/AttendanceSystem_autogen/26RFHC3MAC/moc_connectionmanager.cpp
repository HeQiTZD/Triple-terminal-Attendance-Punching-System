/****************************************************************************
** Meta object code from reading C++ file 'connectionmanager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../NetworkClient/connectionmanager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'connectionmanager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN17ConnectionManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto ConnectionManager::qt_create_metaobjectdata<qt_meta_tag_ZN17ConnectionManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "ConnectionManager",
        "connected",
        "",
        "disconnected",
        "stateChanged",
        "isConnected",
        "connectionStateChanged",
        "ConnectionState",
        "oldState",
        "newState",
        "errorOccurred",
        "errorString",
        "onSocketConnected",
        "onSocketDisconnected",
        "onSocketError",
        "QAbstractSocket::SocketError",
        "error",
        "onReconnectTimeout",
        "Disconnected",
        "Connecting",
        "Connected",
        "Authenticated"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'stateChanged'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'connectionStateChanged'
        QtMocHelpers::SignalData<void(enum ConnectionState, enum ConnectionState)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 7, 8 }, { 0x80000000 | 7, 9 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 11 },
        }}),
        // Slot 'onSocketConnected'
        QtMocHelpers::SlotData<void()>(12, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketError'
        QtMocHelpers::SlotData<void(QAbstractSocket::SocketError)>(14, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 15, 16 },
        }}),
        // Slot 'onReconnectTimeout'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
        // enum 'ConnectionState'
        QtMocHelpers::EnumData<enum ConnectionState>(7, 7, QMC::EnumIsScoped).add({
            {   18, ConnectionState::Disconnected },
            {   19, ConnectionState::Connecting },
            {   20, ConnectionState::Connected },
            {   21, ConnectionState::Authenticated },
        }),
    };
    return QtMocHelpers::metaObjectData<ConnectionManager, qt_meta_tag_ZN17ConnectionManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject ConnectionManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN17ConnectionManagerE_t>.metaTypes,
    nullptr
} };

void ConnectionManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<ConnectionManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->stateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->connectionStateChanged((*reinterpret_cast<std::add_pointer_t<enum ConnectionState>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<enum ConnectionState>>(_a[2]))); break;
        case 4: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->onSocketConnected(); break;
        case 6: _t->onSocketDisconnected(); break;
        case 7: _t->onSocketError((*reinterpret_cast<std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        case 8: _t->onReconnectTimeout(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 7:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)()>(_a, &ConnectionManager::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)()>(_a, &ConnectionManager::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(bool )>(_a, &ConnectionManager::stateChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(ConnectionState , ConnectionState )>(_a, &ConnectionManager::connectionStateChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (ConnectionManager::*)(const QString & )>(_a, &ConnectionManager::errorOccurred, 4))
            return;
    }
}

const QMetaObject *ConnectionManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *ConnectionManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN17ConnectionManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int ConnectionManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void ConnectionManager::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void ConnectionManager::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void ConnectionManager::stateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void ConnectionManager::connectionStateChanged(ConnectionState _t1, ConnectionState _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void ConnectionManager::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}
QT_WARNING_POP
