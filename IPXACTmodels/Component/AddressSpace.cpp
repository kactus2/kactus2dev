/* 
 *
 *  Created on: 28.7.2010
 *      Author: Antti Kamppi
 */

#include "AddressSpace.h"
#include "../GenericVendorExtension.h"

#include <common/utils.h>

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterReader.h>
#include <IPXACTmodels/common/ParameterWriter.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QString>
#include <QObject>
#include <QList>
#include <QXmlStreamWriter>

const int DEFAULT_ADDRESS_UNIT_BITS = 8;

AddressSpace::AddressSpace( const AddressSpace &other ):
NameGroup(other),
range_(other.range_),
width_(other.width_),
segments_(),
addressUnitBits_(other.addressUnitBits_),
localMemoryMap_(),
parameters_(),
vendorExtensions_(other.vendorExtensions_)
 {
	if (other.localMemoryMap_)
	{
		localMemoryMap_ = QSharedPointer<MemoryMapBase>(
			new MemoryMapBase(*other.localMemoryMap_.data()));
	}

	parameters_.clear();
	foreach (QSharedPointer<Parameter> param, other.parameters_)
	{
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}

	segments_.clear();
	foreach (QSharedPointer<Segment> segment, other.segments_)
	{
		if (segment) {
			QSharedPointer<Segment> copy = QSharedPointer<Segment>(
				new Segment(*segment.data()));
			segments_.append(copy);
		}
	}
}

AddressSpace::AddressSpace():
NameGroup(),
range_(),
width_(-1), 
segments_(),
addressUnitBits_(DEFAULT_ADDRESS_UNIT_BITS), 
localMemoryMap_(0),
parameters_(),
vendorExtensions_()
{
}

AddressSpace & AddressSpace::operator=( const AddressSpace &other ) {
	if (this != &other) {
		NameGroup::operator=(other);
		range_ = other.range_;
		width_ = other.width_;
		addressUnitBits_ = other.addressUnitBits_;
        vendorExtensions_ = other.vendorExtensions_;

		if (other.localMemoryMap_) {
			localMemoryMap_ = QSharedPointer<MemoryMapBase>(
				new MemoryMapBase(*other.localMemoryMap_.data()));
		}
		else
			localMemoryMap_ = QSharedPointer<MemoryMapBase>();

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_)
		{
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}

		segments_.clear();
		foreach (QSharedPointer<Segment> segment, other.segments_)
		{
			if (segment) {
				QSharedPointer<Segment> copy = QSharedPointer<Segment>(
					new Segment(*segment.data()));
				segments_.append(copy);
			}
		}
	}
	return *this;
}

// the destructor
AddressSpace::~AddressSpace() {
	segments_.clear();
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getIsPresent()
//-----------------------------------------------------------------------------
QString AddressSpace::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setIsPresent()
//-----------------------------------------------------------------------------
void AddressSpace::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: addressspace::isValid()
//-----------------------------------------------------------------------------
bool AddressSpace::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames, QStringList& errorList, const QString& parentIdentifier ) const
{
	bool valid = true;
	const QString thisIdentifier(QObject::tr("address space %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (range_->getLeft().isEmpty() || range_->getRight().isEmpty()) {
		errorList.append(QObject::tr("No range set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	if (width_ < 0) {
		errorList.append(QObject::tr("No width set for %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<Segment> segment, segments_) {
		if (!segment->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	/*if (localMemoryMap_) {
		
		// if the local memory map contains definitions but is not valid
		if (!localMemoryMap_->isEmpty() && !localMemoryMap_->isValid(componentChoices, remapStateNames, errorList,
            thisIdentifier))
        {
			valid = false;
		}
	}*/

//     ParameterValidator2014 validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), parentIdentifier, componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
//     }

	return valid;
}

//-----------------------------------------------------------------------------
// Function: addressspace::isValid()
//-----------------------------------------------------------------------------
bool AddressSpace::isValid(QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
    QStringList remapStateNames) const
{
	if (name().isEmpty()) {
		return false;
	}

	if (range_->getLeft().isEmpty()||range_->getRight().isEmpty()) {
		return false;
	}

	if (width_ < 0) {
		return false;
	}

	foreach (QSharedPointer<Segment> segment, segments_) {
		if (!segment->isValid()) {
			return false;
		}
	}

	/*if (localMemoryMap_) {

		// if the memory map contains definitions but is not valid
		if (!localMemoryMap_->isEmpty() && !localMemoryMap_->isValid(componentChoices, remapStateNames))
        {
			return false;
		}
	}*/

//     ParameterValidator2014 validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }
	return true;
}

void AddressSpace::setAddressUnitBits(unsigned int addressUnitBits) {
	addressUnitBits_ = addressUnitBits;
}

unsigned int AddressSpace::getAddressUnitBits() const {
	return addressUnitBits_;
}

void AddressSpace::setRange(const QSharedPointer<Range> range) {
	range_ = range;
}

QSharedPointer<Range> AddressSpace::getRange() const
{
	return range_;
}

int AddressSpace::getWidth() const {
	return width_;
}

void AddressSpace::setWidth(int width) {
	width_ = width;
}

QSharedPointer<MemoryMapBase> AddressSpace::getLocalMemoryMap() {
	if (!localMemoryMap_)
	{
		localMemoryMap_ = QSharedPointer<MemoryMapBase>(new MemoryMapBase());
	}
	return localMemoryMap_;
}

bool AddressSpace::hasLocalMemoryMap() const
{
    return localMemoryMap_;
}

void AddressSpace::setLocalMemoryMap( QSharedPointer<MemoryMapBase> localMemoryMap ) {
	if (localMemoryMap_) {
		localMemoryMap_.clear();
	}
	localMemoryMap_ = localMemoryMap;
}

QList<QSharedPointer<Segment> >& AddressSpace::getSegments() {
	return segments_;
}

const QList<QSharedPointer<Segment> >& AddressSpace::getSegments() const {
	return segments_;
}

void AddressSpace::setSegments( const QList<QSharedPointer<Segment> >& segments ) {
	segments_ = segments;
}

QList<QSharedPointer<Parameter> >& AddressSpace::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& AddressSpace::getParameters() const {
	return parameters_;
}

quint64 AddressSpace::getLastAddress() const
{
	quint64 range = General::str2Uint(range_->getRight());
	
	// if range is undefined
	if (range <= 0) {
		return 0;
	}
	return range - 1;
}
