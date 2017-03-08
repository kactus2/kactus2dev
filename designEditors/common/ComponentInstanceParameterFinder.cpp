//-----------------------------------------------------------------------------
// File: ComponentInstanceParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 13.01.2017
//
// Description:
// The implementation for finding parameters for component instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceParameterFinder.h"

#include <IPXACTmodels/common/ConfigurableElementValue.h>

#include <IPXACTmodels/Design/ComponentInstance.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceParameterFinder::ComponentInstanceParameterFinder()
//-----------------------------------------------------------------------------
ComponentInstanceParameterFinder::ComponentInstanceParameterFinder(
    QSharedPointer<const ComponentInstance> componentInstance,
    QSharedPointer<const Component> component) : 
ParameterCache(component),
    componentInstance_(componentInstance)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceParameterFinder::~ComponentInstanceParameterFinder()
//-----------------------------------------------------------------------------
ComponentInstanceParameterFinder::~ComponentInstanceParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString ComponentInstanceParameterFinder::valueForId(QString const& id) const
{
    foreach (QSharedPointer<ConfigurableElementValue> element, *componentInstance_->getConfigurableElementValues())
    {
        if (element->getReferenceId().compare(id) == 0)
        {
            return element->getConfigurableValue();
        }
    }

    return ParameterCache::valueForId(id);
}
