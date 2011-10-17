/****************************************************************************
** Meta object code from reading C++ file 'RemoteObject.h'
**
** Created: Mon 3. Oct 22:07:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/RemoteObject.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'RemoteObject.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__RemoteObject[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
      23,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       8,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x05,
      31,   18,   18,   18, 0x05,
      45,   18,   18,   18, 0x05,
      93,   64,   18,   18, 0x05,
     176,  146,   18,   18, 0x05,
     224,  146,   18,   18, 0x05,
     272,   18,   18,   18, 0x05,
     317,  291,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
     359,   18,   18,   18, 0x08,
     390,  380,   18,   18, 0x08,

 // methods: signature, parameters, type, tag, flags
     473,  426,  419,   18, 0x02,
     601,  559,  419,   18, 0x22,
     715,  678,  419,   18, 0x22,
     815,  783,  419,   18, 0x22,
     901,  874,  419,   18, 0x22,
     973,  951,  419,   18, 0x22,
    1031, 1014,  419,   18, 0x22,
    1075, 1063,  419,   18, 0x22,
    1105, 1098,  419,   18, 0x22,
    1134, 1119,  419,   18, 0x02,
    1188, 1179, 1170,   18, 0x02,
    1215,   18,   18,   18, 0x02,
    1233, 1179, 1170,   18, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_GCF__RemoteObject[] = {
    "GCF::RemoteObject\0\0activated()\0"
    "deactivated()\0couldNotActivate()\0"
    "callId,success,result,method\0"
    "methodInvocationResult(qint32,bool,QVariant,QString)\0"
    "callId,success,value,propName\0"
    "setPropertyResult(qint32,bool,QVariant,QString)\0"
    "getPropertyResult(qint32,bool,QVariant,QString)\0"
    "propertiesCached()\0callId,success,memberName\0"
    "makeConnectionResult(qint32,bool,QString)\0"
    "sendQueuedMessages()\0propNames\0"
    "cacheProperties(QStringList)\0qint32\0"
    "method,arg1,arg2,arg3,arg4,arg5,arg6,arg7,arg8\0"
    "call(QString,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVa"
    "riant,QVariant)\0"
    "method,arg1,arg2,arg3,arg4,arg5,arg6,arg7\0"
    "call(QString,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant,QVa"
    "riant)\0"
    "method,arg1,arg2,arg3,arg4,arg5,arg6\0"
    "call(QString,QVariant,QVariant,QVariant,QVariant,QVariant,QVariant)\0"
    "method,arg1,arg2,arg3,arg4,arg5\0"
    "call(QString,QVariant,QVariant,QVariant,QVariant,QVariant)\0"
    "method,arg1,arg2,arg3,arg4\0"
    "call(QString,QVariant,QVariant,QVariant,QVariant)\0"
    "method,arg1,arg2,arg3\0"
    "call(QString,QVariant,QVariant,QVariant)\0"
    "method,arg1,arg2\0call(QString,QVariant,QVariant)\0"
    "method,arg1\0call(QString,QVariant)\0"
    "method\0call(QString)\0propName,value\0"
    "setObjectProperty(QString,QVariant)\0"
    "QVariant\0propName\0getObjectProperty(QString)\0"
    "cacheProperties()\0getCachedObjectProperty(QString)\0"
};

const QMetaObject GCF::RemoteObject::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_GCF__RemoteObject,
      qt_meta_data_GCF__RemoteObject, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::RemoteObject::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::RemoteObject::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::RemoteObject::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__RemoteObject))
        return static_cast<void*>(const_cast< RemoteObject*>(this));
    return QObject::qt_metacast(_clname);
}

int GCF::RemoteObject::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: activated(); break;
        case 1: deactivated(); break;
        case 2: couldNotActivate(); break;
        case 3: methodInvocationResult((*reinterpret_cast< qint32(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 4: setPropertyResult((*reinterpret_cast< qint32(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 5: getPropertyResult((*reinterpret_cast< qint32(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QString(*)>(_a[4]))); break;
        case 6: propertiesCached(); break;
        case 7: makeConnectionResult((*reinterpret_cast< qint32(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< const QString(*)>(_a[3]))); break;
        case 8: sendQueuedMessages(); break;
        case 9: cacheProperties((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 10: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])),(*reinterpret_cast< const QVariant(*)>(_a[5])),(*reinterpret_cast< const QVariant(*)>(_a[6])),(*reinterpret_cast< const QVariant(*)>(_a[7])),(*reinterpret_cast< const QVariant(*)>(_a[8])),(*reinterpret_cast< const QVariant(*)>(_a[9])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 11: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])),(*reinterpret_cast< const QVariant(*)>(_a[5])),(*reinterpret_cast< const QVariant(*)>(_a[6])),(*reinterpret_cast< const QVariant(*)>(_a[7])),(*reinterpret_cast< const QVariant(*)>(_a[8])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 12: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])),(*reinterpret_cast< const QVariant(*)>(_a[5])),(*reinterpret_cast< const QVariant(*)>(_a[6])),(*reinterpret_cast< const QVariant(*)>(_a[7])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 13: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])),(*reinterpret_cast< const QVariant(*)>(_a[5])),(*reinterpret_cast< const QVariant(*)>(_a[6])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 14: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])),(*reinterpret_cast< const QVariant(*)>(_a[5])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 15: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])),(*reinterpret_cast< const QVariant(*)>(_a[4])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 16: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])),(*reinterpret_cast< const QVariant(*)>(_a[3])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 17: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 18: { qint32 _r = call((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 19: { qint32 _r = setObjectProperty((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QVariant(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< qint32*>(_a[0]) = _r; }  break;
        case 20: { QVariant _r = getObjectProperty((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        case 21: cacheProperties(); break;
        case 22: { QVariant _r = getCachedObjectProperty((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QVariant*>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 23;
    }
    return _id;
}

// SIGNAL 0
void GCF::RemoteObject::activated()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void GCF::RemoteObject::deactivated()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}

// SIGNAL 2
void GCF::RemoteObject::couldNotActivate()
{
    QMetaObject::activate(this, &staticMetaObject, 2, 0);
}

// SIGNAL 3
void GCF::RemoteObject::methodInvocationResult(qint32 _t1, bool _t2, const QVariant & _t3, const QString & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}

// SIGNAL 4
void GCF::RemoteObject::setPropertyResult(qint32 _t1, bool _t2, const QVariant & _t3, const QString & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 4, _a);
}

// SIGNAL 5
void GCF::RemoteObject::getPropertyResult(qint32 _t1, bool _t2, const QVariant & _t3, const QString & _t4)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)) };
    QMetaObject::activate(this, &staticMetaObject, 5, _a);
}

// SIGNAL 6
void GCF::RemoteObject::propertiesCached()
{
    QMetaObject::activate(this, &staticMetaObject, 6, 0);
}

// SIGNAL 7
void GCF::RemoteObject::makeConnectionResult(qint32 _t1, bool _t2, const QString & _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 7, _a);
}
static const uint qt_meta_data_GCF__SignalSpy2[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_GCF__SignalSpy2[] = {
    "GCF::SignalSpy2\0"
};

const QMetaObject GCF::SignalSpy2::staticMetaObject = {
    { &GenericSignalSpy::staticMetaObject, qt_meta_stringdata_GCF__SignalSpy2,
      qt_meta_data_GCF__SignalSpy2, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::SignalSpy2::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::SignalSpy2::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::SignalSpy2::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__SignalSpy2))
        return static_cast<void*>(const_cast< SignalSpy2*>(this));
    return GenericSignalSpy::qt_metacast(_clname);
}

int GCF::SignalSpy2::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = GenericSignalSpy::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
