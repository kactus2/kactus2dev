/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "cpu.h"

#include <IPXACTmodels/common/Parameter.h>
#include <IPXACTmodels/common/ParameterWriter.h>

#include <IPXACTmodels/common/ParameterReader.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
// Function: cpu::AddressSpaceRef::AddressSpaceRef()
//-----------------------------------------------------------------------------
Cpu::AddressSpaceRef::AddressSpaceRef(QString const& addressSpaceReference /* = QString() */) :
addressSpaceRef_(addressSpaceReference),
isPresent_()
{

}

//-----------------------------------------------------------------------------
// Function: cpu::getAddressSpaceRef()
//-----------------------------------------------------------------------------
QString Cpu::AddressSpaceRef::getAddressSpaceRef() const
{
    return addressSpaceRef_;
}

//-----------------------------------------------------------------------------
// Function: cpu::AddressSpaceRef::setAddressSpaceRef()
//-----------------------------------------------------------------------------
void Cpu::AddressSpaceRef::setAddressSpaceRef(QString const& newAddressSpaceRef)
{
    addressSpaceRef_ = newAddressSpaceRef;
}

//-----------------------------------------------------------------------------
// Function: cpu::AddressSpaceRef::getIsPresent()
//-----------------------------------------------------------------------------
QString Cpu::AddressSpaceRef::getIsPresent() const
{
    return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: cpu::setIsPresent()
//-----------------------------------------------------------------------------
void Cpu::AddressSpaceRef::setIsPresent(QString const& newIsPresent)
{
    isPresent_ = newIsPresent;
}

//-----------------------------------------------------------------------------
// Function: Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu(QString const& name /* = QString() */) :
NameGroup(name),
Extendable(),
isPresent_(),
addressSpaceRefs_(new QList<QSharedPointer<AddressSpaceRef> > ()),
parameters_(new QList<QSharedPointer<Parameter> > ())
{

}

//-----------------------------------------------------------------------------
// Function: cpu::Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu( const Cpu &other ):
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
// Function: cpu::operator=()
//-----------------------------------------------------------------------------
Cpu & Cpu::operator=( const Cpu &other )
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
// Function: cpu::~Cpu()
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
/*
bool Cpu::isValid(const QStringList& addrSpaceNames,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
				  QStringList& errorList, 
				  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("cpu %1").arg(name()));

	if (name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for a cpu within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		errorList.append(QObject::tr("No address space reference defined for"
			" cpu %1 within %2").arg(name()).arg(parentIdentifier));
		valid = false;
	}
	// if there are references then check that they are valid
	else {
		foreach (QString addrRef, addressSpaceRefs_) {
			if (!addrSpaceNames.contains(addrRef)) {
				errorList.append(QObject::tr("Cpu %1 contained reference to "
					"address space %2 which is not found within %3").arg(
					name()).arg(addrRef).arg(parentIdentifier));
				valid = false;
			}
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             valid = false;
//         }
// 	}

	return valid;
}
*//*
bool Cpu::isValid(const QStringList& addrSpaceNames, 
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const 
{
	if (name().isEmpty()) 
    {
		return false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		return false;
	}
	// if there are references then check that they are valid.
	else {
		foreach (QString addrRef, addressSpaceRefs_) {
			if (!addrSpaceNames.contains(addrRef)) {
				return false;
			}
		}
	}

//     ParameterValidator validator;
//     foreach (QSharedPointer<Parameter> param, parameters_)
//     {
//         if (!validator.validate(param.data(), componentChoices)) 
//         {
//             return false;
//         }
//     }

	return true;
}
*/
//-----------------------------------------------------------------------------
// Function: cpu::getParameters()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<Parameter> > > Cpu::getParameters()
{
	return parameters_;
}

//-----------------------------------------------------------------------------
// Function: cpu::setParameters()
//-----------------------------------------------------------------------------
void Cpu::setParameters(QSharedPointer<QList<QSharedPointer<Parameter> > > newParameters)
{
    parameters_->clear();
    parameters_ = newParameters;
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
void Cpu::copyParameters(const Cpu& other)
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
void Cpu::copyAddressSpaceRefs(const Cpu& other)
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