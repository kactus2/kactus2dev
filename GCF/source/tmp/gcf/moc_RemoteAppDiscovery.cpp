/****************************************************************************
** Meta object code from reading C++ file 'RemoteAppDiscovery.h'
**
** Created: Mon 3. Oct 22:07:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/RemoteAppDiscovery.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteAppDiscovery.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__RemoteAppDiscovery[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      35,   25,   24,   24, 0x05,
      77,   73,   24,   24, 0x05,

 // slots: signature, parameters, type, tag, flags
     125,   24,   24,   24, 0x08,
     149,   24,   24,   24, 0x08,
     173,   24,   24,   24, 0x08,
     188,   24,   24,   24, 0x08,
     216,  209,   24,   24, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GCF__RemoteAppDiscovery[] = {
    "GCF::RemoteAppDiscovery\0\0addr,port\0"
    "discoveredRemoteApp(QHostAddress,int)\0"
    "rap\0discoveredRemoteApp(GCF::RemoteAppAccessPoint*)\0"
    "sendBroadcastDatagram()\0readBroadcastDatagram()\0"
    "rapConnected()\0rapCouldNotConnect()\0"
    "object\0rapDestroyed(QObject*)\0"
};

const QMetaObject GCF::RemoteAppDiscovery::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__RemoteAppDiscovery,
      qt_meta_data_GCF__RemoteAppDiscovery, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::RemoteAppDiscovery::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::RemoteAppDiscovery::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::RemoteAppDiscovery::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__RemoteAppDiscovery))
        return static_cast<void*>(const_cast< RemoteAppDiscovery*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::RemoteAppDiscovery::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: discoveredRemoteApp((*reinterpret_cast< const QHostAddress(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: discoveredRemoteApp((*reinterpret_cast< GCF::RemoteAppAccessPoint*(*)>(_a[1]))); break;
        case 2: sendBroadcastDatagram(); break;
        case 3: readBroadcastDatagram(); break;
        case 4: rapConnected(); break;
        case 5: rapCouldNotConnect(); break;
        case 6: rapDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void GCF::RemoteAppDiscovery::discoveredRemoteApp(const QHostAddress & _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void GCF::RemoteAppDiscovery::discoveredRemoteApp(GCF::RemoteAppAccessPoint * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
