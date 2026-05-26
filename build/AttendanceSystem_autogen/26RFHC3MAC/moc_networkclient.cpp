/****************************************************************************
** Meta object code from reading C++ file 'networkclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../NetworkClient/networkclient.h"
#include <QtCore/qmetatype.h>
#include <QtCore/QList>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'networkclient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN13NetworkclientE_t {};
} // unnamed namespace

template <> constexpr inline auto Networkclient::qt_create_metaobjectdata<qt_meta_tag_ZN13NetworkclientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Networkclient",
        "connected",
        "",
        "disconnected",
        "networkStateChanged",
        "isOnline",
        "authSuccess",
        "authFailed",
        "code",
        "message",
        "devicePendingAuth",
        "personDataReceived",
        "QList<ServerProtocol::PersonData>",
        "persons",
        "uploadFinished",
        "success",
        "faceSyncItemReceived",
        "QJsonObject",
        "header",
        "payload",
        "attendanceReportResult",
        "employeeId",
        "deviceCommandReceived",
        "personSyncReceived",
        "faceSyncBeginReceived",
        "faceSyncEndReceived",
        "tokenRefreshResponse",
        "scheduleReconnect",
        "onConnectionConnected",
        "onConnectionDisconnected",
        "onConnectionStateChanged",
        "onMessageReceived",
        "onBinaryFrameReceived",
        "onHeartbeatTimeout",
        "onSendError",
        "onSendHeartbeat",
        "data",
        "onOutboxRetryTick"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'connected'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'disconnected'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'networkStateChanged'
        QtMocHelpers::SignalData<void(bool)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Signal 'authSuccess'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'authFailed'
        QtMocHelpers::SignalData<void(int, const QString &)>(7, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 8 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'devicePendingAuth'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'personDataReceived'
        QtMocHelpers::SignalData<void(const QVector<ServerProtocol::PersonData> &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 12, 13 },
        }}),
        // Signal 'uploadFinished'
        QtMocHelpers::SignalData<void(bool, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 15 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'faceSyncItemReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &, const QByteArray &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 18 }, { QMetaType::QByteArray, 19 },
        }}),
        // Signal 'attendanceReportResult'
        QtMocHelpers::SignalData<void(const QString &, bool, const QString &)>(20, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 21 }, { QMetaType::Bool, 15 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'deviceCommandReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(22, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Signal 'personSyncReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(23, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Signal 'faceSyncBeginReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(24, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Signal 'faceSyncEndReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(25, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Signal 'tokenRefreshResponse'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(26, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Slot 'scheduleReconnect'
        QtMocHelpers::SlotData<void()>(27, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'onConnectionConnected'
        QtMocHelpers::SlotData<void()>(28, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionDisconnected'
        QtMocHelpers::SlotData<void()>(29, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionStateChanged'
        QtMocHelpers::SlotData<void(bool)>(30, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Slot 'onMessageReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(31, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 17, 9 },
        }}),
        // Slot 'onBinaryFrameReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &, const QByteArray &)>(32, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 17, 18 }, { QMetaType::QByteArray, 19 },
        }}),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(33, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendError'
        QtMocHelpers::SlotData<void()>(34, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendHeartbeat'
        QtMocHelpers::SlotData<void(const QByteArray &)>(35, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 36 },
        }}),
        // Slot 'onOutboxRetryTick'
        QtMocHelpers::SlotData<void()>(37, 2, QMC::AccessPrivate, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Networkclient, qt_meta_tag_ZN13NetworkclientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Networkclient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkclientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkclientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN13NetworkclientE_t>.metaTypes,
    nullptr
} };

void Networkclient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Networkclient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->connected(); break;
        case 1: _t->disconnected(); break;
        case 2: _t->networkStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 3: _t->authSuccess(); break;
        case 4: _t->authFailed((*reinterpret_cast<std::add_pointer_t<int>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 5: _t->devicePendingAuth(); break;
        case 6: _t->personDataReceived((*reinterpret_cast<std::add_pointer_t<QList<ServerProtocol::PersonData>>>(_a[1]))); break;
        case 7: _t->uploadFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 8: _t->faceSyncItemReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 9: _t->attendanceReportResult((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3]))); break;
        case 10: _t->deviceCommandReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 11: _t->personSyncReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 12: _t->faceSyncBeginReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 13: _t->faceSyncEndReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 14: _t->tokenRefreshResponse((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 15: _t->scheduleReconnect(); break;
        case 16: _t->onConnectionConnected(); break;
        case 17: _t->onConnectionDisconnected(); break;
        case 18: _t->onConnectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 19: _t->onMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 20: _t->onBinaryFrameReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 21: _t->onHeartbeatTimeout(); break;
        case 22: _t->onSendError(); break;
        case 23: _t->onSendHeartbeat((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 24: _t->onOutboxRetryTick(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)()>(_a, &Networkclient::connected, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)()>(_a, &Networkclient::disconnected, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(bool )>(_a, &Networkclient::networkStateChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)()>(_a, &Networkclient::authSuccess, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(int , const QString & )>(_a, &Networkclient::authFailed, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)()>(_a, &Networkclient::devicePendingAuth, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QVector<ServerProtocol::PersonData> & )>(_a, &Networkclient::personDataReceived, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(bool , const QString & )>(_a, &Networkclient::uploadFinished, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & , const QByteArray & )>(_a, &Networkclient::faceSyncItemReceived, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QString & , bool , const QString & )>(_a, &Networkclient::attendanceReportResult, 9))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & )>(_a, &Networkclient::deviceCommandReceived, 10))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & )>(_a, &Networkclient::personSyncReceived, 11))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & )>(_a, &Networkclient::faceSyncBeginReceived, 12))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & )>(_a, &Networkclient::faceSyncEndReceived, 13))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & )>(_a, &Networkclient::tokenRefreshResponse, 14))
            return;
    }
}

const QMetaObject *Networkclient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Networkclient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN13NetworkclientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Networkclient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
    return _id;
}

// SIGNAL 0
void Networkclient::connected()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Networkclient::disconnected()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Networkclient::networkStateChanged(bool _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void Networkclient::authSuccess()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Networkclient::authFailed(int _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1, _t2);
}

// SIGNAL 5
void Networkclient::devicePendingAuth()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Networkclient::personDataReceived(const QVector<ServerProtocol::PersonData> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}

// SIGNAL 7
void Networkclient::uploadFinished(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}

// SIGNAL 8
void Networkclient::faceSyncItemReceived(const QJsonObject & _t1, const QByteArray & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 8, nullptr, _t1, _t2);
}

// SIGNAL 9
void Networkclient::attendanceReportResult(const QString & _t1, bool _t2, const QString & _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 9, nullptr, _t1, _t2, _t3);
}

// SIGNAL 10
void Networkclient::deviceCommandReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 10, nullptr, _t1);
}

// SIGNAL 11
void Networkclient::personSyncReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 11, nullptr, _t1);
}

// SIGNAL 12
void Networkclient::faceSyncBeginReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 12, nullptr, _t1);
}

// SIGNAL 13
void Networkclient::faceSyncEndReceived(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 13, nullptr, _t1);
}

// SIGNAL 14
void Networkclient::tokenRefreshResponse(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 14, nullptr, _t1);
}
QT_WARNING_POP
