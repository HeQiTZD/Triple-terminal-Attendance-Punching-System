/****************************************************************************
** Meta object code from reading C++ file 'attendanceanalyzer.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.10.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/AttendanceAnalyzer/attendanceanalyzer.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'attendanceanalyzer.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18AttendanceAnalyzerE_t {};
} // unnamed namespace

template <> constexpr inline auto AttendanceAnalyzer::qt_create_metaobjectdata<qt_meta_tag_ZN18AttendanceAnalyzerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "AttendanceAnalyzer",
        "dailySummary",
        "QJsonArray",
        "",
        "start",
        "end",
        "department",
        "employeeId",
        "personSummary",
        "departmentSummary"
    };

    QtMocHelpers::UintData qt_methods {
        // Method 'dailySummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &, const QString &, const QString &) const>(1, 3, QMC::AccessPublic, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 }, { QMetaType::QString, 6 }, { QMetaType::QString, 7 },
        }}),
        // Method 'dailySummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &, const QString &) const>(1, 3, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 }, { QMetaType::QString, 6 },
        }}),
        // Method 'dailySummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &) const>(1, 3, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 },
        }}),
        // Method 'personSummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &, const QString &) const>(8, 3, QMC::AccessPublic, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 }, { QMetaType::QString, 6 },
        }}),
        // Method 'personSummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &) const>(8, 3, QMC::AccessPublic | QMC::MethodCloned, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 },
        }}),
        // Method 'departmentSummary'
        QtMocHelpers::MethodData<QJsonArray(const QDate &, const QDate &) const>(9, 3, QMC::AccessPublic, 0x80000000 | 2, {{
            { QMetaType::QDate, 4 }, { QMetaType::QDate, 5 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<AttendanceAnalyzer, qt_meta_tag_ZN18AttendanceAnalyzerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject AttendanceAnalyzer::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AttendanceAnalyzerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AttendanceAnalyzerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18AttendanceAnalyzerE_t>.metaTypes,
    nullptr
} };

void AttendanceAnalyzer::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<AttendanceAnalyzer *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QJsonArray _r = _t->dailySummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        case 1: { QJsonArray _r = _t->dailySummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        case 2: { QJsonArray _r = _t->dailySummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        case 3: { QJsonArray _r = _t->personSummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        case 4: { QJsonArray _r = _t->personSummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        case 5: { QJsonArray _r = _t->departmentSummary((*reinterpret_cast<std::add_pointer_t<QDate>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QDate>>(_a[2])));
            if (_a[0]) *reinterpret_cast<QJsonArray*>(_a[0]) = std::move(_r); }  break;
        default: ;
        }
    }
}

const QMetaObject *AttendanceAnalyzer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *AttendanceAnalyzer::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18AttendanceAnalyzerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int AttendanceAnalyzer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 6;
    }
    return _id;
}
QT_WARNING_POP
