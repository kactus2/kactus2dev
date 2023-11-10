//-----------------------------------------------------------------------------
// File: DesignConfigurationInstantiationParameterFinder.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 05.05.2017
//
// Description:
// For finding parameters with the correct ID.
//-----------------------------------------------------------------------------

#include "DesignConfigurationInstantiationParameterFinder.h"

#include <IPXACTmodels/Component/DesignConfigurationInstantiation.h>
#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::DesignConfigurationInstantiationParameterFinder()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationParameterFinder::DesignConfigurationInstantiationParameterFinder
    (QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation)
    : designConfigurationInstantiation_(designConfigurationInstantiation)
{
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::DesignConfigurationInstantiationParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> DesignConfigurationInstantiationParameterFinder::getParameterWithID(
    QStringView parameterId) const
{
    return searchParameter(parameterId);
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool DesignConfigurationInstantiationParameterFinder::hasId(QStringView id) const
{
    return searchParameter(id) != nullptr;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationParameterFinder::nameForId(QStringView id) const
{
    if (QSharedPointer <Parameter> targetParameter = getParameterWithID(id);
        targetParameter)
    {
        return targetParameter->name();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationParameterFinder::valueForId(QStringView id) const
{
    if (QSharedPointer<Parameter> targetParameter = getParameterWithID(id); 
        targetParameter)
    {
        return targetParameter->getValue();
    }

    return QString();
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList DesignConfigurationInstantiationParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (designConfigurationInstantiation_)
    {
        for (QSharedPointer<Parameter> parameter : *designConfigurationInstantiation_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    allParameterIds.removeAll(QString());
    return allParameterIds;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::getNumberOfParameters()
//-----------------------------------------------------------------------------
int DesignConfigurationInstantiationParameterFinder::getNumberOfParameters() const
{
    int parameterCount = 0;
    if (designConfigurationInstantiation_)
    {
        parameterCount += designConfigurationInstantiation_->getParameters()->count();
    }
    return parameterCount;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::setDesignConfigurationInstantiation()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationParameterFinder::setDesignConfigurationInstantiation
    (QSharedPointer<DesignConfigurationInstantiation const> designConfigurationInstantiation)
{
    designConfigurationInstantiation_ = designConfigurationInstantiation;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::registerParameterModel()
//-----------------------------------------------------------------------------
void DesignConfigurationInstantiationParameterFinder::registerParameterModel(QAbstractItemModel const* /*model*/)
{
    // Nothing to do.
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::searchParameter()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> DesignConfigurationInstantiationParameterFinder::searchParameter(
    QStringView parameterId) const
{
    if (designConfigurationInstantiation_)
    {
        for (QSharedPointer<Parameter> parameter : *designConfigurationInstantiation_->getParameters())
        {
            if (parameter->getValueId().compare(parameterId) == 0)
            {
                return parameter;
            }
        }
    }    

    return QSharedPointer<Parameter>();
}