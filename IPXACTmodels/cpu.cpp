/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "cpu.h"

#include "parameter.h"
#include "GenericVendorExtension.h"

#include <IPXACTmodels/validators/ParameterValidator.h>

#include <QDomNode>
#include <QString>
#include <QList>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QSharedPointer>
#include <QXmlStreamWriter>
#include "XmlUtils.h"

//-----------------------------------------------------------------------------
// Function: Cpu()
//-----------------------------------------------------------------------------
Cpu::Cpu():
nameGroup_(),
addressSpaceRefs_(),
parameters_(),
vendorExtensions_()
{
}

Cpu::Cpu(QDomNode &cpuNode): 
nameGroup_(cpuNode), 
addressSpaceRefs_(),
parameters_(),
vendorExtensions_()
 {

	for (int i = 0; i < cpuNode.childNodes().count(); ++i) {
		QDomNode tempNode = cpuNode.childNodes().at(i);

		// don't try to parse comments
		if (tempNode.isComment()) {
			continue;
		}

		if (tempNode.nodeName() == QString("spirit:addressSpaceRef")) {

			// get the spirit:addressSpaceRef attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString str = attributeMap.namedItem(QString(
					"spirit:addressSpaceRef")).childNodes().at(0).nodeValue();

			// all was fine and attribute can be added
			addressSpaceRefs_.append(str);
		}
		else if (tempNode.nodeName() == QString("spirit:parameters")) {

			// go through all parameters
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {

				QDomNode parameterNode = tempNode.childNodes().at(j);

				// dont parse comments
				if (!parameterNode.isComment()) {
					parameters_.append(QSharedPointer<Parameter>(
							new Parameter(parameterNode)));
				}
			}
		}
        else if (tempNode.nodeName() == QString("spirit:vendorExtensions")) 
        {
            int extensionCount = tempNode.childNodes().count();
            for (int j = 0; j < extensionCount; ++j) {
                QDomNode extensionNode = tempNode.childNodes().at(j);

                vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(extensionNode)));
            }
        }
	}
}

Cpu::Cpu( const Cpu &other ):
nameGroup_(other.nameGroup_),
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
		nameGroup_ = other.nameGroup_;
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

void Cpu::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:cpu");

	writer.writeTextElement("spirit:name", nameGroup_.name());

	if (!nameGroup_.displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", nameGroup_.displayName());
	}

	if (!nameGroup_.description().isEmpty()) {
		writer.writeTextElement("spirit:description", nameGroup_.description());
	}

    for (int i = 0; i < addressSpaceRefs_.size(); ++i) {

        // the IP-Xact specification defines this to be empty element with
        // mandatory attribute of same name. Maybe this will change in
        // future.
        writer.writeEmptyElement("spirit:addressSpaceRef");
        writer.writeAttribute("spirit:addressSpaceRef",
            addressSpaceRefs_.at(i));
    }

	// write parameters if they exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:parameters
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:cpu
	return;
}

bool Cpu::isValid(const QStringList& addrSpaceNames,
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices,
				  QStringList& errorList, 
				  const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("cpu %1").arg(nameGroup_.name()));

	if (nameGroup_.name().isEmpty()) {
		errorList.append(QObject::tr("No name specified for a cpu within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		errorList.append(QObject::tr("No address space reference defined for"
			" cpu %1 within %2").arg(nameGroup_.name()).arg(parentIdentifier));
		valid = false;
	}
	// if there are references then check that they are valid
	else {
		foreach (QString addrRef, addressSpaceRefs_) {
			if (!addrSpaceNames.contains(addrRef)) {
				errorList.append(QObject::tr("Cpu %1 contained reference to "
					"address space %2 which is not found within %3").arg(
					nameGroup_.name()).arg(addrRef).arg(parentIdentifier));
				valid = false;
			}
		}
	}

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        errorList.append(validator.findErrorsIn(param.data(), thisIdentifier, componentChoices));
        if (!validator.validate(param.data(), componentChoices)) 
        {
            valid = false;
        }
	}

	return valid;
}

bool Cpu::isValid(const QStringList& addrSpaceNames, 
    QSharedPointer<QList<QSharedPointer<Choice> > > componentChoices) const 
{
	if (nameGroup_.name().isEmpty()) 
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

    ParameterValidator validator;
    foreach (QSharedPointer<Parameter> param, parameters_)
    {
        if (!validator.validate(param.data(), componentChoices)) 
        {
            return false;
        }
    }

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

QString Cpu::getName() const {
	return nameGroup_.name();
}

void Cpu::setName(const QString &name) {
	nameGroup_.setName(name);
}

const QStringList& Cpu::getAddressSpaceRefs() {
	return addressSpaceRefs_;
}

void Cpu::setAddressSpaceRefs(const QStringList& addressSpaceRefs) {
	// remove old addressSpaceRefs
	addressSpaceRefs_.clear();
	addressSpaceRefs_ = addressSpaceRefs;
}

QString Cpu::getDisplayName() const {
	return nameGroup_.displayName();
}

QString Cpu::getDescription() const {
	return nameGroup_.description();
}

void Cpu::setDisplayName( const QString& displayName ) {
	nameGroup_.setDisplayName(displayName);
}

void Cpu::setDescription( const QString& description ) {
	nameGroup_.setDescription(description);
}
