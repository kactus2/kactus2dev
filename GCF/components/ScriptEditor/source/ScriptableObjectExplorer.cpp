/****************************************************************************
**
** Copyright (C) VCreate Logic Private Limited, Bangalore
**
** Use of this file is limited according to the terms specified by
** VCreate Logic Private Limited, Bangalore.  Details of those terms
** are listed in licence.txt included as part of the distribution package
** of this file. This file may not be distributed without including the
** licence.txt file.
**
** Contact info@vcreatelogic.com if any conditions of this licensing are
** not clear to you.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include "ScriptableObjectExplorer.h"
#include "ScriptEngineManager.h"
#include <QHeaderView>
#include <QMetaObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QTreeWidgetItemIterator>
#include <QApplication>
#include <QStyle>

Q_DECLARE_METATYPE(QMetaMethod)
Q_DECLARE_METATYPE(QMetaProperty)

#define EVENT_ITEM 10001
#define PROPERTY_ITEM 10002
#define METHOD_ITEM 10003
#define OBJECT_ITEM 10004


struct GCF::Components::ScriptableObjectExplorerData
{
    ScriptableObjectExplorerData() : scriptEngineManager(0) { }

    GCF::Components::ScriptEngineManager* scriptEngineManager;
    QList<QWidget*> monitoredWidgets;
    QList<QTreeWidgetItem*> objectItems;
    QMap<QObject*, QTreeWidgetItem*> objectItemMap; // Maybe we can skip objectItems then???

    QPixmap methodPm;
    QPixmap propertyPm;
    QPixmap eventPm;
    QPixmap objectPm;

    QString formattedName(QString name);

    QTreeWidgetItem* loadObjectInfo(QTreeWidget* treeWidget, QTreeWidgetItem* item, QObject* object, QString name=QString(), bool showChildren=false);
    void updateItemPixmap(int itemType, const QPixmap& pm);
};

GCF::Components::ScriptableObjectExplorer::ScriptableObjectExplorer(QWidget* parent)
:QTreeWidget(parent)
{
    d = new GCF::Components::ScriptableObjectExplorerData;
    setColumnCount(1);
    header()->hide();
    setSelectionMode(SingleSelection);

    d->methodPm = QPixmap(":/ScriptEditor/method.png");
    d->propertyPm = QPixmap(":/ScriptEditor/property.png");
    d->eventPm = QPixmap(":/ScriptEditor/event.png");
    d->objectPm = QPixmap(":/ScriptEditor/object.png");

    qApp->installEventFilter(this);
    connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)),
            this, SLOT(on_itemDoubleClicked(QTreeWidgetItem*)));


	header()->setResizeMode(QHeaderView::ResizeToContents);
	setTextElideMode(Qt::ElideNone);
}

GCF::Components::ScriptableObjectExplorer::~ScriptableObjectExplorer()
{
    delete d;
}

void GCF::Components::ScriptableObjectExplorer::setScriptEngineManager(GCF::Components::ScriptEngineManager* manager)
{
    d->scriptEngineManager = manager;
    updateExplorer();
}

GCF::Components::ScriptEngineManager* GCF::Components::ScriptableObjectExplorer::scriptEngineManager() const
{
    return d->scriptEngineManager;
}

QObject* GCF::Components::ScriptableObjectExplorer::containerObject()
{
    return this;
}

IScriptEngineManager* GCF::Components::ScriptableObjectExplorer::scriptEngine()
{
    return d->scriptEngineManager;
}

void GCF::Components::ScriptableObjectExplorer::updateExplorer()
{
    if(!d->scriptEngineManager)
        return;

    // Fetch a list of all top-level objects that have currently been added
    // to the object item map.
    QList<QObject*> oldObjects = d->objectItemMap.keys();

    // Now loop through all objects in the script engine manager and add
    // items only for objects that are not present.
    for(int i=0; i<d->scriptEngineManager->scriptableObjectCount(); i++)
    {
        QObject* object = d->scriptEngineManager->scriptableObject(i);
        QString name = d->scriptEngineManager->scriptableObjectName(i);
        if(oldObjects.contains(object))
        {
            QTreeWidgetItem* objItem = d->objectItemMap[object];
            objItem->setText(0, name);
            objItem->setText(1, name);
            oldObjects.removeAll(object);
            continue;
        }

        bool ec = d->scriptEngineManager->scriptableObjectExcludeChildren(i);
        QTreeWidgetItem* objItem = d->loadObjectInfo(this, 0, object, name, !ec);
        d->objectItemMap[object] = objItem;
    }

    // Now oldObjects is a list of objects that are no longer needed.
    for(int i=0; i<oldObjects.count(); i++)
    {
        QObject* obj = oldObjects[i];
        QTreeWidgetItem* item = d->objectItemMap[obj];

        // Look for child tree widget items of item that may be present in
        // d->objectItems list.
        QTreeWidgetItemIterator it(item);
        while(*it)
        {
            d->objectItems.removeAll(*it);
            ++it;
        }
        delete item;
        d->objectItemMap.remove(obj);
    }

	resizeColumnToContents(0);

    // All done.
}

void GCF::Components::ScriptableObjectExplorer::monitorEvents(QWidget* widget, bool value)
{
    if(value && d->monitoredWidgets.contains(widget))
        return;

    if(!value && !d->monitoredWidgets.contains(widget))
        return;

    if(value)
        d->monitoredWidgets.append(widget);
    else
        d->monitoredWidgets.removeAll(widget);
}

void GCF::Components::ScriptableObjectExplorer::highlightScriptableObject(QObject* object)
{
    if(d->objectItemMap.contains(object))
    {
        QTreeWidgetItem* item = d->objectItemMap[object];
        scrollToItem(item);
        clearSelection();
        item->setSelected(true);
        return;
    }

    for(int i=0; i<d->objectItems.count(); i++)
    {
        QTreeWidgetItem* item = d->objectItems[i];
        if(item->data(0, Qt::UserRole).value<QObject*>() == object)
        {
            scrollToItem(item);
            clearSelection();
            item->setSelected(true);
            return;
        }
    }
}

void GCF::Components::ScriptableObjectExplorer::on_itemDoubleClicked(QTreeWidgetItem* item)
{
    if(!item)
        return;

    int type = item->type();
    QTreeWidgetItem* objItem = item;
    QObject* object = 0;
    QString objectName;

    while(objItem && objItem->type() != OBJECT_ITEM)
        objItem = objItem->parent();

    if(!objItem)
        return;

    object = objItem->data(0, Qt::UserRole).value<QObject*>();
    objectName = objItem->text(1);

    switch(type)
    {
    case OBJECT_ITEM:
        emit objectDoubleClicked(object, objectName);
        break;
    case METHOD_ITEM: {
        QMetaMethod m = item->data(0, Qt::UserRole).value<QMetaMethod>();
        emit methodDoubleClicked(object, objectName, item->text(0), QString(m.signature()) );
        } break;
    case EVENT_ITEM:  {
        QMetaMethod m = item->data(0, Qt::UserRole).value<QMetaMethod>();
        emit signalDoubleClicked(object, objectName, item->text(0), QString(m.signature()) );
        } break;
    case PROPERTY_ITEM:{
        QMetaProperty prop = item->data(0, Qt::UserRole).value<QMetaProperty>();
        emit propertyDoubleClicked(object, objectName, item->text(0), QString(prop.typeName()));
        } break;
    }
}

bool GCF::Components::ScriptableObjectExplorer::eventFilter(QObject* object, QEvent* event)
{
    if(!object->isWidgetType())
        return false;

    if(event->type() != QEvent::MouseButtonPress)
        return false;

    if(!d->monitoredWidgets.count())
        return false;

    QWidget* widget = dynamic_cast<QWidget*>(object);
    QWidget* widget2 = widget;
    while(widget2)
    {
        if(d->monitoredWidgets.contains(widget2))
        {
            highlightScriptableObject(widget);
            break;
        }
        widget2 = widget2->parentWidget();
    }

    return false;
}

QString GCF::Components::ScriptableObjectExplorerData::formattedName(QString name)
{
    QString ret = name;
    ret[0] = ret[0].toUpper();
    ret = ret.section('(', 0, 0);
    return ret;
}

QTreeWidgetItem* GCF::Components::ScriptableObjectExplorerData::loadObjectInfo(QTreeWidget* treeWidget, QTreeWidgetItem* item, QObject* object, QString name, bool showChildren)
{
    if(!object)
        return 0;

    if(!treeWidget && !item)
        return 0;

    static QColor eventColor = Qt::blue;
    static QColor propertyColor = Qt::red;
    static QColor methodColor = Qt::darkYellow;

    if(name.isEmpty())
        name = object->objectName();
    if(name.isEmpty())
        return 0;

    // add the object item
    QTreeWidgetItem* objectItem = 0;
    if(item)
        objectItem = new QTreeWidgetItem(item, OBJECT_ITEM);
    else
        objectItem = new QTreeWidgetItem(treeWidget, OBJECT_ITEM);
    objectItem->setText(0, name);
    objectItem->setData(0, Qt::UserRole, qVariantFromValue<QObject*>(object));
    objectItem->setText(1, name);
    objectItem->setIcon(0, objectPm);
    objectItems.append(objectItem);

    QTreeWidgetItem* eventItem = new QTreeWidgetItem(objectItem, QStringList() << "Events");
    QTreeWidgetItem* methodItem = new QTreeWidgetItem(objectItem, QStringList() << "Methods");
    QTreeWidgetItem* propertyItem = new QTreeWidgetItem(objectItem, QStringList() << "Properties");
    QTreeWidgetItem* childrenItem = new QTreeWidgetItem(objectItem, QStringList() << "Children");

    eventItem->setIcon(0, treeWidget->style()->standardIcon(QStyle::SP_DirIcon));
    methodItem->setIcon(0, treeWidget->style()->standardIcon(QStyle::SP_DirIcon));
    propertyItem->setIcon(0, treeWidget->style()->standardIcon(QStyle::SP_DirIcon));
    childrenItem->setIcon(0, treeWidget->style()->standardIcon(QStyle::SP_DirIcon));

    // add items.
    const QMetaObject* mo = object->metaObject();
    for(int j=QObject::staticMetaObject.methodCount(); j<mo->methodCount(); j++)
    {
        QMetaMethod m = mo->method(j);
        QTreeWidgetItem* item =  0;

        switch(m.methodType())
        {
        case QMetaMethod::Signal: {
            item = new QTreeWidgetItem(eventItem, EVENT_ITEM);
            item->setIcon(0, eventPm);
            QString event = formattedName(m.signature());
            item->setText(0, event);
            item->setData(0, Qt::ForegroundRole, eventColor);
            } break;
        case QMetaMethod::Method:
        case QMetaMethod::Slot:
        default: {
            if(m.access() != QMetaMethod::Public)
                break;
            item = new QTreeWidgetItem(methodItem, METHOD_ITEM);
            item->setIcon(0, methodPm);
            QString methodName = QString(m.signature()).section('(', 0, 0);
            item->setText(0, methodName);
            item->setData(0, Qt::ForegroundRole, methodColor);
            } break;
        }

        if(item)
            item->setData(0, Qt::UserRole, qVariantFromValue<QMetaMethod>(m));
    }

    for(int j=QObject::staticMetaObject.propertyCount(); j<mo->propertyCount(); j++)
    {
        QMetaProperty prop = mo->property(j);
        QTreeWidgetItem* item = new QTreeWidgetItem(propertyItem, PROPERTY_ITEM);
        item->setIcon(0, propertyPm);
        item->setData(0, Qt::UserRole, qVariantFromValue<QMetaProperty>(prop));
        item->setData(0, Qt::ForegroundRole, propertyColor);
        item->setText(0, prop.name());
    }

    if(showChildren)
    {
        QObjectList children = object->children();
        for(int i=0; i<children.count(); i++)
        {
            QObject* childObject = children[i];
            QString childName = QString("%1.%2").arg(name).arg(childObject->objectName());
            loadObjectInfo(treeWidget, childrenItem, childObject, childName, showChildren);
        }
    }

    objectItem->setExpanded(true);

    if(eventItem->childCount())
        eventItem->setExpanded(true);
    else
        delete eventItem;

    if(methodItem->childCount())
        methodItem->setExpanded(false);
    else
        delete methodItem;

    if(propertyItem->childCount())
        propertyItem->setExpanded(false);
    else
        delete propertyItem;

    if(childrenItem->childCount())
        childrenItem->setExpanded(true);
    else
        delete childrenItem;

    return objectItem;
}

