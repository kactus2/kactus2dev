/* 
 *  	Created on: 17.2.2012
 *      Author: Antti Kamppi
 * 		filename: segment.cpp
 *		Project: Kactus 2
 */

#include "segment.h"

#include "generaldeclarations.h"

#include "GenericVendorExtension.h"
#include "XmlUtils.h"
#include "VendorExtension.h"

#include <QStringList>

Segment::Segment():
NameGroup(),
addressOffset_(),
offsetAttributes_(),
range_(),
rangeAttributes_(),
vendorExtensions_()
{

}

Segment::Segment( const Segment& other ):
NameGroup(other),
addressOffset_(other.addressOffset_),
offsetAttributes_(other.offsetAttributes_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
vendorExtensions_(other.vendorExtensions_)
{
}

Segment::~Segment() {
}

Segment& Segment::operator=( const Segment& other ) {

	if (this != &other) {
		NameGroup::operator=(other);
		addressOffset_ = other.addressOffset_;
		offsetAttributes_ = other.offsetAttributes_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
        vendorExtensions_ = other.vendorExtensions_;
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
}

QString Segment::getAddressOffset() const {
	return addressOffset_;
}

QString Segment::getRange() const {
	return range_;
}

const QMap<QString, QString>& Segment::getOffsetAttributes() const {
	return offsetAttributes_;
}

const QMap<QString, QString>& Segment::getRangeAttributes() const {
	return rangeAttributes_;
}

void Segment::setOffset( const QString& addressOffset ) {
	addressOffset_ = addressOffset;
}

void Segment::setRange( const QString& range ) {
	range_ = range;
}

void Segment::setOffsetAttributes( QMap<QString, QString> offsetAttributes )
{
	offsetAttributes_ = offsetAttributes;
}

void Segment::setRangeAttributes( QMap<QString, QString> rangeAttributes )
{
	rangeAttributes_ = rangeAttributes;
}
