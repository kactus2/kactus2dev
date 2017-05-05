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
    (QSharedPointer<ComponentInstantiation const> componentInstantiation) : componentInstantiation_(componentInstantiation)
{
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::~ComponentInstantiationParameterFinder()
//-----------------------------------------------------------------------------
ComponentInstantiationParameterFinder::~ComponentInstantiationParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::ComponentInstantiationParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> ComponentInstantiationParameterFinder::getParameterWithID(QString const& parameterId) const
{
    // First, search for the parameter corresponding the id.
    QSharedPointer<Parameter> parameter = searchParameter(parameterId);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool ComponentInstantiationParameterFinder::hasId(QString const& id) const
{
    if (!componentInstantiation_.isNull() && searchParameter(id))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString ComponentInstantiationParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->name();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentInstantiationParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer<Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->getValue();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: ComponentInstantiationParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList ComponentInstantiationParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (componentInstantiation_)
    {
        foreach (QSharedPointer<ModuleParameter> parameter, *componentInstantiation_->getModuleParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }

        foreach (QSharedPointer<Parameter> parameter, *componentInstantiation_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    allParameterIds.removeAll("");
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
QSharedPointer<Parameter> ComponentInstantiationParameterFinder::searchParameter(QString const& parameterId) const
{
    if (componentInstantiation_)
    {
        foreach (QSharedPointer<ModuleParameter> parameter, *componentInstantiation_->getModuleParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }

        foreach (QSharedPointer<Parameter> parameter, *componentInstantiation_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }
    }    

    return QSharedPointer<Parameter>();
}