/* 
 *
 *  Created on: 30.8.2010
 *      Author: Antti Kamppi
 */

#include "designconfabstractor.h"
#include <library/LibraryManager/vlnv.h>
#include "generaldeclarations.h"

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

				configurableElements_.insert(referenceID, value);
			}
		}

		else if (tempNode.nodeName() == QString("spirit:viewName")) {
			viewName_ = tempNode.childNodes().at(0).nodeValue();
		}
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

	writer.writeTextElement("spirit:instanceName", instanceName_);

	if (!displayName_.isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName_);
	}

	if (!description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	writer.writeEmptyElement("spirit:abstractorRef");
	General::writeVLNVAttributes(writer, &abstractorRef_);

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

	writer.writeTextElement("spirit:viewName", viewName_);

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
