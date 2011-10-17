/****************************************************************************
** Meta object code from reading C++ file 'AppAccessPointComponent.h'
**
** Created: Wed 12. Oct 15:35:38 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/AppAccessPointComponent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'AppAccessPointComponent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__AppAccessPointComponent[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // methods: signature, parameters, type, tag, flags
      54,   39,   30,   29, 0x02,
     109,   88,   30,   29, 0x02,
     154,  143,   30,   29, 0x02,

       0        // eod
};

static const char qt_meta_stringdata_GCF__AppAccessPointComponent[] = {
    "GCF::AppAccessPointComponent\0\0QObject*\0"
    "ipAddress,port\0getRemoteApplication(QString,int)\0"
    "remoteApp,objectName\0"
    "getRemoteObject(QObject*,QString)\0"
    "objectName\0getLocalObject(QString)\0"
};

const QMetaObject GCF::AppAccessPointComponent::staticMetaObject = {
    { &AbstractComponent::staticMetaObject, qt_meta_stringdata_GCF__AppAccessPointComponent,
      qt_meta_data_GCF__AppAccessPointComponent, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::AppAccessPointComponent::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::AppAccessPointComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::AppAccessPointComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__AppAccessPointComponent))
        return static_cast<void*>(const_cast< AppAccessPointComponent*>(this));
    if (!strcmp(_clname, "IConfigurationDialogPageProvider"))
        return static_cast< IConfigurationDialogPageProvider*>(const_cast< AppAccessPointComponent*>(this));
    if (!strcmp(_clname, "com.vcreatelogic.IConfigurationDialogPageProvider/1.0"))
        return static_cast< IConfigurationDialogPageProvider*>(const_cast< AppAccessPointComponent*>(this));
    return AbstractComponent::qt_metacast(_clname);
}

int GCF::AppAccessPointComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractComponent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: { QObject* _r = getRemoteApplication((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        case 1: { QObject* _r = getRemoteObject((*reinterpret_cast< QObject*(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        case 2: { QObject* _r = getLocalObject((*reinterpret_cast< const QString(*)>(_a[1])));
            if (_a[0]) *reinterpret_cast< QObject**>(_a[0]) = _r; }  break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
static const uint qt_meta_data_GCF__AppAccessPointConfigPage[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      31,   30,   30,   30, 0x09,
      59,   30,   30,   30, 0x09,
      90,   30,   30,   30, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__AppAccessPointConfigPage[] = {
    "GCF::AppAccessPointConfigPage\0\0"
    "on_cmdMeToAppsAdd_clicked()\0"
    "on_cmdMeToAppsRemove_clicked()\0"
    "on_cmdAppsToMeRemove_clicked()\0"
};

const QMetaObject GCF::AppAccessPointConfigPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GCF__AppAccessPointConfigPage,
      qt_meta_data_GCF__AppAccessPointConfigPage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::AppAccessPointConfigPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::AppAccessPointConfigPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::AppAccessPointConfigPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__AppAccessPointConfigPage))
        return static_cast<void*>(const_cast< AppAccessPointConfigPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int GCF::AppAccessPointConfigPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_cmdMeToAppsAdd_clicked(); break;
        case 1: on_cmdMeToAppsRemove_clicked(); break;
        case 2: on_cmdAppsToMeRemove_clicked(); break;
        default: ;
        }
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
