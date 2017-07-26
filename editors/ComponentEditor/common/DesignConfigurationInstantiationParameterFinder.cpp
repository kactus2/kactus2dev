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
// Function: DesignConfigurationInstantiationParameterFinder::~DesignConfigurationInstantiationParameterFinder()
//-----------------------------------------------------------------------------
DesignConfigurationInstantiationParameterFinder::~DesignConfigurationInstantiationParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::DesignConfigurationInstantiationParameterFinder()
//-----------------------------------------------------------------------------
QSharedPointer<Parameter> DesignConfigurationInstantiationParameterFinder::getParameterWithID(QString const& parameterId) const
{
    // First, search for the parameter corresponding the id.
    QSharedPointer<Parameter> parameter = searchParameter(parameterId);

    return parameter;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::hasId()
//-----------------------------------------------------------------------------
bool DesignConfigurationInstantiationParameterFinder::hasId(QString const& id) const
{
    if (!designConfigurationInstantiation_.isNull() && searchParameter(id))
    {
        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::nameForId()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationParameterFinder::nameForId(QString const& id) const
{
    QSharedPointer <Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->name();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString DesignConfigurationInstantiationParameterFinder::valueForId(QString const& id) const
{
    QSharedPointer<Parameter> targetParameter = getParameterWithID(id);

    if (targetParameter)
    {
        return targetParameter->getValue();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: DesignConfigurationInstantiationParameterFinder::getAllParameterIds()
//-----------------------------------------------------------------------------
QStringList DesignConfigurationInstantiationParameterFinder::getAllParameterIds() const
{
    QStringList allParameterIds;

    if (designConfigurationInstantiation_)
    {
        foreach (QSharedPointer<Parameter> parameter, *designConfigurationInstantiation_->getParameters())
        {
            allParameterIds.append(parameter->getValueId());
        }
    }

    allParameterIds.removeAll("");
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
QSharedPointer<Parameter> DesignConfigurationInstantiationParameterFinder::searchParameter(QString const& parameterId) const
{
    if (designConfigurationInstantiation_)
    {
        foreach (QSharedPointer<Parameter> parameter, *designConfigurationInstantiation_->getParameters())
        {
            if (parameter->getValueId() == parameterId)
            {
                return parameter;
            }
        }
    }    

    return QSharedPointer<Parameter>();
}