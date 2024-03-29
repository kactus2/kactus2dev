//-----------------------------------------------------------------------------
// File: DesignParameterFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 04.04.2022
//
// Description:
// The implementation for finding parameters for designs.
//-----------------------------------------------------------------------------

#include "DesignParameterFinder.h"

#include <IPXACTmodels/common/ConfigurableVLNVReference.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>
#include <IPXACTmodels/Component/DesignInstantiation.h>

//-----------------------------------------------------------------------------
// Function: ComponentInstanceParameterFinder::ComponentInstanceParameterFinder()
//-----------------------------------------------------------------------------
DesignParameterFinder::DesignParameterFinder(QSharedPointer<DesignInstantiation> designInstantiation):
ListParameterFinder()
{
    if (designInstantiation && designInstantiation->getDesignReference())
    {
        configurableElementValues_ = designInstantiation->getDesignReference()->getConfigurableElementValues();
    }
}

//-----------------------------------------------------------------------------
// Function: DesignParameterFinder::valueForId()
//-----------------------------------------------------------------------------
QString DesignParameterFinder::valueForId(QStringView id) const
{
    if (configurableElementValues_)
    {
        for (QSharedPointer<ConfigurableElementValue> element : *configurableElementValues_)
        {
            if (element->getReferenceId().compare(id) == 0)
            {
                return element->getConfigurableValue();
            }
        }
    }

    return ListParameterFinder::valueForId(id);
}
