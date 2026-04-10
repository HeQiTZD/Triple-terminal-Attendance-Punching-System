/****************************************************************************
** Meta object code from reading C++ file 'Person.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/Models/Person.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Person.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN6PersonE_t {};
} // unnamed namespace

template <> constexpr inline auto Person::qt_create_metaobjectdata<qt_meta_tag_ZN6PersonE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "Person",
        "idChanged",
        "",
        "nameChanged",
        "employeeIdChanged",
        "departmentChanged",
        "positionChanged",
        "createdAtChanged",
        "updatedAtChanged",
        "id",
        "name",
        "employeeId",
        "department",
        "position",
        "createdAt",
        "updatedAt"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'idChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'nameChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'employeeIdChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'departmentChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'positionChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'createdAtChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'updatedAtChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'id'
        QtMocHelpers::PropertyData<int>(9, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'name'
        QtMocHelpers::PropertyData<QString>(10, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 1),
        // property 'employeeId'
        QtMocHelpers::PropertyData<QString>(11, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'department'
        QtMocHelpers::PropertyData<QString>(12, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'position'
        QtMocHelpers::PropertyData<QString>(13, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'createdAt'
        QtMocHelpers::PropertyData<QDateTime>(14, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'updatedAt'
        QtMocHelpers::PropertyData<QDateTime>(15, QMetaType::QDateTime, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<Person, qt_meta_tag_ZN6PersonE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject Person::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6PersonE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6PersonE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN6PersonE_t>.metaTypes,
    nullptr
} };

void Person::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<Person *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->idChanged(); break;
        case 1: _t->nameChanged(); break;
        case 2: _t->employeeIdChanged(); break;
        case 3: _t->departmentChanged(); break;
        case 4: _t->positionChanged(); break;
        case 5: _t->createdAtChanged(); break;
        case 6: _t->updatedAtChanged(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::idChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::nameChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::employeeIdChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::departmentChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::positionChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::createdAtChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (Person::*)()>(_a, &Person::updatedAtChanged, 6))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<int*>(_v) = _t->id(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->name(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->employeeId(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->department(); break;
        case 4: *reinterpret_cast<QString*>(_v) = _t->position(); break;
        case 5: *reinterpret_cast<QDateTime*>(_v) = _t->createdAt(); break;
        case 6: *reinterpret_cast<QDateTime*>(_v) = _t->updatedAt(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setId(*reinterpret_cast<int*>(_v)); break;
        case 1: _t->setName(*reinterpret_cast<QString*>(_v)); break;
        case 2: _t->setEmployeeId(*reinterpret_cast<QString*>(_v)); break;
        case 3: _t->setDepartment(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setPosition(*reinterpret_cast<QString*>(_v)); break;
        case 5: _t->setCreatedAt(*reinterpret_cast<QDateTime*>(_v)); break;
        case 6: _t->setUpdatedAt(*reinterpret_cast<QDateTime*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *Person::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Person::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN6PersonE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int Person::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
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
void Person::idChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void Person::nameChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void Person::employeeIdChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void Person::departmentChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void Person::positionChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void Person::createdAtChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void Person::updatedAtChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}
QT_WARNING_POP
