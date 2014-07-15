//-----------------------------------------------------------------------------
// File: VerilogPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 14.7.2014
//
// Description:
// VerilogPort is used within top component entity to print the declaration for one port.
//-----------------------------------------------------------------------------

#include "VerilogPort.h"

#include <kactusGenerators/HDLGenerator/HDLUtils.h>

#include <IPXACTmodels/port.h>

//-----------------------------------------------------------------------------
// Function: VerilogPort::VerilogPort()
//-----------------------------------------------------------------------------
VerilogPort::VerilogPort(Port* port, QObject* parent):
HDLPort(port, parent)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPort::VerilogPort()
//-----------------------------------------------------------------------------
VerilogPort::~VerilogPort()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPort::write()
//-----------------------------------------------------------------------------
void VerilogPort::write( QTextStream& stream ) const
{
    if (nonWriteable())
    {
        return;
    }

    stream << directionString() << " ";

    if (!type().isEmpty())
    {
        stream << type() << " ";
    }

    if (size() != 1)
    {
        stream << "[" << left() << ":" << right() << "]" << " ";
    }

    stream << name() << ";";


    if (!description().isEmpty())
    {
        stream << " ";
        HDLUtils::writeDescription(description(), stream, "//");
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPort::nonWriteable()
//-----------------------------------------------------------------------------
bool VerilogPort::nonWriteable() const
{
    return direction() == General::DIRECTION_INVALID || direction() == General::DIRECTION_PHANTOM ||
        name().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: VerilogPort::directionString()
//-----------------------------------------------------------------------------
QString VerilogPort::directionString() const
{
    QString dir;

    if (direction() == General::IN)
    {
        dir = "input";    
    }
    else if(direction() == General::OUT)
    {
        dir = "output";    
    }
    else if (direction() == General::INOUT)    
    {
        dir = "inout";    
    }

    return dir;
}
