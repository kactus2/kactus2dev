/****************************************************************************
** Meta object code from reading C++ file 'SignalMultiplexer.h'
**
** Created: Mon 3. Oct 22:07:40 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/SignalMultiplexer.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'SignalMultiplexer.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__SignalMultiplexer[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      28,   24,   23,   23, 0x08,
      57,   24,   23,   23, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GCF__SignalMultiplexer[] = {
    "GCF::SignalMultiplexer\0\0obj\0"
    "receiver_destroyed(QObject*)\0"
    "sender_destroyed(QObject*)\0"
};

const QMetaObject GCF::SignalMultiplexer::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__SignalMultiplexer,
      qt_meta_data_GCF__SignalMultiplexer, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::SignalMultiplexer::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::SignalMultiplexer::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::SignalMultiplexer::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__SignalMultiplexer))
        return static_cast<void*>(const_cast< SignalMultiplexer*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::SignalMultiplexer::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: receiver_destroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 1: sender_destroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
