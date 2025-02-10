//-----------------------------------------------------------------------------
// File: ComponentInstantiationParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 05.05.2017
//
// Description:
// For finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "ComponentInstantiationParameterFinder.h"

#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::ComponentInstantiationParameterFinder()
//-----------------------------------------------------------------------------
ComponentInstantiationParameterFinder::ComponentInstantiationParameterFinder
    (QSharedPointer<ComponentInstantiation const> componentInstantiation) :
    componentInstantiation_(componentInstantiation)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::ComponentInstantiationParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentInstantiationParameterFinder::getParameterWithID(QStringView parameterId) const
{
    return searchParameter(parameterId);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentInstantiationParameterFinder::hasId(QStringView id) const
{
    return !componentInstantiation_.isNull() && searchParameter(id);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentInstantiationParameterFinder::nameForId(QStringView id) const
{
    if (QSharedPointer <Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentInstantiationParameterFinder::valueForId(QStringView id) const
{
    if (QSharedPointer<Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->getValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ComponentInstantiationParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (componentInstantiation_)
    {
        for (QSharedPointer<ModuleParameter> parameter : *componentInstantiation_->getModuleParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        for (QSharedPointer<Parameter> parameter : *componentInstantiation_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    allParameterIds.removeAll(QString());
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int ComponentInstantiationParameterFinder::getNumberOfParameters() const
{
    int parameterCount = 0;
    if (componentInstantiation_)
    {
        parameterCount += componentInstantiation_->getModuleParameters()->count();
        parameterCount += componentInstantiation_->getParameters()->count();
    }
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::setComponentInstantiation()
//-----------------------------------------------------------------------------
void ComponentInstantiationParameterFinder::setComponentInstantiation(QSharedPointer<ComponentInstantiation const> componentInstantiation)
{
    componentInstantiation_ = componentInstantiation;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void ComponentInstantiationParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::searchParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentInstantiationParameterFinder::searchParameter(QStringView parameterId) const
{
    if (componentInstantiation_)
    {
        for (QSharedPointer<ModuleParameter> parameter : *componentInstantiation_->getModuleParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        for (QSharedPointer<Parameter> parameter : *componentInstantiation_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }
    }    

    return QSharedPointer<Parameter>();
}