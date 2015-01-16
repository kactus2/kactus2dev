//-----------------------------------------------------------------------------
// File: ParameterResolver.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 12.01.2015
//
// Description:
// Finds a parameter in a component using an id.
//-----------------------------------------------------------------------------

#include "ParameterResolver.h"

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/parameter.h>

//-----------------------------------------------------------------------------
// Function: ParameterResolver::ParameterResolver()
//-----------------------------------------------------------------------------
ParameterResolver::ParameterResolver(QSharedPointer<Component> targetComponent): component_(targetComponent)
{

}

//-----------------------------------------------------------------------------
// Function: ParameterResolver::~ParameterResolver()
//-----------------------------------------------------------------------------
ParameterResolver::~ParameterResolver()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterResolver::hasId()
//-----------------------------------------------------------------------------
bool ParameterResolver::hasId(QString const& id) const
{
    foreach(QSharedPointer<Parameter> parameter, component_->getParameters())
    {
        if (parameter->getValueId() == id)
        {
            return true;
        }
    }
    return false;
}

//-----------------------------------------------------------------------------
// Function: ParameterResolver::nameForId()
//-----------------------------------------------------------------------------
QString ParameterResolver::nameForId(QString const& id) const
{
    foreach(QSharedPointer<Parameter> parameter, component_->getParameters())
    {
        if (parameter->getValueId() == id)
        {
            return parameter->getName();
        }
    }

   return QString();
}
