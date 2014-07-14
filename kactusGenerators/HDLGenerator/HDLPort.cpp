//-----------------------------------------------------------------------------
// File: HDLPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.7.2014
//
// Description:
// Base class for all HDL port declarations.
//-----------------------------------------------------------------------------

#include "HDLPort.h"

#include <IPXACTmodels/port.h>

//-----------------------------------------------------------------------------
// Function: HDLPort::HDLPort()
//-----------------------------------------------------------------------------
HDLPort::HDLPort(Port* port, QObject* parent) :
HDLObject(parent, port->getName(), port->getTypeName(), port->getDefaultValue(), port->getDescription()),
    direction_(port->getDirection()),
    left_(port->getLeftBound()),
    right_(port->getRightBound()),
    connected_(false) 
{

}

//-----------------------------------------------------------------------------
// Function: HDLPort::~HDLPort()
//-----------------------------------------------------------------------------
HDLPort::~HDLPort()
{

}

//-----------------------------------------------------------------------------
// Function: HDLPort::size()
//-----------------------------------------------------------------------------
int HDLPort::size() const {
	return left() - right() + 1;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::left()
//-----------------------------------------------------------------------------
int HDLPort::left() const
{
	return left_;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::right()
//-----------------------------------------------------------------------------
int HDLPort::right() const
{
	return right_;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::direction()
//-----------------------------------------------------------------------------
General::Direction HDLPort::direction() const
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::setConnected()
//-----------------------------------------------------------------------------
void HDLPort::setConnected(bool shouldConnect)
{
	connected_ = shouldConnect;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::isConnected()
//-----------------------------------------------------------------------------
bool HDLPort::isConnected() const
{
	return connected_;
}

//-----------------------------------------------------------------------------
// Function: HDLPort::hasConnectedPorts()
//-----------------------------------------------------------------------------
bool HDLPort::hasConnectedPorts( QList<QSharedPointer<HDLPort> > const& ports ) {
	foreach (QSharedPointer<HDLPort> port, ports)
    {
		// if at least one port that is connected is found
		if (port->isConnected())
        {
			return true;
		}
	}

	// all ports were unconnected.
	return false;
}
