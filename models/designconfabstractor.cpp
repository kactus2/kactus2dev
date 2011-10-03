/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#include "designconfabstractor.h"
#include <LibraryManager/vlnv.h>
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QSharedPointer>
#include <QObject>
#include <QMap>

DesignConfAbstractor::DesignConfAbstractor(QDomNode& abstractorNode):
instanceName_(QString()), displayName_(QString()), description_(QString()),
abstractorRef_(), configurableElements_(), viewName_(QString()) {

	// go through all child items
	for (int i = 0; i < abstractorNode.childNodes().count(); ++i) {
		QDomNode tempNode = abstractorNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:instanceName")) {
			instanceName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:displayName")) {
			displayName_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:description")) {
			description_ = tempNode.childNodes().at(0).nodeValue();
		}

		else if (tempNode.nodeName() == QString("spirit:abstractorRef")) {
			abstractorRef_ = General::createVLNV(tempNode, VLNV::ABSTRACTOR);
		}

		else if (tempNode.nodeName() ==
				QString("spirit:configurableElementValues")) {
			for (int j = 0; j < tempNode.childNodes().count(); ++j) {
				QDomNode confElementNode = tempNode.childNodes().at(j);

				// get value of the element
				QString value = confElementNode.childNodes().at(0).nodeValue();

				// get the reference ID attribute
				QDomNamedNodeMap attributeMap = confElementNode.attributes();
				QString referenceID = attributeMap.namedItem(QString(
						"spirit:referenceId")).nodeValue();

				// if one of the mandatory elements is missing
				if (value.isNull()) {
					throw Parse_error(QObject::tr("Mandatory element spirit:"
							"configurableElementValue is missing in spirit:"
							"abstractor"));
				}

				if (referenceID.isNull()) {
					throw Parse_error(QObject::tr("Mandatory element spirit:"
							"referenceId is missing in spirit:configurable"
							"ElementValue"));
				}
				configurableElements_.insert(referenceID, value);
			}
		}

		else if (tempNode.nodeName() == QString("spirit:viewName")) {
			viewName_ = tempNode.childNodes().at(0).nodeValue();
		}
	}

	// if mandatory elements are missing
	if (instanceName_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:"
				"instanceName is missing in spirit:abstractor"));
	}

	if (!abstractorRef_.isValid()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:abstractorRef"
				" is missing in spirit:abstractor"));
	}

	if (viewName_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:viewName"
				" is missing in spirit:abstractor"));
	}
	return;
}

DesignConfAbstractor::DesignConfAbstractor( const DesignConfAbstractor& other ):
instanceName_(other.instanceName_),
displayName_(other.displayName_),
description_(other.description_),
abstractorRef_(other.abstractorRef_),
configurableElements_(other.configurableElements_),
viewName_(other.viewName_) {
}

DesignConfAbstractor& DesignConfAbstractor::operator=( const DesignConfAbstractor& other ) {
	if (this != &other) {
		instanceName_ = other.instanceName_;
		displayName_ = other.displayName_;
		description_ = other.description_;
		abstractorRef_ = other.abstractorRef_;
		configurableElements_ = other.configurableElements_;
		viewName_ = other.viewName_;
	}
	return *this;
}

// the destructor
DesignConfAbstractor::~DesignConfAbstractor() {
}

void DesignConfAbstractor::write(QXmlStreamWriter& writer) {

	writer.writeStartElement("spirit:abstractor");

	// if mandatory name missing
	if (instanceName_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element instanceName is "
				"missing in spirit:abstractor"));
	}
	else {
		writer.writeTextElement("spirit:instanceName", instanceName_);
	}

	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	// if mandatory reference to abstractor is missing
	if (!abstractorRef_.isValid()) {
		throw Write_error(QObject::tr("Mandatory element spirit:abstractorRef"
				" missing in spirit:abstractor"));
	}
	else {
		writer.writeEmptyElement("spirit:abstractorRef");
		General::writeVLNVAttributes(writer, &abstractorRef_);
	}

	// if there are any configurable elements
	if (configurableElements_.size() != 0) {
		writer.writeStartElement("spirit:configurableElementValues");

		for (QMap<QString, QString>::iterator i = configurableElements_.begin();
				i != configurableElements_.end(); ++i) {

			writer.writeStartElement("spirit:configurableElementValue");
			writer.writeAttribute("spirit:referenceId", i.key());
			writer.writeCharacters(i.value());
			writer.writeEndElement(); // spirit:configurableElementValue
		}

		writer.writeEndElement(); // spirit:configurableElementValues
	}

	if (viewName_.isEmpty()) {
		throw Write_error(QObject::tr("Mandatory element spirit:viewName "
				"missing in spirit:abstractor"));
	}
	else {
		writer.writeTextElement("spirit:viewName", viewName_);
	}

	writer.writeEndElement(); // spirit:abstractor
}

void DesignConfAbstractor::setInstanceName(const QString& instanceName) {
	instanceName_ = instanceName;
}

void DesignConfAbstractor::setConfigurableElements(
		QMap<QString,QString>& configurableElements) {

	// delete old configurable elements
	configurableElements_.clear();

	// save new elements
	configurableElements_ = configurableElements;
}

void DesignConfAbstractor::setDisplayName(const QString& displayName) {
	displayName_ = displayName;
}

QString DesignConfAbstractor::getViewName() const {
	return viewName_;
}

const QMap<QString,QString>& DesignConfAbstractor::getConfigurableElements() {
	return configurableElements_;
}

void DesignConfAbstractor::setViewName(const QString& viewName) {
	viewName_ = viewName;
}

void DesignConfAbstractor::setAbstractorRef(const VLNV& abstractorRef) {

	// save new abstractorRef
	abstractorRef_ = abstractorRef;
}

QString DesignConfAbstractor::getDisplayName() const {
	return displayName_;
}

void DesignConfAbstractor::setDescription(const QString& description) {
	description_ = description;
}

QString DesignConfAbstractor::getInstanceName() const {
	return instanceName_;
}

VLNV DesignConfAbstractor::getAbstractorRef() const {
	return abstractorRef_;
}

QString DesignConfAbstractor::getDescription() const {
	return description_;
}
