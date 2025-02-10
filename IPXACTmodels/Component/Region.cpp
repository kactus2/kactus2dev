//-----------------------------------------------------------------------------
// File: Region.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 15.06.2023
//
// Description:
// Region describes the location and size of an area in memory.
//-----------------------------------------------------------------------------

#include "Region.h"

//-----------------------------------------------------------------------------
// Function: Region::Region()
//-----------------------------------------------------------------------------
Region::Region(QString const& name, QString const& addressOffset, QString const& range):
NameGroup(name),
Extendable(),
addressOffset_(addressOffset),
range_(range)
{

}

//-----------------------------------------------------------------------------
// Function: Region::Region()
//-----------------------------------------------------------------------------
Region::Region( const Region& other ):
NameGroup(other),
    Extendable(other),
    addressOffset_(other.addressOffset_),
    range_(other.range_)
{

}

//-----------------------------------------------------------------------------
// Function: Region::~Region()
//-----------------------------------------------------------------------------
Region::~Region()
{

}

//-----------------------------------------------------------------------------
// Function: Region::operator=()
//-----------------------------------------------------------------------------
Region& Region::operator=( const Region& other )
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);

		addressOffset_ = other.addressOffset_;
		range_ = other.range_;
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Region::getAddressOffset()
//-----------------------------------------------------------------------------
QString Region::getAddressOffset() const
{
	return addressOffset_;
}

//-----------------------------------------------------------------------------
// Function: Region::setOffset()
//-----------------------------------------------------------------------------
void Region::setOffset( QString const& addressOffset )
{
    addressOffset_ = addressOffset;
}

//-----------------------------------------------------------------------------
// Function: Region::getRange()
//-----------------------------------------------------------------------------
QString Region::getRange() const
{
	return range_;
}

//-----------------------------------------------------------------------------
// Function: Region::setRange()
//-----------------------------------------------------------------------------
void Region::setRange( QString const& range )
{
    range_ = range;
}
