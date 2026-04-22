/****************************************************************************
** Meta object code from reading C++ file 'faceData.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../src/Models/faceData.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'faceData.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN8FaceDataE_t {};
} // unnamed namespace

template <> constexpr inline auto FaceData::qt_create_metaobjectdata<qt_meta_tag_ZN8FaceDataE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "FaceData",
        "idChanged",
        "",
        "id",
        "personIdChanged",
        "personId",
        "featureVectorChanged",
        "featureVector",
        "statusChanged",
        "status",
        "createdAtChanged",
        "createdAt",
        "updatedAtChanged",
        "updatedAt",
        "featureSizeChanged",
        "featureSize"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'idChanged'
        QtMocHelpers::SignalData<void(int)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 3 },
        }}),
        // Signal 'personIdChanged'
        QtMocHelpers::SignalData<void(int)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 5 },
        }}),
        // Signal 'featureVectorChanged'
        QtMocHelpers::SignalData<void(const QByteArray &)>(6, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QByteArray, 7 },
        }}),
        // Signal 'statusChanged'
        QtMocHelpers::SignalData<void(const QString &)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 9 },
        }}),
        // Signal 'createdAtChanged'
        QtMocHelpers::SignalData<void(const QDateTime &)>(10, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QDateTime, 11 },
        }}),
        // Signal 'updatedAtChanged'
        QtMocHelpers::SignalData<void(const QDateTime &)>(12, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QDateTime, 13 },
        }}),
        // Signal 'featureSizeChanged'
        QtMocHelpers::SignalData<void(int)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Int, 15 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'id'
        QtMocHelpers::PropertyData<int>(3, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'personId'
        QtMocHelpers::PropertyData<int>(5, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'featureVector'
        QtMocHelpers::PropertyData<QByteArray>(7, QMetaType::QByteArray, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'status'
        QtMocHelpers::PropertyData<QString>(9, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'createdAt'
        QtMocHelpers::PropertyData<QDateTime>(11, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'updatedAt'
        QtMocHelpers::PropertyData<QDateTime>(13, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'featureSize'
        QtMocHelpers::PropertyData<int>(15, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<FaceData, qt_meta_tag_ZN8FaceDataE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject FaceData::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8FaceDataE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8FaceDataE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN8FaceDataE_t>.metaTypes,
    nullptr
} };

void FaceData::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<FaceData *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->idChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 1: _t->personIdChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        case 2: _t->featureVectorChanged((*reinterpret_cast<std::add_pointer_t<QByteArray>>(_a[1]))); break;
        case 3: _t->statusChanged((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 4: _t->createdAtChanged((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1]))); break;
        case 5: _t->updatedAtChanged((*reinterpret_cast<std::add_pointer_t<QDateTime>>(_a[1]))); break;
        case 6: _t->featureSizeChanged((*reinterpret_cast<std::add_pointer_t<int>>(_a[1]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(int )>(_a, &FaceData::idChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(int )>(_a, &FaceData::personIdChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(const QByteArray & )>(_a, &FaceData::featureVectorChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(const QString & )>(_a, &FaceData::statusChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(const QDateTime & )>(_a, &FaceData::createdAtChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(const QDateTime & )>(_a, &FaceData::updatedAtChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (FaceData::*)(int )>(_a, &FaceData::featureSizeChanged, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->id(); break;
        case 1: *reinterpret_cast<int*>(_v) = _t->personId(); break;
        case 2: *reinterpret_cast<QByteArray*>(_v) = _t->featureVector(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->status(); break;
        case 4: *reinterpret_cast<QDateTime*>(_v) = _t->createdAt(); break;
        case 5: *reinterpret_cast<QDateTime*>(_v) = _t->updatedAt(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->featureSize(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setId(*reinterpret_cast<int*>(_v)); break;
        case 1: _t->setPersonId(*reinterpret_cast<int*>(_v)); break;
        case 2: _t->setFeatureVector(*reinterpret_cast<QByteArray*>(_v)); break;
        case 3: _t->setStatus(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setCreatedAt(*reinterpret_cast<QDateTime*>(_v)); break;
        case 5: _t->setUpdatedAt(*reinterpret_cast<QDateTime*>(_v)); break;
        case 6: _t->setFeatureSize(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *FaceData::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *FaceData::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN8FaceDataE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int FaceData::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 7)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 7;
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
void FaceData::idChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void FaceData::personIdChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void FaceData::featureVectorChanged(const QByteArray & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 2, nullptr, _t1);
}

// SIGNAL 3
void FaceData::statusChanged(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 3, nullptr, _t1);
}

// SIGNAL 4
void FaceData::createdAtChanged(const QDateTime & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 4, nullptr, _t1);
}

// SIGNAL 5
void FaceData::updatedAtChanged(const QDateTime & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 5, nullptr, _t1);
}

// SIGNAL 6
void FaceData::featureSizeChanged(int _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 6, nullptr, _t1);
}
QT_WARNING_POP
