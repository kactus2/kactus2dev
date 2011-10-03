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

#include "ValueEditorFactory.h"
#include "IValueEditorCreator.h"
#include "IEnumValueEditorCreator.h"
#include <QMap>

struct GCF::Components::ValueEditorFactoryData
{
    QMap<QString, IValueEditorCreator*> creatorMap;
    QList<IValueEditorCreator*> creatorList;
    IEnumValueEditorCreator* enumCreator;
};

GCF::Components::ValueEditorFactory::ValueEditorFactory()
{
    d = new GCF::Components::ValueEditorFactoryData;
    d->enumCreator = 0;
}

GCF::Components::ValueEditorFactory::~ValueEditorFactory()
{
    delete d;
}

GCF_BEGIN_QUERY_TABLE(GCF::Components::ValueEditorFactory)
    GCF_IMPLEMENTS(IValueEditorFactory);
GCF_END_QUERY_TABLE(GCF::Components::ValueEditorFactory)

void GCF::Components::ValueEditorFactory::setEnumCreator(IEnumValueEditorCreator* creator)
{
    d->enumCreator = creator;
}

IEnumValueEditorCreator* GCF::Components::ValueEditorFactory::enumCreator()
{
    return d->enumCreator;
}

void GCF::Components::ValueEditorFactory::registerCreator(IValueEditorCreator* creator)
{
    if(!creator)
        return;

    QString typeName(creator->typeName());
    if(d->creatorMap.contains(typeName))
    {
        IValueEditorCreator* oldCreator = d->creatorMap[typeName];
        d->creatorList.removeAll(oldCreator);
        d->creatorMap.remove(typeName);
        oldCreator->destroySelf();
    }

    d->creatorMap[typeName] = creator;
    d->creatorList.append(creator);
}

void GCF::Components::ValueEditorFactory::unregisterCreator(IValueEditorCreator* creator)
{
    if(!creator)
        return;

    QString typeName(creator->typeName());
    if(!d->creatorMap.contains(typeName))
        return; // Somthing wrong.

    if(!d->creatorList.contains(creator))
        return;

    IValueEditorCreator* oldCreator = d->creatorMap[typeName];
    if(oldCreator != creator)
        return; // Something terribly wrong.

    d->creatorList.removeAll(creator);
    d->creatorMap.remove(typeName);

    // Creator is not destroyed here.
    // creator->destroySelf();
}

int GCF::Components::ValueEditorFactory::creatorCount()
{
    return d->creatorList.count();
}

IValueEditorCreator* GCF::Components::ValueEditorFactory::creator(int index)
{
    if(index < 0 || index >= d->creatorList.count())
        return 0;

    return d->creatorList[index];
}

IValueEditorCreator* GCF::Components::ValueEditorFactory::creator(const char* typeName)
{
    QString typeNameStr(typeName);

    if(d->creatorMap.contains(typeName))
        return d->creatorMap[typeName];

    if( typeNameStr.startsWith("QList") )
        return d->creatorMap["QList<double>"];

    return 0;
}

QWidget* GCF::Components::ValueEditorFactory::createEditor(const char* typeName, QWidget* parent, IValueEditorCreator** creator)
{
    if(creator)
        *creator = 0;

    IValueEditorCreator* creator2 = this->creator(typeName);
    if(!creator2)
        return 0;

    if(creator)
        *creator = creator2;

    return creator2->createEditor(parent);
}

QWidget* GCF::Components::ValueEditorFactory::createEnumEditor(const char* typeName, QObject* object, QWidget* parent, IEnumValueEditorCreator** creator)
{
    if(creator)
        *creator = 0;

    if(!object || !typeName || !d->enumCreator)
        return 0;

    *creator = d->enumCreator;

    const QMetaObject* mo = object->metaObject();
    for(int i=0; i<mo->enumeratorCount(); i++)
    {
        QMetaEnum enumStruct = mo->enumerator(i);
        if(qstrcmp(enumStruct.name(), typeName))
            continue;

        QWidget* editor = d->enumCreator->createEditor(parent);
        d->enumCreator->initialize(editor, enumStruct);
        return editor;
    }

    return 0;
}

