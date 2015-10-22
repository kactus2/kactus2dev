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

// #include "segment.h"
#include "Segment.h"

// #include "generaldeclarations.h"

// #include "GenericVendorExtension.h"
// #include "XmlUtils.h"
// #include "VendorExtension.h"

// #include <QStringList>

//-----------------------------------------------------------------------------
// Function: Segment::Segment()
//-----------------------------------------------------------------------------
Segment::Segment(QString const& name, QString const& addressOffset, QString const& range):
NameGroup(name),
addressOffset_(addressOffset),
offsetAttributes_(),
range_(range),
rangeAttributes_()
// vendorExtensions_()
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
// vendorExtensions_(other.vendorExtensions_)
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
//         vendorExtensions_ = other.vendorExtensions_;
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
/*
bool Segment::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("segment %1").arg(name()));

	if (name().isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No name set for segment within %1").arg(parentIdentifier));
	}

	if (addressOffset_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No address offset set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	if (range_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No range set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	return valid;
}

bool Segment::isValid() const {
	
	if (name().isEmpty()) {
		return false;
	}
	else if (addressOffset_.isEmpty()) {
		return false;
	}
	else if (range_.isEmpty()) {
		return false;
	}

	return true;
}*/

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
