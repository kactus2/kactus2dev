//-----------------------------------------------------------------------------
// File: WirePortsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2019
//
// Description:
// Sorting proxy model for wire ports.
//-----------------------------------------------------------------------------

#include "WirePortsFilter.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: WirePortsFilter::WirePortsFilter()
//-----------------------------------------------------------------------------
WirePortsFilter::WirePortsFilter(QObject* parent):
PortsFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: WirePortsFilter::portIsAccepted()
//-----------------------------------------------------------------------------
bool WirePortsFilter::portIsAccepted(QSharedPointer<Port> port) const
{
    if (port->getTransactional())
    {
        return false;
    }
    else
    {
        return true;
    }
}
