/****************************************************************************
** Meta object code from reading C++ file 'SyncManager.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../Sync/SyncManager.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SyncManager.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN11SyncManagerE_t {};
} // unnamed namespace

template <> constexpr inline auto SyncManager::qt_create_metaobjectdata<qt_meta_tag_ZN11SyncManagerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "SyncManager",
        "syncStarted",
        "",
        "syncCompleted",
        "syncFailed",
        "reason",
        "personSyncProgress",
        "count",
        "faceSyncProgress",
        "sendMessage",
        "QJsonObject",
        "message",
        "requestFaceDbReload",
        "requestSync",
        "handlePersonSync",
        "handleFaceSyncBegin",
        "handleFaceItem",
        "header",
        "payload",
        "handleFaceSyncEnd"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'syncStarted'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'syncCompleted'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'syncFailed'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'personSyncProgress'
        QtMocHelpers::SignalData<void(int)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'faceSyncProgress'
        QtMocHelpers::SignalData<void(int)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 7 },
        }}),
        // Signal 'sendMessage'
        QtMocHelpers::SignalData<void(const QJsonObject &)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Signal 'requestFaceDbReload'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'requestSync'
        QtMocHelpers::SlotData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Slot 'handlePersonSync'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'handleFaceSyncBegin'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
        // Slot 'handleFaceItem'
        QtMocHelpers::SlotData<void(const QJsonObject &, const QByteArray &)>(16, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 17 }, { QMetaType::QByteArray, 18 },
        }}),
        // Slot 'handleFaceSyncEnd'
        QtMocHelpers::SlotData<void(const QJsonObject &)>(19, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 10, 11 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<SyncManager, qt_meta_tag_ZN11SyncManagerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject SyncManager::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SyncManagerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SyncManagerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN11SyncManagerE_t>.metaTypes,
    nullptr
} };

void SyncManager::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<SyncManager *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->syncStarted(); break;
        case 1: _t->syncCompleted(); break;
        case 2: _t->syncFailed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 3: _t->personSyncProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 4: _t->faceSyncProgress((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 5: _t->sendMessage((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 6: _t->requestFaceDbReload(); break;
        case 7: _t->requestSync(); break;
        case 8: _t->handlePersonSync((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 9: _t->handleFaceSyncBegin((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        case 10: _t->handleFaceItem((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[2]))); break;
        case 11: _t->handleFaceSyncEnd((*reinterpret_cast<std::add_pointer_t<QJsonObject>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)()>(_a, &SyncManager::syncStarted, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)()>(_a, &SyncManager::syncCompleted, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)(const QString & )>(_a, &SyncManager::syncFailed, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)(int )>(_a, &SyncManager::personSyncProgress, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)(int )>(_a, &SyncManager::faceSyncProgress, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)(const QJsonObject & )>(_a, &SyncManager::sendMessage, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (SyncManager::*)()>(_a, &SyncManager::requestFaceDbReload, 6))
            return;
    }
}

const QMetaObject *SyncManager::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *SyncManager::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN11SyncManagerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int SyncManager::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void SyncManager::syncStarted()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void SyncManager::syncCompleted()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void SyncManager::syncFailed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void SyncManager::personSyncProgress(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void SyncManager::faceSyncProgress(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void SyncManager::sendMessage(const QJsonObject & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void SyncManager::requestFaceDbReload()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
