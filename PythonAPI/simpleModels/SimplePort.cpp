//-----------------------------------------------------------------------------
// File: SimplePort.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.02.2020
//
// Description:
// Simplified port data model.
//-----------------------------------------------------------------------------

#include "SimplePort.h"

#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: SimplePort::SimplePort()
//-----------------------------------------------------------------------------
SimplePort::SimplePort(QString const& name, QString const& description, int const& bitWidth,
    QString const& direction, QString const& dataType):
SimpleNameGroup(name, description),
dataType_(dataType),
bitWidth_(bitWidth),
direction_(direction)
{

}

//-----------------------------------------------------------------------------
// Function: SimplePort::getDataType()
//-----------------------------------------------------------------------------
QString SimplePort::getDataType() const
{
    return dataType_;
}

//-----------------------------------------------------------------------------
// Function: SimplePort::getBitWidth()
//-----------------------------------------------------------------------------
int SimplePort::getBitWidth() const
{
    return bitWidth_;
}

//-----------------------------------------------------------------------------
// Function: SimplePort::getDirection()
//-----------------------------------------------------------------------------
QString SimplePort::getDirection() const
{
    return direction_;
}
