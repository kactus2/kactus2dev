//-----------------------------------------------------------------------------
// File: TopComponentParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 18.03.2015
//
// Description:
// Parameter finder for design top component.
//-----------------------------------------------------------------------------

#include "TopComponentParameterFinder.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ModuleParameter.h>

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::TopComponentParameterFinder()
//-----------------------------------------------------------------------------
TopComponentParameterFinder::TopComponentParameterFinder(QSharedPointer<Component> component):
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::TopComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> TopComponentParameterFinder::getParameterWithID(QStringView parameterId) const
{
    if (!component_.isNull())
    {
        for (QSharedPointer<Parameter> parameter : *component_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        for (QSharedPointer<Parameter> viewParameter : activeViewParameters())
        {
            if (viewParameter->getValueId() == parameterId)
            {
                return viewParameter;
            }
        }
    }

    return QSharedPointer<Parameter>(new Parameter());
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool TopComponentParameterFinder::hasId(QStringView id) const
{
    if (!component_.isNull())
    {
        for (QSharedPointer<Parameter> parameter : *component_->getParameters())
        {
            if (parameter->getValueId() == id)
            {
                return true;
            }
        }

        for (QSharedPointer<Parameter> viewParameter : activeViewParameters())
        {
            if (viewParameter->getValueId() == id)
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString TopComponentParameterFinder::nameForId(QStringView id) const
{
    return  getParameterWithID(id)->name();
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString TopComponentParameterFinder::valueForId(QStringView id) const
{
    return  getParameterWithID(id)->getValue();
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList TopComponentParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (!component_.isNull())
    {
        for (QSharedPointer<Parameter> parameter : *component_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        for (QSharedPointer<Parameter> parameter : activeViewParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int TopComponentParameterFinder::getNumberOfParameters() const
{
    return getAllParameterIds().size();
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void TopComponentParameterFinder::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::setActiveView()
//-----------------------------------------------------------------------------
void TopComponentParameterFinder::setActiveView(QSharedPointer<View> view)
{
    activeView_ = view;
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void TopComponentParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::activeViewParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > TopComponentParameterFinder::activeViewParameters() const
{
    QList<QSharedPointer<Parameter> > viewParameters;

    if (!activeView_)
    {
        return viewParameters;
    }

    QSharedPointer<ComponentInstantiation> instantiation = component_->getModel()->
        findComponentInstantiation(activeView_->getComponentInstantiationRef());

    if (!instantiation)
    {
        return viewParameters;
    }

    viewParameters.append(*instantiation->getParameters());

    for (QSharedPointer<ModuleParameter> parameter : *instantiation->getModuleParameters())
    {
        viewParameters.append(parameter);
    }

    return viewParameters;
}
