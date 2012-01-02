/* 
 *
 *  Created on: 6.8.2010
 *      Author: Antti Kamppi
 */

#include "cpu.h"
#include "parameter.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

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
Cpu::Cpu() : name_(QString()), addressSpaceRefs_(), parameters_()
{
}

Cpu::Cpu(QDomNode &cpuNode): name_(QString()), addressSpaceRefs_(),
parameters_() {

	for (int i = 0; i < cpuNode.childNodes().count(); ++i) {
		QDomNode tempNode = cpuNode.childNodes().at(i);

		// don't try to parse comments
		if (tempNode.isComment()) {
			continue;
		}

		if (tempNode.nodeName() == QString("spirit:name")) {

			// strip whitespace characters
			name_ = tempNode.childNodes().at(0).nodeValue();
			name_ = General::removeWhiteSpace(name_);
		}
		else if (tempNode.nodeName() == QString("spirit:addressSpaceRef")) {

			// get the spirit:addressSpaceRef attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();
			QString str = attributeMap.namedItem(QString(
					"spirit:addressSpaceRef")).childNodes().at(0).nodeValue();

			// if the mandatory element is missing
			if (str.isNull()) {
				throw Parse_error(QObject::tr("Mandatory attribute spirit:"
						"addressSpaceRef missing in spirit:addressSpaceRef"));
			}

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
	}

	// if mandatory elements are missing

// 	if (name_.isNull()) {
// 		throw Parse_error(QObject::tr("Mandatory element name missing in "
// 				"spirit:cpu"));
// 	}
// 
// 	if (addressSpaceRefs_.size() == 0) {
// 		throw Parse_error(QObject::tr(
// 				"Mandatory element spirit:addressSpaceRef missing in "
// 				"spirit:cpu"));
// 	}
}

Cpu::Cpu( const Cpu &other ):
name_(other.name_),
addressSpaceRefs_(other.addressSpaceRefs_),
parameters_() {

	foreach (QSharedPointer<Parameter> param, other.parameters_) {
		if (param) {
			QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
				new Parameter(*param.data()));
		}
	}
}

Cpu & Cpu::operator=( const Cpu &other ) {
	if (this != &other) {
		name_ = other.name_;
		addressSpaceRefs_ = other.addressSpaceRefs_;

		parameters_.clear();
		foreach (QSharedPointer<Parameter> param, other.parameters_) {
			if (param) {
				QSharedPointer<Parameter> copy = QSharedPointer<Parameter>(
					new Parameter(*param.data()));
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

	// if mandatory name doesn't exist
	if (name_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory name missing in "
				"spirit:cpu"));
	}
	else {
		writer.writeTextElement("spirit:name", name_);
	}

	// atleast one addressSpaceRef must be found
	if (addressSpaceRefs_.size() == 0) {
		throw Write_error(QObject::tr("Mandatory addressSpaceRef missing in "
				"spirit:cpu"));
	}
	else {
		for (int i = 0; i < addressSpaceRefs_.size(); ++i) {

			// the IP-Xact specification defines this to be empty element with
			// mandatory attribute of same name. Maybe this will change in
			// future.
			writer.writeEmptyElement("spirit:addressSpaceRef");
			writer.writeAttribute("spirit:addressSpaceRef",
					addressSpaceRefs_.at(i));
		}
	}

	// write parameters if they exist
	if (parameters_.size() != 0) {
		writer.writeStartElement("spirit:parameters");

		for (int i = 0; i < parameters_.size(); ++i) {
			parameters_.at(i)->write(writer);
		}
		writer.writeEndElement(); // spirit:parameters
	}

	writer.writeEndElement(); // spirit:cpu
	return;
}

bool Cpu::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("cpu %1").arg(name_));

	if (name_.isEmpty()) {
		errorList.append(QObject::tr("No name specified for a cpu within %1").arg(
			parentIdentifier));
		valid = false;
	}

	if (addressSpaceRefs_.isEmpty()) {
		errorList.append(QObject::tr("No address space reference defined for"
			" cpu %1 within %2").arg(name_).arg(parentIdentifier));
		valid = false;
	}

	foreach (QSharedPointer<Parameter> param, parameters_) {
		if (!param->isValid(errorList, thisIdentifier)) {
			valid = false;
		}
	}

	return valid;
}

const QList<QSharedPointer<Parameter> >& Cpu::getParameters() {
	return parameters_;
}

void Cpu::setParameters(QList<QSharedPointer<Parameter> > &parameters) {
	// delete the old parameters
	parameters_.clear();

	// save the new parameteres
	parameters_ = parameters;
}

QString Cpu::getName() const {
	return name_;
}

void Cpu::setName(const QString &name) {
	name_ = name;
}

const QStringList& Cpu::getAddressSpaceRefs() {
	return addressSpaceRefs_;
}

void Cpu::setAddressSpaceRefs(QList<QString> &addressSpaceRefs) {
	// remove old addressSpaceRefs
	addressSpaceRefs_.clear();
	addressSpaceRefs_ = addressSpaceRefs;
}
