//-----------------------------------------------------------------------------
// File: ParameterConfigurableElementFinder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 18.02.2026
//
// Description:
// The implementation for finding configurable elements from a list with the correct ID.
//-----------------------------------------------------------------------------

#include "ParameterConfigurableElementFinder.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ConfigurableElementValue.h>

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementFinder::ParameterConfigurableElementFinder()
//-----------------------------------------------------------------------------
ParameterConfigurableElementFinder::ParameterConfigurableElementFinder():
ListParameterFinder()
{

}

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementFinder::setCEVList()
//-----------------------------------------------------------------------------
void ParameterConfigurableElementFinder::setCEVList(QSharedPointer<QList<QSharedPointer<ConfigurableElementValue> > > elementList) noexcept
{
    cevList_ = elementList;
}

//-----------------------------------------------------------------------------
// Function: ParameterConfigurableElementFinder::valueForId()
//-----------------------------------------------------------------------------
QString ParameterConfigurableElementFinder::valueForId(QStringView id) const
{
    if (cevList_ != nullptr)
    {
		for (auto element : *cevList_)
		{
			if (element->getReferenceId() == id)
			{
				return element->getConfigurableValue();
			}
		}
    }

    return ListParameterFinder::valueForId(id);
}
