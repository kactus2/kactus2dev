/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "Cpu.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/common/ParameterReader.h>

#include <QString>
#include <QList>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
// Function: Cpu::AddressSpaceRef::AddressSpaceRef()
//-----------------------------------------------------------------------------
Cpu::AddressSpaceRef::AddressSpaceRef(QString const& addressSpaceReference) :
addressSpaceRef_(addressSpaceReference),
isPresent_()
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
Extendable(),
isPresent_(),
addressSpaceRefs_(new QList<QSharedPointer<AddressSpaceRef> > ()),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: Cpu::Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu(Cpu const& other):
NameGroup(other),
Extendable(other),
isPresent_(other.isPresent_),
addressSpaceRefs_(new QList<QSharedPointer<AddressSpaceRef> > ()),
parameters_(new QList<QSharedPointer<Parameter> > ())
{
    copyParameters(other);
    copyAddressSpaceRefs(other);
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

        addressSpaceRefs_->clear();
        copyAddressSpaceRefs(other);

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
    addressSpaceRefs_.clear();
	parameters_.clear();
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

    foreach (QSharedPointer<Cpu::AddressSpaceRef> addressSpaceRef, *addressSpaceRefs_)
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
    foreach (QString const& addressSpaceName, addrSpaceNames)
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
// Function: cpu::copyParameters()
//-----------------------------------------------------------------------------
void Cpu::copyParameters(Cpu const& other)
{
    foreach (QSharedPointer<Parameter> param, *other.parameters_)
    {
        if (param)
        {
            QSharedPointer<Parameter> copy(new Parameter(*param.data()));
            parameters_->append(copy);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: cpu::copyAddressSpaceRefs()
//-----------------------------------------------------------------------------
void Cpu::copyAddressSpaceRefs(Cpu const& other)
{
    foreach (QSharedPointer<Cpu::AddressSpaceRef> reference, *other.addressSpaceRefs_)
    {
        if (reference)
        {
            QSharedPointer<Cpu::AddressSpaceRef> copy (new AddressSpaceRef(*reference.data()));
            addressSpaceRefs_->append(copy);
        }
    }
}
