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
        "personDataReceived",
        "QList<ServerProtocol::PersonData>",
        "persons",
        "uploadFinished",
        "success",
        "faceSyncItemReceived",
        "QJsonObject",
        "header",
        "payload",
        "onConnectionConnected",
        "onConnectionDisconnected",
        "onConnectionStateChanged",
        "onMessageReceived",
        "onBinaryFrameReceived",
        "onHeartbeatTimeout",
        "onSendError",
        "onSendHeartbeat",
        "data"
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
        // Signal 'personDataReceived'
        QtMocHelpers::SignalData<void(const QVector<ServerProtocol::PersonData> &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 11, 12 },
        }}),
        // Signal 'uploadFinished'
        QtMocHelpers::SignalData<void(bool, const QString &)>(13, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 14 }, { QMetaType::QString, 9 },
        }}),
        // Signal 'faceSyncItemReceived'
        QtMocHelpers::SignalData<void(const QJsonObject &, const QByteArray &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 16, 17 }, { QMetaType::QByteArray, 18 },
        }}),
        // Slot 'onConnectionConnected'
        QtMocHelpers::SlotData<void()>(19, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionDisconnected'
        QtMocHelpers::SlotData<void()>(20, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onConnectionStateChanged'
        QtMocHelpers::SlotData<void(bool)>(21, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::Bool, 5 },
        }}),
        // Slot 'onMessageReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(22, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 16, 9 },
        }}),
        // Slot 'onBinaryFrameReceived'
        QtMocHelpers::SlotData<void(const QJsonObject &, const QByteArray &)>(23, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { 0x80000000 | 16, 17 }, { QMetaType::QByteArray, 18 },
        }}),
        // Slot 'onHeartbeatTimeout'
        QtMocHelpers::SlotData<void()>(24, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendError'
        QtMocHelpers::SlotData<void()>(25, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'onSendHeartbeat'
        QtMocHelpers::SlotData<void(const QByteArray &)>(26, 2, QMC::AccessPrivate, QMetaType::Void, {{
            { QMetaType::QByteArray, 27 },
        }}),
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
        case 5: _t->personDataReceived((*reinterpret_cast<std::add_pointer_t<QList<ServerProtocol::PersonData>>>(_a[1]))); break;
        case 6: _t->uploadFinished((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2]))); break;
        case 7: _t->faceSyncItemReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 8: _t->onConnectionConnected(); break;
        case 9: _t->onConnectionDisconnected(); break;
        case 10: _t->onConnectionStateChanged((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1]))); break;
        case 11: _t->onMessageReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 12: _t->onBinaryFrameReceived((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 13: _t->onHeartbeatTimeout(); break;
        case 14: _t->onSendError(); break;
        case 15: _t->onSendHeartbeat((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
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
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QVector<ServerProtocol::PersonData> & )>(_a, &Networkclient::personDataReceived, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(bool , const QString & )>(_a, &Networkclient::uploadFinished, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (Networkclient::*)(const QJsonObject & , const QByteArray & )>(_a, &Networkclient::faceSyncItemReceived, 7))
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
        if (_id < 16)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 16;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 16)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 16;
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
void Networkclient::personDataReceived(const QVector<ServerProtocol::PersonData> & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void Networkclient::uploadFinished(bool _t1, const QString & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1, _t2);
}

// SIGNAL 7
void Networkclient::faceSyncItemReceived(const QJsonObject & _t1, const QByteArray & _t2)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2);
}
QT_WARNING_POP
