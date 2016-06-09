//-----------------------------------------------------------------------------
// File: TopComponentParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
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
component_(component), activeView_()
{

}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::~TopComponentParameterFinder()
//-----------------------------------------------------------------------------
TopComponentParameterFinder::~TopComponentParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::TopComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> TopComponentParameterFinder::getParameterWithID(QString const& parameterId) const
{
    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<Parameter> viewParameter, activeViewParameters())
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
bool TopComponentParameterFinder::hasId(QString const& id) const
{
    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            if (parameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<Parameter> viewParameter, activeViewParameters())
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
QString TopComponentParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);
    return targetParameter->name();
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString TopComponentParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);
    return targetParameter->getValue();
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList TopComponentParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, *component_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> parameter, activeViewParameters())
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
    QStringList allParameterIds = getAllParameterIds();
    return allParameterIds.size();
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
void TopComponentParameterFinder::setActiveView(QString const& view)
{
    activeView_ = view;
}

//-----------------------------------------------------------------------------
// Function: TopComponentParameterFinder::activeViewParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > TopComponentParameterFinder::activeViewParameters() const
{
    QList<QSharedPointer<Parameter> > viewParameters;
    
    foreach (QSharedPointer<View> view, *component_->getViews())
    {
        if (view->name() == activeView_)
        {
            if (!view->getComponentInstantiationRef().isEmpty())
            {
                foreach (QSharedPointer<ComponentInstantiation> instantiation,
                    *component_->getComponentInstantiations())
                {
                    if (instantiation->name() == view->getComponentInstantiationRef())
                    {
                        viewParameters.append(*instantiation->getParameters());

                        foreach (QSharedPointer<ModuleParameter> parameter, *instantiation->getModuleParameters())
                        {
                            viewParameters.append(parameter);
                        }

                        break;
                    }
                }
            }
            break;
        }
    }

    return viewParameters;
}
