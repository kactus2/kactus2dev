//-----------------------------------------------------------------------------
// File: RemapPort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 23.09.2015
//
// Description:
// Describes the ipxact:remapPort element.
//-----------------------------------------------------------------------------

#include "RemapPort.h"

//-----------------------------------------------------------------------------
// Function: RemapPort::RemapPort()
//-----------------------------------------------------------------------------
RemapPort::RemapPort(QString const& portReference) :
value_(),
    portRef_(portReference),
    portIndex_()
{

}

//-----------------------------------------------------------------------------
// Function: RemapPort::RemapPort()
//-----------------------------------------------------------------------------
RemapPort::RemapPort(const RemapPort &other) :
value_(other.value_),
    portRef_(other.portRef_),
    portIndex_(other.portIndex_)
{

}

//-----------------------------------------------------------------------------
// Function: RemapPort::operator=()
//-----------------------------------------------------------------------------
RemapPort& RemapPort::operator=(const RemapPort& other)
{
    if (this != &other)
    {
        value_ = other.value_;
        portRef_ = other.portRef_;
        portIndex_ = other.portIndex_;
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::~RemapPort()
//-----------------------------------------------------------------------------
RemapPort::~RemapPort()
{

}

//-----------------------------------------------------------------------------
// Function: RemapPort::getPortNameRef()
//-----------------------------------------------------------------------------
QString RemapPort::getPortNameRef() const
{
    return portRef_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setPortNameRef()
//-----------------------------------------------------------------------------
void RemapPort::setPortNameRef(QString const& newPortNameRef)
{
    portRef_ = newPortNameRef;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::getPortIndex()
//-----------------------------------------------------------------------------
QString RemapPort::getPortIndex() const
{
    return portIndex_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setPortIndex()
//-----------------------------------------------------------------------------
void RemapPort::setPortIndex(QString const& newPortIndex)
{
    portIndex_ = newPortIndex;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::getValue()
//-----------------------------------------------------------------------------
QString RemapPort::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: RemapPort::setValue()
//-----------------------------------------------------------------------------
void RemapPort::setValue(QString const& newValue)
{
    value_ = newValue;
}
