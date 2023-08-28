//-----------------------------------------------------------------------------
// File: PacketField.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 25.05.2023
//
// Description:
// Implementation for ipxact:packetField element within abstraction definition.
//-----------------------------------------------------------------------------

#include "PacketField.h"

//-----------------------------------------------------------------------------
// Function: PacketField::PacketField()
//-----------------------------------------------------------------------------
PacketField::PacketField(QString const& name):
NameGroup(name),
Extendable()
{
}

//-----------------------------------------------------------------------------
// Function: PacketField::PacketField()
//-----------------------------------------------------------------------------
PacketField::PacketField(PacketField const& other) :
    NameGroup(other),
    Extendable(other),
    width_(other.width_),
    value_(other.value_),
    endianness_(other.endianness_),
    qualifier_(new Qualifier(*other.qualifier_))
{

}

//-----------------------------------------------------------------------------
// Function: PacketField::operator==()
//-----------------------------------------------------------------------------
PacketField& PacketField::operator==(PacketField const& other)
{
    if (this != &other)
    {
        NameGroup::operator=(other);
        Extendable::operator=(other);
        width_ = other.width_;
        value_ = other.value_;
        endianness_ = other.endianness_;

        if (other.qualifier_)
        {
            qualifier_ = QSharedPointer<Qualifier>(new Qualifier(*other.qualifier_));
        }
    }

    return *this;
}

//-----------------------------------------------------------------------------
// Function: PacketField::getWidth()
//-----------------------------------------------------------------------------
QString PacketField::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: PacketField::setWidth()
//-----------------------------------------------------------------------------
void PacketField::setWidth(QString const& width)
{
    width_ = width;
}

//-----------------------------------------------------------------------------
// Function: PacketField::getValue()
//-----------------------------------------------------------------------------
QString PacketField::getValue() const
{
    return value_;
}

//-----------------------------------------------------------------------------
// Function: PacketField::setValue()
//-----------------------------------------------------------------------------
void PacketField::setValue(QString const& value)
{
    value_ = value;
}

//-----------------------------------------------------------------------------
// Function: PacketField::getEndianness()
//-----------------------------------------------------------------------------
QString PacketField::getEndianness() const
{
    return endianness_;
}

//-----------------------------------------------------------------------------
// Function: PacketField::setEndianness()
//-----------------------------------------------------------------------------
void PacketField::setEndianness(QString const& endianness)
{
    endianness_ = endianness;
}

//-----------------------------------------------------------------------------
// Function: PacketField::getQualifier()
//-----------------------------------------------------------------------------
QSharedPointer<Qualifier> PacketField::getQualifier() const
{
    return qualifier_;
}

//-----------------------------------------------------------------------------
// Function: PacketField::setQualifier()
//-----------------------------------------------------------------------------
void PacketField::setQualifier(QSharedPointer<Qualifier> const& qualifier)
{
    qualifier_ = qualifier;
}
