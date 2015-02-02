//-----------------------------------------------------------------------------
// File: ComponentParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2015
//
// Description:
// The interface for finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "ComponentParameterFinder.h"

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::ComponentParameterFinder(QSharedPointer<Component> component):
component_(component)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::~ComponentParameterFinder()
//-----------------------------------------------------------------------------
ComponentParameterFinder::~ComponentParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::ComponentParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentParameterFinder::getParameterWithID(QString const& parameterId) const
{
    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, component_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<ModelParameter> modelParameter, component_->getModelParameters())
        {
            if (modelParameter->getValueId() == parameterId)
            {
                return modelParameter;
            }
        }

        foreach (QSharedPointer<View> view, component_->getViews())
        {
            foreach(QSharedPointer<Parameter> parameter, view->getParameters())
            {
                if (parameter->getValueId() == parameterId)
                {
                    return parameter;
                }
            }
        }
    }

    return QSharedPointer<Parameter>(new Parameter);
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentParameterFinder::hasId(QString const& id) const
{
    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, component_->getParameters())
        {
            if (parameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<ModelParameter> componentModelParameter, component_->getModelParameters())
        {
            if (componentModelParameter->getValueId() == id)
            {
                return true;
            }
        }

        foreach (QSharedPointer<View> view, component_->getViews())
        {
            foreach (QSharedPointer<Parameter> viewParameter, view->getParameters())
            {
                if (viewParameter->getValueId() == id)
                {
                    return true;
                }
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getName();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getValue();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::()
//-----------------------------------------------------------------------------
QStringList ComponentParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (!component_.isNull())
    {
        foreach (QSharedPointer<Parameter> parameter, component_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
        foreach (QSharedPointer<ModelParameter> modelParameter, component_->getModelParameters())
        {
            allParameterIds.append(modelParameter->getValueId());
        }
        foreach (QSharedPointer<View> view, component_->getViews())
        {
            foreach (QSharedPointer<Parameter> viewParameter, view->getParameters())
            {
                allParameterIds.append(viewParameter->getValueId());
            }
        }
    }
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ComponentParameterFinder::getNumberOfParameters() const
{
    QStringList allParameterIds = getAllParameterIds();

    return allParameterIds.size();
}

//-----------------------------------------------------------------------------
// Function: ComponentParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void ComponentParameterFinder::setComponent(QSharedPointer<Component> component)
{
    component_ = component;
}