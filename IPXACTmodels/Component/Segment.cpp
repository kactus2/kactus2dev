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
Region(name, addressOffset, range)
{

}

//-----------------------------------------------------------------------------
// Function: Segment::Segment()
//-----------------------------------------------------------------------------
Segment::Segment( const Segment& other ):
	Region(other),
	isPresent_(other.isPresent_),
    offsetAttributes_(other.offsetAttributes_),
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
		Region::operator=(other);

		offsetAttributes_ = other.offsetAttributes_;
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
