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
// Function: Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu():
NameGroup(),
addressSpaceRefs_(),
parameters_(),
vendorExtensions_()
{
}

Cpu::Cpu( const Cpu &other ):
NameGroup(other),
addressSpaceRefs_(other.addressSpaceRefs_),
parameters_(),
vendorExtensions_(other.vendorExtensions_)
{
	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy(new Parameter(*param.data()));
			parameters_.append(copy);
		}
	}
}

Cpu & Cpu::operator=( const Cpu &other ) {
	if (this != &other) {
		NameGroup::operator=(other);
		addressSpaceRefs_ = other.addressSpaceRefs_;
        vendorExtensions_ = other.vendorExtensions_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy(new Parameter(*param.data()));
				parameters_.append(copy);
			}
		}
	}
	return *this;
}

Cpu::~Cpu() {
	parameters_.clear();
}

//-----------------------------------------------------------------------------
// Function: View::getIsPresent()
//-----------------------------------------------------------------------------
QString Cpu::getIsPresent() const
{
	return isPresent_;
}

//-----------------------------------------------------------------------------
// Function: View::setIsPresent()
//-----------------------------------------------------------------------------
void Cpu::setIsPresent(QString const& newIsPresent)
{
	isPresent_ = newIsPresent;
}

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

QList<QSharedPointer<Parameter> >& Cpu::getParameters() {
	return parameters_;
}

const QList<QSharedPointer<Parameter> >& Cpu::getParameters() const {
	return parameters_;
}

void Cpu::setParameters(QList<QSharedPointer<Parameter> > &parameters) {
	// delete the old parameters
	parameters_.clear();

	// save the new parameteres
	parameters_ = parameters;
}

const QStringList& Cpu::getAddressSpaceRefs() {
	return addressSpaceRefs_;
}

void Cpu::setAddressSpaceRefs(const QStringList& addressSpaceRefs) {
	// remove old addressSpaceRefs
	addressSpaceRefs_.clear();
	addressSpaceRefs_ = addressSpaceRefs;
}
