//-----------------------------------------------------------------------------
// File: MultipleParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 29.01.2015
//
// Description:
// Finds parameters from multiple components.
//-----------------------------------------------------------------------------

#include "MultipleParameterFinder.h"

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::MultipleParameterFinder()
//-----------------------------------------------------------------------------
MultipleParameterFinder::MultipleParameterFinder(QSharedPointer<Component> component):
    oldComponent_(component),
    newComponent_(component)
{

}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::MultipleParameterFinder()
//-----------------------------------------------------------------------------
MultipleParameterFinder::~MultipleParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getParameterWithID()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> MultipleParameterFinder::getParameterWithID(QString const& parameterId) const
{
    QList <QSharedPointer<Component> > allComponents;
    allComponents.append(oldComponent_);
    allComponents.append(newComponent_);

    foreach (QSharedPointer<Component> component, allComponents)
    {
        foreach (QSharedPointer<Parameter> parameter, component->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<ModelParameter> modelParameter, component->getModelParameters())
        {
            if (modelParameter->getValueId() == parameterId)
            {
                return modelParameter;
            }
        }

        foreach (QSharedPointer<View> view, component->getViews())
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
// Function: MultipleParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool MultipleParameterFinder::hasId(QString const& id) const
{
    QList <QSharedPointer<Component> > allComponents;
    allComponents.append(oldComponent_);
    allComponents.append(newComponent_);
    
    foreach (QSharedPointer<Component> component, allComponents)
    {
        foreach (QSharedPointer<Parameter> parameter, component->getParameters())
        {
            if (parameter->getValueId() == id)
            {
                return true;
            }
        }
        
        foreach (QSharedPointer<ModelParameter> componentModelParameter, component->getModelParameters())
        {
            if (componentModelParameter->getValueId() == id)
            {
                return true;
            }
        }
        
        foreach (QSharedPointer<View> view, component->getViews())
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
// Function: MultipleParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getName();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString MultipleParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    return targetParameter->getValue();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList MultipleParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    QList <QSharedPointer<Component> > allComponents;
    allComponents.append(oldComponent_);
    allComponents.append(newComponent_);
    
    foreach (QSharedPointer<Component> component, allComponents)
    {
        foreach (QSharedPointer<Parameter> parameter, component->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
        foreach (QSharedPointer<ModelParameter> modelParameter, component->getModelParameters())
        {
            allParameterIds.append(modelParameter->getValueId());
        }
        foreach (QSharedPointer<View> view, component->getViews())
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
// Function: MultipleParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int MultipleParameterFinder::getNumberOfParameters() const
{
    QStringList allParameterIds = getAllParameterIds();

    return allParameterIds.size();
}

//-----------------------------------------------------------------------------
// Function: MultipleParameterFinder::setComponent()
//-----------------------------------------------------------------------------
void MultipleParameterFinder::setComponent(QSharedPointer<Component> component)
{
    newComponent_ = component;
}
