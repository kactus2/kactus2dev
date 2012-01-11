/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#include "librarycomponent.h"
#include <LibraryManager/vlnv.h>
#include <common/KactusAttribute.h>

#include "generaldeclarations.h"


#include "../exceptions/write_error.h"

#include <QString>
#include <QDomDocument>
#include <QTextStream>
#include <QDomNode>
#include <QDomNodeList>
#include <QDomElement>
#include <QDomNamedNodeMap>
#include <QSharedPointer>
#include <QObject>
#include <QList>
#include <QStringList>
#include <QDate>
#include <QTime>
#include <QFile>
#include <QXmlStreamWriter>

#include <QDebug>

LibraryComponent::LibraryComponent(QDomDocument &doc):
vlnv_(), 
description_(),
kactus2Attributes_() {

	// get the VLNV
	vlnv_ = QSharedPointer<VLNV>(new VLNV(findVLNV(doc)));

	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNodeList children = doc.childNodes().item(i).childNodes();

	for (int i = 0; i < children.size(); ++i) {

		// get the description
		if (children.at(i).nodeName() ==
				QString("spirit:description")) {
			description_ = children.at(i).childNodes().item(0).nodeValue();
		}
	}
	return;
}

// the default constructor
LibraryComponent::LibraryComponent():
vlnv_(), 
description_(),
kactus2Attributes_() {

	vlnv_ = QSharedPointer<VLNV>(new VLNV());
}

LibraryComponent::LibraryComponent(const VLNV &vlnv):
vlnv_(), 
description_(),
kactus2Attributes_() {
    vlnv_ = QSharedPointer<VLNV>(new VLNV(vlnv));
}

LibraryComponent::LibraryComponent( const LibraryComponent &other ):
vlnv_(),
description_(other.description_),
kactus2Attributes_(other.kactus2Attributes_) {

	if (other.vlnv_) {
		vlnv_ = QSharedPointer<VLNV>(new VLNV(*other.vlnv_));
	}
	else
		vlnv_ = QSharedPointer<VLNV>(new VLNV());
}

LibraryComponent & LibraryComponent::operator=( const LibraryComponent &other ) {
	if (this != &other) {

		if (other.vlnv_) {
			vlnv_ = QSharedPointer<VLNV>(new VLNV(*other.vlnv_));
		}
		else
			vlnv_ = QSharedPointer<VLNV>(new VLNV());

		description_ = other.description_;

		kactus2Attributes_ = other.kactus2Attributes_;
	}
	return *this;
}

LibraryComponent::~LibraryComponent() {
}

VLNV LibraryComponent::findVLNV(QDomDocument &doc) {
	// first the correct element is searched for by its name
	// and then the child of the element (which is the text element that holds
	// the value of the element) is added to the struct

	// get the type of the document
	QDomNodeList nodeList = doc.childNodes();
	// search for the first element with children
	int i = 0;
	while (!nodeList.at(i).hasChildNodes()) {
		++i;

		if (i >= nodeList.size()) {
			return VLNV();
		}
	}

	QString type(nodeList.at(i).nodeName());
	type = General::removeWhiteSpace(type);

	// the vendor information
	QDomNodeList vendorList = doc.elementsByTagName(SPIRIT_VENDOR);
	QString vendor(vendorList.item(0).childNodes().item(0).nodeValue());
	vendor = General::removeWhiteSpace(vendor);

	// the library information
	QDomNodeList libraryList = doc.elementsByTagName(SPIRIT_LIBRARY);
	QString library(libraryList.item(0).childNodes().item(0).nodeValue());
	library = General::removeWhiteSpace(library);

	// The name information
	QDomNodeList nameList = doc.elementsByTagName(SPIRIT_NAME);
	QString name(nameList.item(0).childNodes().item(0).nodeValue());
	name = General::removeWhiteSpace(name);

	// The version information
	QDomNodeList versionList = doc.elementsByTagName(SPIRIT_VERSION);
	QString version(versionList.item(0).childNodes().item(0).nodeValue());
	version = General::removeWhiteSpace(version);

	// if one of the mandatory elements is missing then throw exception
	if (type.isNull() || vendor.isNull() || library.isNull() ||
			name.isNull() || version.isNull()) {
		return VLNV();
	}

	return VLNV(type, vendor, library, name, version);
}

VLNV *LibraryComponent::getVlnv() const {
	return vlnv_.data();
}

void LibraryComponent::setVlnv( const VLNV& vlnv ) {
	vlnv_.clear();
	vlnv_ = QSharedPointer<VLNV>(new VLNV(vlnv));
}

QString LibraryComponent::getDescription() const {
	return description_;
}

void LibraryComponent::setDescription(const QString & description) {
	description_ = description;
}

void LibraryComponent::write(QXmlStreamWriter& writer) {
	writer.writeStartDocument();

	// create a string to set as a comment to the top of the document
	QString topComment(QObject::tr("Created by Kactus2"));

	// comment contains the creation date and time
	topComment += QTime::currentTime().toString(QString("hh:mm:ss")) += QString(" ");
	topComment += QDate::currentDate().toString(QString("dd.MM.yyyy"));
	writer.writeComment(topComment);

	// the comment has been generated to the top of the document and we can
	// write the actual data

	// write the element that specifies the type of the document
	writer.writeStartElement(vlnv_->getTypestr());

	// dont close the elements because the child class will now write the
	// document-type specific info into document and will take care of closing
	// the open elements
	return;
}

void LibraryComponent::writeVLNV(QXmlStreamWriter& writer) {
	// write the VLNV info
	writer.writeTextElement(SPIRIT_VENDOR,
			LibraryComponent::vlnv_->getVendor());
	writer.writeTextElement(SPIRIT_LIBRARY,
			LibraryComponent::vlnv_->getLibrary());
	writer.writeTextElement(SPIRIT_NAME,
			LibraryComponent::vlnv_->getName());
	writer.writeTextElement(SPIRIT_VERSION,
			LibraryComponent::vlnv_->getVersion());
}

void LibraryComponent::writeKactus2Attributes( QXmlStreamWriter& writer ) {
	writer.writeStartElement("kactus2:kts_attributes");
	
	// write the attribute describing the product hierarchy
	if (kactus2Attributes_.contains("kts_productHier")) {
		writer.writeTextElement("kactus2:kts_productHier", 
			kactus2Attributes_.value("kts_productHier"));
	}

	// write the implementation attribute
	bool wasSoftware = false;
	if (kactus2Attributes_.contains("kts_implementation")) {

		// if this is defined as software
		if (kactus2Attributes_.value("kts_implementation") == "SW") {
			wasSoftware = true;
		}

		writer.writeTextElement("kactus2:kts_implementation", 
			kactus2Attributes_.value("kts_implementation"));
	}

	// if was software but the software type is not defined
	if (wasSoftware && !kactus2Attributes_.contains("kts_sw_type")) {
		throw Write_error(QObject::tr("Software type must be specified for software"));
	}
	// if was software and software type is defined
	else if (wasSoftware) {
		writer.writeTextElement("kactus2:kts_sw_type", kactus2Attributes_.value("kts_sw_type"));
	}

	// if firmness is defined
	if (kactus2Attributes_.contains("kts_firmness")) {
		writer.writeTextElement("kactus2:kts_firmness",
			kactus2Attributes_.value("kts_firmness"));
	}

	// if bus def type is defined
	if (kactus2Attributes_.contains("kts_busdef_type")) {
		writer.writeTextElement("kactus2:kts_busdef_type",
			kactus2Attributes_.value("kts_busdef_type"));
	}

	writer.writeEndElement(); // kactus2:kts_attributes
}

void LibraryComponent::parseKactus2Attributes( QDomNode& attributeNode ) {
	
	// used to check that if one is found then both must be found.
	bool wasSoftware = false;
	bool hadSoftwareType = false;

	// go through all the child nodes of the fileNode
	for (int i = 0; i < attributeNode.childNodes().count(); ++i) {
		QDomNode tempNode = attributeNode.childNodes().at(i);

		// temporary variables to store the attribute name and value
		QString attName;
		QString attValue;

		if (tempNode.nodeName() == "kactus2:kts_productHier") {
			kactus2Attributes_.insert("kts_productHier", tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == "kactus2:kts_implementation") {
			QString value = tempNode.childNodes().at(0).nodeValue();
			kactus2Attributes_.insert("kts_implementation", value);

			// if implementation was defined as software
			if (value == "SW")
				wasSoftware = true;
		}
		else if (tempNode.nodeName() == "kactus2:kts_sw_type") {
			kactus2Attributes_.insert("kts_sw_type", tempNode.childNodes().at(0).nodeValue());
			hadSoftwareType = true;
		}
		else if (tempNode.nodeName() == "kactus2:kts_firmness") {
			kactus2Attributes_.insert("kts_firmness", tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == "kactus2:kts_busdef_type") {
			kactus2Attributes_.insert("kts_busdef_type", tempNode.childNodes().at(0).nodeValue());
		}
	}
	return;
}

void LibraryComponent::setXMLNameSpaceAttributes( QMap<QString, QString>& attributes ) {

	attributes.clear();
	attributes.insert("xmlns:spirit", "http://www.spiritconsortium.org/XMLSchema/SPIRIT/1.5");
	attributes.insert("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	attributes.insert("xsi:schemaLocation", "http://www.spiritconsortium.org/XMLSchema/"
		"SPIRIT/1.5 http://www.spiritconsortium.org/XMLSchema/SPIRIT/1.5/index.xsd");
	attributes.insert("xmlns:kactus2", "http://funbase.cs.tut.fi/");
}
