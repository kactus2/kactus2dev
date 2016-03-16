//-----------------------------------------------------------------------------
// File: Segment.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.10.2015
//
// Description:
// Segment describes the location and size of an area in containing address space.
//-----------------------------------------------------------------------------

#include "Segment.h"

//-----------------------------------------------------------------------------
// Function: Segment::Segment()
//-----------------------------------------------------------------------------
Segment::Segment(QString const& name, QString const& addressOffset, QString const& range):
NameGroup(name),
addressOffset_(addressOffset),
offsetAttributes_(),
range_(range),
rangeAttributes_()
{

}

//-----------------------------------------------------------------------------
// Function: Segment::Segment()
//-----------------------------------------------------------------------------
Segment::Segment( const Segment& other ):
NameGroup(other),
addressOffset_(other.addressOffset_),
offsetAttributes_(other.offsetAttributes_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_)
{

}

//-----------------------------------------------------------------------------
// Function: Segment::~Segment()
//-----------------------------------------------------------------------------
Segment::~Segment()
{

}

//-----------------------------------------------------------------------------
// Function: Segment::operator=()
//-----------------------------------------------------------------------------
Segment& Segment::operator=( const Segment& other )
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);

		addressOffset_ = other.addressOffset_;
		offsetAttributes_ = other.offsetAttributes_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Segment::getIsPresent()
//-----------------------------------------------------------------------------
QString Segment::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Segment::setIsPresent()
//-----------------------------------------------------------------------------
void Segment::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Segment::getAddressOffset()
//-----------------------------------------------------------------------------
QString Segment::getAddressOffset() const
{
	return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: Segment::setOffset()
//-----------------------------------------------------------------------------
void Segment::setOffset( QString const& addressOffset )
{
    addressOffset_ = addressOffset;
}

//-----------------------------------------------------------------------------
// Function: Segment::getOffsetAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> Segment::getOffsetAttributes() const
{
    return offsetAttributes_;
}

//-----------------------------------------------------------------------------
// Function: Segment::setOffsetAttributes()
//-----------------------------------------------------------------------------
void Segment::setOffsetAttributes( QMap<QString, QString> offsetAttributes )
{
    offsetAttributes_ = offsetAttributes;
}

//-----------------------------------------------------------------------------
// Function: Segment::getRange()
//-----------------------------------------------------------------------------
QString Segment::getRange() const
{
	return range_;
}

//-----------------------------------------------------------------------------
// Function: Segment::setRange()
//-----------------------------------------------------------------------------
void Segment::setRange( QString const& range )
{
    range_ = range;
}

//-----------------------------------------------------------------------------
// Function: Segment::getRangeAttributes()
//-----------------------------------------------------------------------------
QMap<QString, QString> Segment::getRangeAttributes() const
{
	return rangeAttributes_;
}

//-----------------------------------------------------------------------------
// Function: Segment::setRangeAttributes()
//-----------------------------------------------------------------------------
void Segment::setRangeAttributes( QMap<QString, QString> rangeAttributes )
{
	rangeAttributes_ = rangeAttributes;
}
