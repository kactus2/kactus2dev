/****************************************************************************
** Meta object code from reading C++ file 'ComponentLoaderComponent.h'
**
** Created: Mon 3. Oct 22:07:42 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/ComponentLoaderComponent.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ComponentLoaderComponent.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__ComponentLoaderComponent[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      39,   31,   30,   30, 0x05,

 // slots: signature, parameters, type, tag, flags
      74,   65,   30,   30, 0x0a,
     118,   97,   30,   30, 0x0a,
     154,  150,   30,   30, 0x0a,
     185,  150,   30,   30, 0x0a,
     219,   30,   30,   30, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_GCF__ComponentLoaderComponent[] = {
    "GCF::ComponentLoaderComponent\0\0message\0"
    "loadingComponent(QString)\0compName\0"
    "loadComponent(QString)\0compName,libraryFile\0"
    "loadComponent2(QString,QString)\0dir\0"
    "addComponentDirectory(QString)\0"
    "removeComponentDirectory(QString)\0"
    "registerComponents()\0"
};

const QMetaObject GCF::ComponentLoaderComponent::staticMetaObject = {
    { &AbstractComponent::staticMetaObject, qt_meta_stringdata_GCF__ComponentLoaderComponent,
      qt_meta_data_GCF__ComponentLoaderComponent, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::ComponentLoaderComponent::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::ComponentLoaderComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::ComponentLoaderComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__ComponentLoaderComponent))
        return static_cast<void*>(const_cast< ComponentLoaderComponent*>(this));
    return AbstractComponent::qt_metacast(_clname);
}

int GCF::ComponentLoaderComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractComponent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadingComponent((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: loadComponent((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: loadComponent2((*reinterpret_cast< const QString(*)>(_a[1])),(*reinterpret_cast< const QString(*)>(_a[2]))); break;
        case 3: addComponentDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: removeComponentDirectory((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 5: registerComponents(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void GCF::ComponentLoaderComponent::loadingComponent(const QString & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_GCF__ComponentLoaderConfigPage[] = {

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
      32,   31,   31,   31, 0x08,
      56,   31,   31,   31, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_GCF__ComponentLoaderConfigPage[] = {
    "GCF::ComponentLoaderConfigPage\0\0"
    "on_cmdAddPath_clicked()\0"
    "on_cmdRemovePath_clicked()\0"
};

const QMetaObject GCF::ComponentLoaderConfigPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GCF__ComponentLoaderConfigPage,
      qt_meta_data_GCF__ComponentLoaderConfigPage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::ComponentLoaderConfigPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::ComponentLoaderConfigPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::ComponentLoaderConfigPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__ComponentLoaderConfigPage))
        return static_cast<void*>(const_cast< ComponentLoaderConfigPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int GCF::ComponentLoaderConfigPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_cmdAddPath_clicked(); break;
        case 1: on_cmdRemovePath_clicked(); break;
        default: ;
        }
        _id -= 2;
    }
    return _id;
}
static const uint qt_meta_data_GCF__ComponentLoaderConfigComponent[] = {

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

static const char qt_meta_stringdata_GCF__ComponentLoaderConfigComponent[] = {
    "GCF::ComponentLoaderConfigComponent\0"
};

const QMetaObject GCF::ComponentLoaderConfigComponent::staticMetaObject = {
    { &AbstractComponent::staticMetaObject, qt_meta_stringdata_GCF__ComponentLoaderConfigComponent,
      qt_meta_data_GCF__ComponentLoaderConfigComponent, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::ComponentLoaderConfigComponent::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::ComponentLoaderConfigComponent::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::ComponentLoaderConfigComponent::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__ComponentLoaderConfigComponent))
        return static_cast<void*>(const_cast< ComponentLoaderConfigComponent*>(this));
    return AbstractComponent::qt_metacast(_clname);
}

int GCF::ComponentLoaderConfigComponent::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = AbstractComponent::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
QT_END_MOC_NAMESPACE
