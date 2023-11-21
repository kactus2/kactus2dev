//-----------------------------------------------------------------------------
// File: Structural.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 20.11.2023
//
// Description:
// Implementation of ipxact:structural in component port.
//-----------------------------------------------------------------------------

#include "Structural.h"


//-----------------------------------------------------------------------------
// Function: Structural::setType()
//-----------------------------------------------------------------------------
constexpr void Structural::setType(Type type) noexcept
{
    portType_ = type;
}

//-----------------------------------------------------------------------------
// Function: Structural::getType()
//-----------------------------------------------------------------------------
constexpr Structural::Type Structural::getType() const noexcept
{
    return portType_;
}

//-----------------------------------------------------------------------------
// Function: Structural::setDirection()
//-----------------------------------------------------------------------------
constexpr void Structural::setDirection(DirectionTypes::Direction direction) noexcept
{
    direction_ = direction;
}

//-----------------------------------------------------------------------------
// Function: Structural::getDirection()
//-----------------------------------------------------------------------------
constexpr DirectionTypes::Direction Structural::getDirection() const noexcept
{
    return direction_;
}

//-----------------------------------------------------------------------------
// Function: Structural::getVectors()
//-----------------------------------------------------------------------------
QSharedPointer<QList<Vector> > Structural::getVectors() const
{
    return vectors_;
}

//-----------------------------------------------------------------------------
// Function: Structural::toString()
//-----------------------------------------------------------------------------
QString Structural::toString(Type type)
{
    if (type == Type::Struct)
    {
        return QStringLiteral("struct");
    }
    else if (type == Type::Union)
    {
        return QStringLiteral("union");
    }
    else if (type == Type::Interface)
    {
        return QStringLiteral("interface");
    }

    return QString();
}
