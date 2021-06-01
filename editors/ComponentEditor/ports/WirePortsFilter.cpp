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

#include <editors/ComponentEditor/ports/WirePortColumns.h>
#include <editors/ComponentEditor/ports/interfaces/PortsInterface.h>

//-----------------------------------------------------------------------------
// Function: WirePortsFilter::WirePortsFilter()
//-----------------------------------------------------------------------------
WirePortsFilter::WirePortsFilter(QSharedPointer<PortsInterface> portInterface, QObject* parent):
PortsFilter(portInterface, parent)
{

}

//-----------------------------------------------------------------------------
// Function: WirePortsFilter::nameColumn()
//-----------------------------------------------------------------------------
int WirePortsFilter::nameColumn() const
{
    return WirePortColumns::NAME;
}

//-----------------------------------------------------------------------------
// Function: WirePortsFilter::portIsAccepted()
//-----------------------------------------------------------------------------
bool WirePortsFilter::portIsAccepted(QString const& portName) const
{
    if (getInterface()->portIsTransactional(portName.toStdString()))
    {
        return false;
    }
    else
    {
        return true;
    }
}
