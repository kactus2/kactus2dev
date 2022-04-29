//-----------------------------------------------------------------------------
// File: ComponentAndInstantiationsParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 02.04.2020
//
// Description:
// The implementation for finding parameters from the full component with the correct ID.
//-----------------------------------------------------------------------------

#include "ComponentAndInstantiationsParameterFinder.h"

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>

//-----------------------------------------------------------------------------
// Function: ComponentAndInstantiationsParameterFinder::ComponentAndInstantiationsParameterFinder()
//-----------------------------------------------------------------------------
ComponentAndInstantiationsParameterFinder::ComponentAndInstantiationsParameterFinder(
    QSharedPointer<Component const> component):
ComponentParameterFinder(component)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentAndInstantiationsParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ComponentAndInstantiationsParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds = ComponentParameterFinder::getAllParameterIds();

    for (auto instantiationParameter : allInstantiationsParameters())
    {
        allParameterIds.append(instantiationParameter->getValueId());
    }

    allParameterIds.removeAll("");
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ComponentAndInstantiationsParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ComponentAndInstantiationsParameterFinder::getNumberOfParameters() const
{
    int parameterCount = ComponentParameterFinder::getNumberOfParameters();
    parameterCount += allInstantiationsParameters().count();
    
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentAndInstantiationsParameterFinder::searchParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentAndInstantiationsParameterFinder::searchParameter(QString const& parameterId)
    const
{
    QSharedPointer<Parameter> parameter = ComponentParameterFinder::searchParameter(parameterId);
    if (parameter)
    {
        return parameter;
    }
    else
    {
        for (auto instantiationParameter : allInstantiationsParameters())
        {
            if (instantiationParameter->getValueId() == parameterId)
            {
                return instantiationParameter;
            }
        }
    }

    return QSharedPointer<Parameter>();
}

//-----------------------------------------------------------------------------
// Function: ComponentAndInstantiationsParameterFinder::allInstantiationsParameters()
//-----------------------------------------------------------------------------
QList<QSharedPointer<Parameter> > ComponentAndInstantiationsParameterFinder::allInstantiationsParameters() const
{
    QList<QSharedPointer<Parameter> > instantiationsParameters;
    for (auto instantiation : *getComponent()->getComponentInstantiations())
    {
        instantiationsParameters.append(*instantiation->getParameters());

        for (auto moduleParameter : *instantiation->getModuleParameters())
        {
            instantiationsParameters.append(moduleParameter);
        }
    }

    for (auto instantiation : *getComponent()->getDesignConfigurationInstantiations())
    {
        instantiationsParameters.append(*instantiation->getParameters());
    }

    return instantiationsParameters;
}
