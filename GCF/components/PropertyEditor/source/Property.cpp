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

#include "Property.h"
#include "IValueEditorCreator.h"
#include "IEditableProperties.h"
#include "IPropertyEditorNotifications.h"

#include <QObject>
#include <QPointer>

struct GCF::Components::PropertyData
{
    QPointer<QObject> object;
    QString propName;
    IValueEditorCreator* valueEditorCreator;
    QMetaEnum enumStruct;
    IEditableProperties* editableProperties;
    int editablePropertyIndex;
    IPropertyEditorNotifications* editorNotifications;
};

GCF::Components::Property::Property(QObject* object, QString propName)
{
    d = new GCF::Components::PropertyData;
    d->valueEditorCreator = 0;
    d->editableProperties = 0;
    d->editablePropertyIndex = -1;
    d->object = object;
    d->propName = propName;
    d->editorNotifications = qobject_cast<IPropertyEditorNotifications*>(object);
}

GCF::Components::Property::~Property()
{
    delete d;
}

IPropertyEditorNotifications* GCF::Components::Property::editorNotifications()
{
    return d->editorNotifications;
}

void GCF::Components::Property::setValueEditorCreator(IValueEditorCreator* creator)
{
    d->valueEditorCreator = creator;
}

IValueEditorCreator* GCF::Components::Property::valueEditorCreator()
{
    return d->valueEditorCreator;
}

void GCF::Components::Property::setEnumStruct(QMetaEnum enumStruct)
{
    d->enumStruct = enumStruct;
}

bool GCF::Components::Property::isEnum()
{
    return d->enumStruct.isValid();
}

QMetaEnum GCF::Components::Property::enumStruct()
{
    return d->enumStruct;
}

void GCF::Components::Property::setEditableProperties(IEditableProperties* editProps)
{
    d->editableProperties = editProps;
}

void GCF::Components::Property::setEditbalePropertyIndex(int index)
{
    d->editablePropertyIndex = index;
}

QString GCF::Components::Property::propertyNameQString()
{
    return d->propName;
}

GCF_BEGIN_QUERY_TABLE(GCF::Components::Property)
    GCF_IMPLEMENTS(IProperty)
GCF_END_QUERY_TABLE(GCF::Components::Property)

QObject* GCF::Components::Property::propertyObject()
{
    return d->object;
}

const char* GCF::Components::Property::propertyName()
{
    return d->propName.toAscii().data();
}

QVariant GCF::Components::Property::propertyValue()
{
    if(d->object.isNull())
        return QVariant();

    if(d->editableProperties)
        return d->editableProperties->propertyValue(d->editablePropertyIndex);

    return d->object->property(d->propName.toAscii().data());
}

bool GCF::Components::Property::setPropertyValue(QVariant value)
{
    if(d->object.isNull())
        return false;

    if(d->editableProperties)
    {
        d->editableProperties->setPropertyValue(d->editablePropertyIndex, value);
        return true;
    }

    bool ret = d->object->setProperty(d->propName.toAscii().data(), value);
    if(ret)
        QMetaObject::invokeMethod(d->object, "propertyChanged", Qt::QueuedConnection, Q_ARG(QString, d->propName));
    return ret;
}

IEditableProperties* GCF::Components::Property::editableProperties()
{
    return d->editableProperties;
}

int GCF::Components::Property::editablePropertyIndex()
{
    return d->editablePropertyIndex;
}


