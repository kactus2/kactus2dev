//-----------------------------------------------------------------------------
// File: SubPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.11.2023
//
// Description:
// Implementation of ipxact:SubPort in component port.
//-----------------------------------------------------------------------------

#include "SubPort.h"

//-----------------------------------------------------------------------------
// Function: SubPort::setWire()
//-----------------------------------------------------------------------------
void SubPort::setWire(QSharedPointer<Wire> wire)
{
    structured_.clear();
    wire_ = wire;
}

//-----------------------------------------------------------------------------
// Function: SubPort::setStructured()
//-----------------------------------------------------------------------------
void SubPort::setStructured(QSharedPointer<Structured> structured)
{
    wire_.clear();
    structured_ = structured;
}
