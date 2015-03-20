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

#include <IPXACTmodels/component.h>

#include <IPXACTmodels/parameter.h>

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
            if (parameter->getValueId() == parameterId || parameter->getCopySources().contains(parameterId))
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<ModelParameter> modelParameter, *component_->getModelParameters())
        {
            if (modelParameter->getValueId() == parameterId || modelParameter->getCopySources().contains(parameterId))
            {
                return modelParameter;
            }
        }

        foreach (QSharedPointer<Parameter> viewParameter, activeViewParameters())
        {
            if (viewParameter->getValueId() == parameterId || viewParameter->getCopySources().contains(parameterId))
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
            if (parameter->getValueId() == id || parameter->getCopySources().contains(id))
            {
                return true;
            }
        }

        foreach (QSharedPointer<ModelParameter> modelParameter, *component_->getModelParameters())
        {
            if (modelParameter->getValueId() == id || modelParameter->getCopySources().contains(id))
            {
                return true;
            }
        }

        foreach (QSharedPointer<Parameter> viewParameter, activeViewParameters())
        {
            if (viewParameter->getValueId() == id || viewParameter->getCopySources().contains(id))
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
    return targetParameter->getName();
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
            allParameterIds.append(parameter->getCopySources());
        }

        foreach (QSharedPointer<ModelParameter> modelParameter, *component_->getModelParameters())
        {
            allParameterIds.append(modelParameter->getValueId());
            allParameterIds.append(modelParameter->getCopySources());
        }

        foreach (QSharedPointer<Parameter> parameter, activeViewParameters())
        {
            allParameterIds.append(parameter->getValueId());
            allParameterIds.append(parameter->getCopySources());
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

    View* activeView = component_->findView(activeView_);
    if (activeView)
    {
        QString implementationView = activeView_;
        if (activeView->isHierarchical())
        {
            implementationView = activeView->getTopLevelView();
        }

        foreach (QSharedPointer<View> view, component_->getViews())
        {
            if (view->getName() == implementationView)
            {
                viewParameters.append(*view->getParameters());
                foreach(QSharedPointer<ModelParameter> moduleParameter, *view->getModuleParameters())
                {
                    viewParameters.append(moduleParameter);
                }
            }
        }
    }
    
    return viewParameters;
}
