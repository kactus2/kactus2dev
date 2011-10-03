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

#include <GCF/ComponentFactory.h>
#include <GCF/AbstractComponent.h>
#include <QMetaObject>
#include <QApplication>

/**
\class GCF::ComponentFactory GCF::ComponentFactory.h
\brief Factory class for all components

The GCF::ComponentFactory class, as the name suggests, is a factory class for components.
Components must be created by making use of the GCF::ComponentFactory class only. This
allows the CF to safely manage the lifetime of components and ensure appropriate
behavior by components.

\note Anytime a new component should be made available to the factory, the component
must be inserted into the factory table in ComponentFactoryTable.
*/


namespace GCF
{
    struct ComponentInfo
    {
        QString className;
        CreateComponentFunction createFn;
        const QMetaObject* metaObject;
    };
}

struct GCF::ComponentFactoryData
{
    ComponentFactoryData() : initialized(false) { }

    bool initialized;
    QList<GCF::AbstractComponent*> componentList;
    QList<GCF::ComponentInfo> componentInfoList;
};

/**
Since GCF::ComponentFactory is a singleton class, the one and only instance of GCF::ComponentFactory
can be fetched using the Instance() static method.

As with all singleton classes, the constructor of GCF::ComponentFactory class is declared protected
and the destructor is declared public.
*/
GCF::ComponentFactory & GCF::ComponentFactory::instance()
{
    static GCF::ComponentFactory theFactory;
    theFactory.initialize();
    return theFactory;
}

/**
Protected Constructor
*/
GCF::ComponentFactory::ComponentFactory()
{
    d = new GCF::ComponentFactoryData;
    connect(qApp, SIGNAL(lastWindowClosed()), this, SLOT(on_qApp_lastWindowClosed()));
}

/**
Public Destructor
*/
GCF::ComponentFactory::~ComponentFactory()
{
    if(d->componentList.count())
        qWarning("Deleting GCF::ComponentFactory before calling finalize()");
    delete d;
}

/**
The initialize() and finalize() methods perform the initialization and finalization
of the Component Factory. The initialize method basically initializes the internal
structures used to manage components. The finalize() deletes all components in the
system and eventually destroys itself.

\note Normally you would never have to use these functions.
*/
void GCF::ComponentFactory::initialize()
{
    if(d->initialized)
        return;
    registerComponentInfo();
    d->initialized = true;
}

/**
The initialize() and finalize() methods perform the initialization and finalization
of the Component Factory. The initialize method basically initializes the internal
structures used to manage components. The finalize() deletes all components in the
system and eventually destroys itself.

\note Normally you would never have to use these functions.
*/
void GCF::ComponentFactory::finalize()
{
    while(d->componentList.count())
        d->componentList.first()->finalize();
}

/**
Returns meta information about the component whose classname is passed as parameter
to this function.
*/
const QMetaObject* GCF::ComponentFactory::componentMeta(const QString& className) const
{
    for(int i=0; i<d->componentInfoList.count(); i++)
    {
        if(d->componentInfoList[i].className == className)
            return d->componentInfoList[i].metaObject;
    }
    return 0;
}

/**
This function returns true if the component factory can create an instance of the
component whose class name is passed as parameter to this function.
*/
bool GCF::ComponentFactory::canCreate(const QString& className) const
{
    for(int i=0; i<d->componentInfoList.count(); i++)
    {
        if(d->componentInfoList[i].className == className)
            return true;
    }
    return false;
}

/**
The create component method creates an instance of a GCF::AbstractComponent subclass whose
class name is specified as parameter. The function prints out a warning message if
more than one instance of that class is attempted for creation.

\note The returned component will be initialized, after this function returns,
as soon as possible
*/
GCF::AbstractComponent* GCF::ComponentFactory::createComponent(const QString& className)
{
    // Now create the new component.
    for(int i=0; i<d->componentInfoList.count(); i++)
    {
        if(d->componentInfoList[i].className == className)
        {
            GCF::AbstractComponent* component = d->componentInfoList[i].createFn();
            if(component)
                component->setParent(this);
            QMetaObject::invokeMethod(component, "initialize", Qt::QueuedConnection);
            return component;
        }
    }
    return 0;
}

/**
These methods help discover the components created in the system. The findComponent()
method returns a pointer to a component whose component name is passed as parameter.
If no such component was found a NULL pointer is returned.

If you know that multiple instances of a component was created, then you can fetch a
list of all instances of a component using the findComponents() method.

The components() method simply returns a list of all components in the system.
*/
GCF::AbstractComponent* GCF::ComponentFactory::findComponent(const QString& componentName) const
{
    for(int i=0; i<d->componentList.count(); i++)
    {
        if(d->componentList[i]->componentName() == componentName || d->componentList[i]->inherits(qPrintable(componentName)))
            return d->componentList[i];
    }
    return 0;
}

/**
These methods help discover the components created in the system. The findComponent()
method returns a pointer to a component whose component name is passed as parameter.
If no such component was found a NULL pointer is returned.

If you know that multiple instances of a component was created, then you can fetch a
list of all instances of a component using the findComponents() method.

The components() method simply returns a list of all components in the system.
*/
QList<GCF::AbstractComponent*> GCF::ComponentFactory::findComponents(const QString& componentName) const
{
    QList<GCF::AbstractComponent*> ret;
    for(int i=0; i<d->componentList.count(); i++)
    {
        if(d->componentList[i]->componentName() == componentName || d->componentList[i]->inherits(qPrintable(componentName)))
            ret.append(d->componentList[i]);
    }
    return ret;
}

/**
These methods help discover the components created in the system. The findComponent()
method returns a pointer to a component whose class name is passed as parameter.
If no such component was found a NULL pointer is returned.

If you know that multiple instances of a component was created, then you can fetch a
list of all instances of a component using the findComponents() method.

The components() method simply returns a list of all components in the system.

The componentNames() method returns a list of names of all components.
*/
QList<GCF::AbstractComponent*> GCF::ComponentFactory::components() const
{
    return d->componentList;
}

/**
These methods help discover the components created in the system. The findComponent()
method returns a pointer to a component whose class name is passed as parameter.
If no such component was found a NULL pointer is returned.

If you know that multiple instances of a component was created, then you can fetch a
list of all instances of a component using the findComponents() method.

The components() method simply returns a list of all components in the system.

The componentNames() method returns a list of names of all components.
*/
QStringList GCF::ComponentFactory::componentNames() const
{
    QStringList ret;
    for(int i=0; i<d->componentList.count(); i++)
        ret.append(d->componentList[i]->metaObject()->className());
    return ret;
}

/**
These functions are used to fetch a list of active and inactive components.
*/
QList<GCF::AbstractComponent*> GCF::ComponentFactory::activeComponents() const
{
    QList<GCF::AbstractComponent*> ret;
    for(int i=0; i<d->componentList.count(); i++)
    {
        GCF::AbstractComponent* comp = d->componentList[i];
        if(comp->isActive())
            ret.append(comp);
    }
    return ret;
}

/**
These functions are used to fetch a list of active and inactive components.
*/
QList<GCF::AbstractComponent*> GCF::ComponentFactory::inactiveComponents() const
{
    QList<GCF::AbstractComponent*> ret;
    for(int i=0; i<d->componentList.count(); i++)
    {
        GCF::AbstractComponent* comp = d->componentList[i];
        if(!comp->isActive())
            ret.append(comp);
    }
    return ret;
}

/**
\internal
*/
void GCF::ComponentFactory::registerComponent(GCF::AbstractComponent* comp)
{
    d->componentList.append(comp);
}

/**
\internal
*/
void GCF::ComponentFactory::unregisterComponent(GCF::AbstractComponent* comp)
{
    d->componentList.removeAll(comp);
}

/**
\internal
*/
void GCF::ComponentFactory::announceComponentCreation(GCF::AbstractComponent* comp)
{
    for(int i=0; i<d->componentList.count(); i++)
    {
        GCF::AbstractComponent* component = d->componentList[i];
        if(component == comp)
            continue;
        component->componentCreated(comp);
    }
}

/**
\internal
*/
void GCF::ComponentFactory::announceComponentDestruction(GCF::AbstractComponent* comp)
{
    for(int i=0; i<d->componentList.count(); i++)
    {
        GCF::AbstractComponent* component = d->componentList[i];
        if(component == comp)
            continue;
        component->componentDestroyed(comp);
    }
}

/**
\internal
*/
void GCF::ComponentFactory::on_qApp_lastWindowClosed()
{
    finalize();
}

void GCF::ComponentFactory::registerComponentInfo(const QString& className, CreateComponentFunction createFn, const QMetaObject* meta)
{
    for(int i=0; i<d->componentInfoList.count(); i++)
    {
        GCF::ComponentInfo info = d->componentInfoList[i];
        if(info.className == className)
        {
            info.createFn = createFn;
            info.metaObject = meta;
            return;
        }
    }

    GCF::ComponentInfo info;
    info.className = className;
    info.createFn = createFn;
    info.metaObject = meta;

    d->componentInfoList.push_back(info);
}

/****************************************************************************
Component Factory Table
****************************************************************************/

#define GCF_COMPONENT_FACTORY_TABLE_BEGIN void GCF::ComponentFactory::registerComponentInfo() {

#define GCF_COMPONENT_FACTORY_TABLE_ENTRY(ClassName) \
    { \
        GCF::ComponentInfo info; \
        info.className = #ClassName; \
        info.createFn = ClassName::CreateInstance; \
        info.metaObject = &ClassName::staticMetaObject; \
        d->componentInfoList.push_back(info); \
    }

#define GCF_COMPONENT_FACTORY_TABLE_END }

#include "ComponentFactoryTable"

