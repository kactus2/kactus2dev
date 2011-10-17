/****************************************************************************
** Meta object code from reading C++ file 'GenericSignalSpy.h'
**
** Created: Wed 12. Oct 15:35:37 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/GenericSignalSpy.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'GenericSignalSpy.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__GenericSignalSpy[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   23,   22,   22, 0x05,

 // slots: signature, parameters, type, tag, flags
      62,   55,   22,   22, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__GenericSignalSpy[] = {
    "GCF::GenericSignalSpy\0\0args\0"
    "caughtSignal(QVariantList)\0object\0"
    "senderDestroyed(QObject*)\0"
};

const QMetaObject GCF::GenericSignalSpy::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__GenericSignalSpy,
      qt_meta_data_GCF__GenericSignalSpy, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::GenericSignalSpy::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::GenericSignalSpy::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::GenericSignalSpy::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__GenericSignalSpy))
        return static_cast<void*>(const_cast< GenericSignalSpy*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::GenericSignalSpy::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: caughtSignal((*reinterpret_cast< const QVariantList(*)>(_a[1]))); break;
        case 1: senderDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void GCF::GenericSignalSpy::caughtSignal(const QVariantList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
