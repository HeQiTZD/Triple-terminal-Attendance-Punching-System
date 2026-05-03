/****************************************************************************
** Meta object code from reading C++ file 'tcpserver.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/TcpServer/tcpserver.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tcpserver.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9TcpServerE_t {};
} // unnamed namespace

template <> constexpr inline auto TcpServer::qt_create_metaobjectdata<qt_meta_tag_ZN9TcpServerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "TcpServer",
        "isRunningChanged",
        "",
        "clientCountChanged",
        "clientsChanged",
        "clientConnected",
        "deviceId",
        "ipAddress",
        "clientDisconnected",
        "attendanceRecordReceived",
        "QJsonObject",
        "record",
        "deviceStatusReceived",
        "status",
        "errorOccurred",
        "errorString",
        "syncRequested",
        "onNewConnection",
        "onSocketDisconnected",
        "onSocketReadyRead",
        "onHeartbeatTimeout",
        "startServer",
        "pot",
        "stopServer",
        "sendToClient",
        "data",
        "brodcastsToAll",
        "connectedClients",
        "QVariantList",
        "isRunning",
        "clientCount"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'isRunningChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clientCountChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clientsChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'clientConnected'
        QtMocHelpers::SignalData<void(const QString &, const QString &)>(5, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Signal 'clientDisconnected'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Signal 'attendanceRecordReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'deviceStatusReceived'
        QtMocHelpers::SignalData<void(const QString &, const QJsonObject &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 }, { 0x80000000 | 10, 13 },
        }}),
        // Signal 'errorOccurred'
        QtMocHelpers::SignalData<void(const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 },
        }}),
        // Signal 'syncRequested'
        QtMocHelpers::SignalData<void(const QString &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 6 },
        }}),
        // Slot 'onNewConnection'
        QtMocHelpers::SlotData<void()>(17, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketDisconnected'
        QtMocHelpers::SlotData<void()>(18, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSocketReadyRead'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'startServer'
        QtMocHelpers::MethodData<bool(quint16)>(21, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::UShort, 22 },
        }}),
        // Method 'startServer'
        QtMocHelpers::MethodData<bool()>(21, 2, QMC::AccessPublic | QMC::MethodCloned, QMetaType::Bool),
        // Method 'stopServer'
        QtMocHelpers::MethodData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'sendToClient'
        QtMocHelpers::MethodData<bool(const QString &, const QJsonObject &)>(24, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 6 }, { 0x80000000 | 10, 25 },
        }}),
        // Method 'brodcastsToAll'
        QtMocHelpers::MethodData<void(const QJsonObject &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 25 },
        }}),
        // Method 'connectedClients'
        QtMocHelpers::MethodData<QVariantList() const>(27, 2, QMC::AccessPublic, 0x80000000 | 28),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'isRunning'
        QtMocHelpers::PropertyData<bool>(29, QMetaType::Bool, QMC::DefaultPropertyFlags, 0),
        // property 'clientCount'
        QtMocHelpers::PropertyData<int>(30, QMetaType::Int, QMC::DefaultPropertyFlags, 1),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<TcpServer, qt_meta_tag_ZN9TcpServerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject TcpServer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TcpServerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TcpServerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9TcpServerE_t>.metaTypes,
    nullptr
} };

void TcpServer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<TcpServer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->isRunningChanged(); break;
        case 1: _t->clientCountChanged(); break;
        case 2: _t->clientsChanged(); break;
        case 3: _t->clientConnected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 4: _t->clientDisconnected((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 5: _t->attendanceRecordReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 6: _t->deviceStatusReceived((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2]))); break;
        case 7: _t->errorOccurred((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 8: _t->syncRequested((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 9: _t->onNewConnection(); break;
        case 10: _t->onSocketDisconnected(); break;
        case 11: _t->onSocketReadyRead(); break;
        case 12: _t->onHeartbeatTimeout(); break;
        case 13: { bool _r = _t->startServer((*reinterpret_cast<std::add_pointer_t<quint16>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 14: { bool _r = _t->startServer();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 15: _t->stopServer(); break;
        case 16: { bool _r = _t->sendToClient((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 17: _t->brodcastsToAll((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 18: { QVariantList _r = _t->connectedClients();
            if (_a[0]) *reinterpret_cast<QVariantList*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)()>(_a, &TcpServer::isRunningChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)()>(_a, &TcpServer::clientCountChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)()>(_a, &TcpServer::clientsChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QString & , const QString & )>(_a, &TcpServer::clientConnected, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QString & )>(_a, &TcpServer::clientDisconnected, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QJsonObject & )>(_a, &TcpServer::attendanceRecordReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QString & , const QJsonObject & )>(_a, &TcpServer::deviceStatusReceived, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QString & )>(_a, &TcpServer::errorOccurred, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (TcpServer::*)(const QString & )>(_a, &TcpServer::syncRequested, 8))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<bool*>(_v) = _t->isRunning(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->clientCount(); break;
        default: break;
        }
    }
}

const QMetaObject *TcpServer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *TcpServer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9TcpServerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int TcpServer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 19)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 19;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 19)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 19;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void TcpServer::isRunningChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void TcpServer::clientCountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void TcpServer::clientsChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void TcpServer::clientConnected(const QString & _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1, _t2);
}

// SIGNAL 4
void TcpServer::clientDisconnected(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void TcpServer::attendanceRecordReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void TcpServer::deviceStatusReceived(const QString & _t1, const QJsonObject & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void TcpServer::errorOccurred(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1);
}

// SIGNAL 8
void TcpServer::syncRequested(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1);
}
QT_WARNING_POP
