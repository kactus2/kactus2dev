//-----------------------------------------------------------------------------
// File: TransactionalPortsFilter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.05.2019
//
// Description:
// Sorting proxy model for transactional ports.
//-----------------------------------------------------------------------------

#include "TransactionalPortsFilter.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: TransactionalPortsFilter::TransactionalPortsFilter()
//-----------------------------------------------------------------------------
TransactionalPortsFilter::TransactionalPortsFilter(QObject* parent):
PortsFilter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: TransactionalPortsFilter::portIsAccepted()
//-----------------------------------------------------------------------------
bool TransactionalPortsFilter::portIsAccepted(QSharedPointer<Port> port) const
{
    if (port->getWire())
    {
        return false;
    }
    else
    {
        return true;
    }
}
