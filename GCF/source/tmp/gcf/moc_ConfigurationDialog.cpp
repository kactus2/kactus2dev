/****************************************************************************
** Meta object code from reading C++ file 'ConfigurationDialog.h'
**
** Created: Mon 3. Oct 22:07:41 2011
**      by: The Qt Meta Object Compiler version 62 (Qt 4.7.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../include/GCF/ConfigurationDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'ConfigurationDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.7.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_GCF__ConfigurationDialog[] = {

 // content:
       5,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      32,   26,   25,   25, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__ConfigurationDialog[] = {
    "GCF::ConfigurationDialog\0\0index\0"
    "on_lstComponents_currentRowChanged(int)\0"
};

const QMetaObject GCF::ConfigurationDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_GCF__ConfigurationDialog,
      qt_meta_data_GCF__ConfigurationDialog, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::ConfigurationDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::ConfigurationDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::ConfigurationDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__ConfigurationDialog))
        return static_cast<void*>(const_cast< ConfigurationDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int GCF::ConfigurationDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_lstComponents_currentRowChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_GCF__ComponentPropertyEditorPage[] = {

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
      34,   33,   33,   33, 0x0a,
      51,   33,   33,   33, 0x0a,
      73,   68,   33,   33, 0x09,
     120,   33,   33,   33, 0x09,
     148,   33,   33,   33, 0x09,
     188,  179,   33,   33, 0x09,

       0        // eod
};

static const char qt_meta_stringdata_GCF__ComponentPropertyEditorPage[] = {
    "GCF::ComponentPropertyEditorPage\0\0"
    "loadProperties()\0saveProperties()\0"
    "item\0on_componentTree_itemClicked(QTreeWidgetItem*)\0"
    "on_cmdAddProperty_clicked()\0"
    "on_cmdRemoveProperty_clicked()\0item,col\0"
    "on_propertyEditor_itemChanged(QTreeWidgetItem*,int)\0"
};

const QMetaObject GCF::ComponentPropertyEditorPage::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_GCF__ComponentPropertyEditorPage,
      qt_meta_data_GCF__ComponentPropertyEditorPage, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &GCF::ComponentPropertyEditorPage::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *GCF::ComponentPropertyEditorPage::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *GCF::ComponentPropertyEditorPage::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_GCF__ComponentPropertyEditorPage))
        return static_cast<void*>(const_cast< ComponentPropertyEditorPage*>(this));
    return QWidget::qt_metacast(_clname);
}

int GCF::ComponentPropertyEditorPage::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: loadProperties(); break;
        case 1: saveProperties(); break;
        case 2: on_componentTree_itemClicked((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1]))); break;
        case 3: on_cmdAddProperty_clicked(); break;
        case 4: on_cmdRemoveProperty_clicked(); break;
        case 5: on_propertyEditor_itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
