//-----------------------------------------------------------------------------
// File: AddressSpace.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: 
// Date:
//
// Description:
// Describes the ipxact:addressSpace element.
//-----------------------------------------------------------------------------

#include "AddressSpace.h"

#include <IPXACTmodels/common/Parameter.h>

//-----------------------------------------------------------------------------
// Function: AddressSpace::()
//-----------------------------------------------------------------------------
AddressSpace::AddressSpace(QString const& name, QString const& range, QString const& width) :
NameGroup(name),
Extendable(),
range_(range),
width_(width),
segments_(new QList<QSharedPointer<Segment> > ()),
addressUnitBits_(),
localMemoryMap_(0),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: AddressSpace::AddressSpace()
//-----------------------------------------------------------------------------
AddressSpace::AddressSpace( const AddressSpace &other ):
NameGroup(other),
Extendable(other),
range_(other.range_),
width_(other.width_),
segments_(new QList<QSharedPointer<Segment> > ()),
addressUnitBits_(other.addressUnitBits_),
localMemoryMap_(),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyLocalMemoryMap(other);
    copySegments(other);
    copyParameters(other);
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::operator=()
//-----------------------------------------------------------------------------
AddressSpace & AddressSpace::operator=( const AddressSpace &other )
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);
		range_ = other.range_;
		width_ = other.width_;
		addressUnitBits_ = other.addressUnitBits_;

        localMemoryMap_.clear();
        copyLocalMemoryMap(other);

        segments_->clear();
        copySegments(other);

        parameters_->clear();
        copyParameters(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::~AddressSpace()
//-----------------------------------------------------------------------------
AddressSpace::~AddressSpace()
{
    localMemoryMap_.clear();
	segments_.clear();
    parameters_.clear();
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
// Function: AddressSpace::setAddressUnitBits()
//-----------------------------------------------------------------------------
void AddressSpace::setAddressUnitBits(QString const& newAddressUnitBits)
{
	addressUnitBits_ = newAddressUnitBits;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getAddressUnitBits()
//-----------------------------------------------------------------------------
QString AddressSpace::getAddressUnitBits() const
{
	return addressUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setRange()
//-----------------------------------------------------------------------------
void AddressSpace::setRange(QString const& newRange)
{
	range_ = newRange;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getRange()
//-----------------------------------------------------------------------------
QString AddressSpace::getRange() const
{
	return range_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getWidth()
//-----------------------------------------------------------------------------
QString AddressSpace::getWidth() const
{
	return width_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setWidth()
//-----------------------------------------------------------------------------
void AddressSpace::setWidth(QString const& newWidth)
{
	width_ = newWidth;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getLocalMemoryMap()
//-----------------------------------------------------------------------------
QSharedPointer<MemoryMapBase> AddressSpace::getLocalMemoryMap()
{
	return localMemoryMap_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::hasLocalMemoryMap()
//-----------------------------------------------------------------------------
bool AddressSpace::hasLocalMemoryMap() const
{
    return localMemoryMap_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpace::setLocalMemoryMap( QSharedPointer<MemoryMapBase> localMemoryMap )
{
	if (localMemoryMap_)
    {
		localMemoryMap_.clear();
	}
	localMemoryMap_ = localMemoryMap;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Segment> > > AddressSpace::getSegments() const
{
	return segments_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setSegments()
//-----------------------------------------------------------------------------
void AddressSpace::setSegments(QSharedPointer<QList<QSharedPointer<Segment> > > segments)
{
	segments_ = segments;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > AddressSpace::getParameters() const
{
	return parameters_;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::setParameters()
//-----------------------------------------------------------------------------
void AddressSpace::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_ = newParameters;
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::copyLocalMemoryMap()
//-----------------------------------------------------------------------------
void AddressSpace::copyLocalMemoryMap(const AddressSpace& other)
{
    if (other.localMemoryMap_)
    {
        localMemoryMap_ = QSharedPointer<MemoryMapBase>(new MemoryMapBase(*other.localMemoryMap_.data()));
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::copySegments()
//-----------------------------------------------------------------------------
void AddressSpace::copySegments(const AddressSpace& other)
{
    foreach (QSharedPointer<Segment> segment, *other.segments_)
    {
        if (segment)
        {
            QSharedPointer<Segment> copy = QSharedPointer<Segment>(new Segment(*segment.data()));
            segments_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AddressSpace::copyParameters()
//-----------------------------------------------------------------------------
void AddressSpace::copyParameters(const AddressSpace& other)
{
    foreach (QSharedPointer<Parameter> parameter, *other.parameters_)
    {
        if (parameter)
        {
            QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(new Parameter(*parameter.data()));
            parameters_->append(copy);
        }
    }
}