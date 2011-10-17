/****************************************************************************
** Meta object code from reading C++ file 'AppAccessPoint.h'
**
** Created: Mon 3. Oct 22:07:42 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/AppAccessPoint.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppAccessPoint.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__AppAccessPoint[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       3,   19, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x09,

 // properties: name, type, flags
      49,   41, 0x0a095103,
      70,   65, 0x01095103,
      95,   87, 0x85095003,

       0        // eod
};

static const char qt_meta_stringdata_GCF__AppAccessPoint[] = {
    "GCF::AppAccessPoint\0\0slotNewConnection()\0"
    "QString\0AccessPointName\0bool\0"
    "AppAccessEnabled\0quint16\0ServerPort\0"
};

const QMetaObject GCF::AppAccessPoint::staticMetaObject = {
    { &QTcpServer::staticMetaObject, qt_meta_stringdata_GCF__AppAccessPoint,
      qt_meta_data_GCF__AppAccessPoint, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::AppAccessPoint::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::AppAccessPoint::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::AppAccessPoint::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__AppAccessPoint))
        return static_cast<void*>(const_cast< AppAccessPoint*>(this));
    return QTcpServer::qt_metacast(_clname);
}

int GCF::AppAccessPoint::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QTcpServer::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotNewConnection(); break;
        default: ;
        }
        _id -= 1;
    }
#ifndef QT_NO_PROPERTIES
      else if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast< QString*>(_v) = accessPointName(); break;
        case 1: *reinterpret_cast< bool*>(_v) = isAppAccessEnabled(); break;
        case 2: *reinterpret_cast< quint16*>(_v) = port(); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: setAccessPointName(*reinterpret_cast< QString*>(_v)); break;
        case 1: setAppAccessEnabled(*reinterpret_cast< bool*>(_v)); break;
        case 2: setPort(*reinterpret_cast< quint16*>(_v)); break;
        }
        _id -= 3;
    } else if (_c == QMetaObject::ResetProperty) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyDesignable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyScriptable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyStored) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyEditable) {
        _id -= 3;
    } else if (_c == QMetaObject::QueryPropertyUser) {
        _id -= 3;
    }
#endif // QT_NO_PROPERTIES
    return _id;
}
static const uint qt_meta_data_GCF__RemoteApp[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x09,
      35,   15,   15,   15, 0x09,
      47,   15,   15,   15, 0x09,
      66,   15,   15,   15, 0x09,
      86,   82,   15,   15, 0x09,
     112,   15,   15,   15, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__RemoteApp[] = {
    "GCF::RemoteApp\0\0slotDisconnected()\0"
    "slotError()\0slotAboutToClose()\0"
    "slotReadyRead()\0obj\0objectDestroyed(QObject*)\0"
    "sendPendingMessages()\0"
};

const QMetaObject GCF::RemoteApp::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__RemoteApp,
      qt_meta_data_GCF__RemoteApp, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::RemoteApp::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::RemoteApp::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::RemoteApp::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__RemoteApp))
        return static_cast<void*>(const_cast< RemoteApp*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::RemoteApp::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: slotDisconnected(); break;
        case 1: slotError(); break;
        case 2: slotAboutToClose(); break;
        case 3: slotReadyRead(); break;
        case 4: objectDestroyed((*reinterpret_cast< QObject*(*)>(_a[1]))); break;
        case 5: sendPendingMessages(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
