/****************************************************************************
** Meta object code from reading C++ file 'RemoteAppAccessPoint.h'
**
** Created: Mon 3. Oct 22:07:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/RemoteAppAccessPoint.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteAppAccessPoint.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__RemoteAppAccessPoint[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      15,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      27,   26,   26,   26, 0x05,
      44,   26,   26,   26, 0x05,
      66,   26,   26,   26, 0x05,

 // slots: signature, parameters, type, tag, flags
      89,   26,   26,   26, 0x09,
     105,   26,   26,   26, 0x09,
     124,   26,   26,   26, 0x09,
     136,   26,   26,   26, 0x09,
     155,   26,   26,   26, 0x09,
     174,   26,   26,   26, 0x09,
     194,  190,   26,   26, 0x09,
     226,   26,   26,   26, 0x09,
     253,   26,   26,   26, 0x09,
     271,   26,   26,   26, 0x09,
     293,   26,   26,   26, 0x09,
     314,   26,   26,   26, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__RemoteAppAccessPoint[] = {
    "GCF::RemoteAppAccessPoint\0\0connectedToApp()\0"
    "disconnectedFromApp()\0couldNotConnectToApp()\0"
    "slotConnected()\0slotDisconnected()\0"
    "slotError()\0slotStateChanged()\0"
    "slotAboutToClose()\0slotReadyRead()\0"
    "obj\0remoteObjectDestroyed(QObject*)\0"
    "sendRequestAccessMessage()\0activateObjects()\0"
    "sendPendingMessages()\0checkConnectStatus()\0"
    "closeSocket()\0"
};

const QMetaObject GCF::RemoteAppAccessPoint::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__RemoteAppAccessPoint,
      qt_meta_data_GCF__RemoteAppAccessPoint, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::RemoteAppAccessPoint::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::RemoteAppAccessPoint::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::RemoteAppAccessPoint::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__RemoteAppAccessPoint))
        return static_cast<void*>(const_cast< RemoteAppAccessPoint*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::RemoteAppAccessPoint::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: connectedToApp(); break;
        case 1: disconnectedFromApp(); break;
        case 2: couldNotConnectToApp(); break;
        case 3: slotConnected(); break;
        case 4: slotDisconnected(); break;
        case 5: slotError(); break;
        case 6: slotStateChanged(); break;
        case 7: slotAboutToClose(); break;
        case 8: slotReadyRead(); break;
        case 9: remoteObjectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 10: sendRequestAccessMessage(); break;
        case 11: activateObjects(); break;
        case 12: sendPendingMessages(); break;
        case 13: checkConnectStatus(); break;
        case 14: closeSocket(); break;
        default: ;
        }
        _id -= 15;
    }
    return _id;
}

// SIGNAL 0
void GCF::RemoteAppAccessPoint::connectedToApp()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GCF::RemoteAppAccessPoint::disconnectedFromApp()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GCF::RemoteAppAccessPoint::couldNotConnectToApp()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}
QT_END_MOC_NAMESPACE
