/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "Cpu.h"

#include <IPXACTmodels/common/Parameter.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: Cpu::AddressSpaceRef::AddressSpaceRef()
//-----------------------------------------------------------------------------
Cpu::AddressSpaceRef::AddressSpaceRef(QString const& addressSpaceReference) :
addressSpaceRef_(addressSpaceReference)
{

}

//-----------------------------------------------------------------------------
// Function: Cpu::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString Cpu::AddressSpaceRef::getAddressSpaceRef() const
{
    return addressSpaceRef_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::AddressSpaceRef::setAddressSpaceRef()
//-----------------------------------------------------------------------------
void Cpu::AddressSpaceRef::setAddressSpaceRef(QString const& newAddressSpaceRef)
{
    addressSpaceRef_ = newAddressSpaceRef;
}

//-----------------------------------------------------------------------------
// Function: Cpu::AddressSpaceRef::getIsPresent()
//-----------------------------------------------------------------------------
QString Cpu::AddressSpaceRef::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::AddressSpaceRef::setIsPresent()
//-----------------------------------------------------------------------------
void Cpu::AddressSpaceRef::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Cpu::Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu(QString const& name) :
NameGroup(name),
Extendable()
{

}

//-----------------------------------------------------------------------------
// Function: Cpu::Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu(Cpu const& other):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
range_(other.range_),
width_(other.width_),
addressUnitBits_(other.addressUnitBits_),
memoryMapReference_(other.memoryMapReference_)
{
    copyParameters(other);
    copyAddressSpaceRefs(other);
    copyRegions(other);
    copyExecutableImages(other);
}

//-----------------------------------------------------------------------------
// Function: Cpu::operator=()
//-----------------------------------------------------------------------------
Cpu & Cpu::operator=(Cpu const& other)
{
	if (this != &other)
    {
		NameGroup::operator=(other);
        Extendable::operator=(other);

        range_ = other.range_;
        width_ = other.width_;
        addressUnitBits_ = other.addressUnitBits_;
        memoryMapReference_ = other.memoryMapReference_;

        addressSpaceRefs_->clear();
        copyAddressSpaceRefs(other);

        regions_->clear();
        copyRegions(other);

        executableImages_->clear();
        copyExecutableImages(other);

        parameters_->clear();
        copyParameters(other);
	}
	return *this;
}

//-----------------------------------------------------------------------------
// Function: Cpu::~Cpu()
//-----------------------------------------------------------------------------
Cpu::~Cpu()
{

}

//-----------------------------------------------------------------------------
// Function: Cpu::getIsPresent()
//-----------------------------------------------------------------------------
QString Cpu::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setIsPresent()
//-----------------------------------------------------------------------------
void Cpu::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: cpu::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > Cpu::getParameters()
{
	return parameters_;
}

//-----------------------------------------------------------------------------
// Function: cpu::getAddressSpaceRefs()
//-----------------------------------------------------------------------------
QStringList Cpu::getAddressSpaceRefs() const
{
    QStringList references;

    for (QSharedPointer<Cpu::AddressSpaceRef> addressSpaceRef : *addressSpaceRefs_)
    {
        references.append(addressSpaceRef->getAddressSpaceRef());
    }

    return references;
}

//-----------------------------------------------------------------------------
// Function: cpu::getAddressSpaceReferences()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Cpu::AddressSpaceRef> > > Cpu::getAddressSpaceReferences() const
{
    return addressSpaceRefs_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setAddressSpaceRefs()
//-----------------------------------------------------------------------------
void Cpu::setAddressSpaceRefs(QStringList addrSpaceNames)
{
	addressSpaceRefs_->clear();
    for (QString const& addressSpaceName : addrSpaceNames)
    {
        QSharedPointer<AddressSpaceRef> addressSpaceRef(new AddressSpaceRef(addressSpaceName));
        addressSpaceRefs_->append(addressSpaceRef);
    }
}

//-----------------------------------------------------------------------------
// Function: cpu::setAddressSpaceRefs()
//-----------------------------------------------------------------------------
void Cpu::setAddressSpaceReferences(QSharedPointer<QList<QSharedPointer<AddressSpaceRef> > > newAddressSpaceRefs)
{
	addressSpaceRefs_->clear();
	addressSpaceRefs_ = newAddressSpaceRefs;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getRange()
//-----------------------------------------------------------------------------
QString Cpu::getRange() const
{
    return range_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setRange()
//-----------------------------------------------------------------------------
void Cpu::setRange(QString const& range)
{
    range_ = range;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getWidth()
//-----------------------------------------------------------------------------
QString Cpu::getWidth() const
{
    return width_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setWidth()
//-----------------------------------------------------------------------------
void Cpu::setWidth(QString const& width)
{
    width_ = width;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getMemoryMapReference()
//-----------------------------------------------------------------------------
QString Cpu::getMemoryMapReference() const
{
    return memoryMapReference_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setMemoryMapReference()
//-----------------------------------------------------------------------------
void Cpu::setMemoryMapReference(QString const& memoryMapRef)
{
    memoryMapReference_ = memoryMapRef;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getRegions()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Region> > > Cpu::getRegions() const
{
    return regions_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getAddressUnitBits()
//-----------------------------------------------------------------------------
QString Cpu::getAddressUnitBits(bool defaultIfEmpty /*= true*/) const
{
    if (addressUnitBits_.isEmpty() && defaultIfEmpty)
    {
        return QStringLiteral("8");
    }

    return addressUnitBits_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::getExecutableImages()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<ExecutableImage> > > Cpu::getExecutableImages()
{
    return executableImages_;
}

//-----------------------------------------------------------------------------
// Function: Cpu::setAddressUnitBits()
//-----------------------------------------------------------------------------
void Cpu::setAddressUnitBits(QString const& addressUnitBits)
{
    addressUnitBits_ = addressUnitBits;
}

//-----------------------------------------------------------------------------
// Function: cpu::copyParameters()
//-----------------------------------------------------------------------------
void Cpu::copyParameters(Cpu const& other)
{
    for (QSharedPointer<Parameter> param : *other.parameters_)
    {
        QSharedPointer<Parameter> copy(new Parameter(*param));
        parameters_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: cpu::copyAddressSpaceRefs()
//-----------------------------------------------------------------------------
void Cpu::copyAddressSpaceRefs(Cpu const& other)
{
    for (QSharedPointer<Cpu::AddressSpaceRef> reference : *other.addressSpaceRefs_)
    {
        QSharedPointer<Cpu::AddressSpaceRef> copy(new AddressSpaceRef(*reference));
        addressSpaceRefs_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: Cpu::copyRegions()
//-----------------------------------------------------------------------------
void Cpu::copyRegions(Cpu const& other)
{
    for (QSharedPointer<Region> region: *other.regions_)
    {
        QSharedPointer<Region> copy(new Region(*region));
        regions_->append(copy);
    }
}

//-----------------------------------------------------------------------------
// Function: Cpu::copyExecutableImages()
//-----------------------------------------------------------------------------
void Cpu::copyExecutableImages(Cpu const& other)
{
    for (QSharedPointer<ExecutableImage> image : *other.executableImages_)
    {
        QSharedPointer<ExecutableImage> copy(new ExecutableImage(*image));
        executableImages_->append(copy);
    }
}
