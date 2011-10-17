/****************************************************************************
** Meta object code from reading C++ file 'AbstractComponent.h'
**
** Created: Wed 12. Oct 15:35:38 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/AbstractComponent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AbstractComponent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__AbstractComponent[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      24,   23,   23,   23, 0x0a,
      37,   23,   23,   23, 0x0a,
      52,   48,   23,   23, 0x0a,
      72,   68,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GCF__AbstractComponent[] = {
    "GCF::AbstractComponent\0\0initialize()\0"
    "finalize()\0val\0setActive(bool)\0obj\0"
    "objectDestroyed(QObject*)\0"
};

const QMetaObject GCF::AbstractComponent::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__AbstractComponent,
      qt_meta_data_GCF__AbstractComponent, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::AbstractComponent::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::AbstractComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::AbstractComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__AbstractComponent))
        return static_cast<void*>(const_cast< AbstractComponent*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::AbstractComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: initialize(); break;
        case 1: finalize(); break;
        case 2: setActive((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
