/* 
 *
 *  Created on: 15.7.2010
 *      Author: Antti Kamppi
 */

#include "busdefinition.h"
#include "librarycomponent.h"
#include "generaldeclarations.h"
  
#include <QDomDocument>
#include <QString>
#include <QDomNamedNodeMap>
#include <QDomNode>
#include <QDomNodeList>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamWriter>

BusDefinition::BusDefinition(QDomDocument & doc): 
LibraryComponent(doc),
directConnection_(true),
isAddressable_(true), 
maxMasters_(-1),
maxSlaves_(-1),
systemGroupNames_(),
extends_(),
attributes_()  {

	LibraryComponent::vlnv_->setType(VLNV::BUSDEFINITION);

	// parse the attributes of the busDefinition
	// find the IP-Xact root element (spirit:component or spirit:design ...)
	QDomNodeList nodeList = doc.childNodes();
	int i = 0;
	// search for the first element with children (=document type)
	while (!nodeList.at(i).hasChildNodes()) {
		++i;
	}

	QDomNode busDefNode = doc.childNodes().item(i);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// get directConnection
	QDomNodeList temp = doc.elementsByTagName("spirit:directConnection");
	if (temp.size() > 0) {
		QString directConnection = temp.item(0).childNodes().item(0).nodeValue();
		directConnection_ = General::str2Bool(directConnection, true);
	}

	// get isAddressable
	QDomNodeList temp2 = doc.elementsByTagName("spirit:isAddressable");
	if (temp2.size() > 0) {
		QString isAddressable = temp2.item(0).childNodes().item(0).nodeValue();
		isAddressable_ = General::str2Bool(isAddressable, true);
	}

	// get maxMasters_
	QDomNodeList temp3 = doc.elementsByTagName("spirit:maxMasters");
	if (temp3.size() > 0) {
		maxMasters_ = temp3.item(0).childNodes().item(0).nodeValue().toInt();
	}

	// get maxSlaves
	QDomNodeList temp4 = doc.elementsByTagName("spirit:maxSlaves");
	if (temp4.size() > 0) {
		maxSlaves_ = temp4.item(0).childNodes().item(0).nodeValue().toInt();
	}

	// get systemGroupNames
	QDomNodeList temp5 = doc.elementsByTagName("spirit:systemGroupNames");
	for (int i = 0; i < temp5.item(0).childNodes().size(); ++i) {
		QString systemGName = temp5.item(0).childNodes().item(i).
				childNodes().item(0).nodeValue();
		systemGName = General::removeWhiteSpace(systemGName);
		systemGroupNames_.append(systemGName);
	}

	// get extends
	QDomNodeList temp6 = doc.elementsByTagName("spirit:extends");
	if (temp6.size() > 0) {
		QDomNode extendNode = temp6.item(0);
		extends_ = General::createVLNV(extendNode, VLNV::BUSDEFINITION);
	}

	// get vendor extensions
	QDomNodeList temp7 = doc.elementsByTagName("kactus2:kts_busdef_type");
	if (!temp7.isEmpty()) {
		QDomNode typeNode = temp7.item(0);
		kactus2Attributes_.insert("kts_busdef_type", typeNode.childNodes().at(0).nodeValue());
	}

	return;
}

// the default constructor
BusDefinition::BusDefinition(): 
LibraryComponent(), 
directConnection_(true),
isAddressable_(true),
maxMasters_(-1),
maxSlaves_(-1),
systemGroupNames_(), 
extends_(), 
attributes_() {

}

BusDefinition::BusDefinition( BusDefinition const& other ):
LibraryComponent(other),
directConnection_(other.directConnection_),
isAddressable_(other.isAddressable_),
maxMasters_(other.maxMasters_),
maxSlaves_(other.maxSlaves_),
systemGroupNames_(other.systemGroupNames_),
extends_(other.extends_),
attributes_(other.attributes_) {
}

BusDefinition & BusDefinition::operator=( BusDefinition const& other ) {
	if (this != &other) {
	
		LibraryComponent::operator=(other);

		directConnection_ = other.directConnection_;
		isAddressable_ = other.isAddressable_;
		maxMasters_ = other.maxMasters_;
		maxSlaves_ = other.maxSlaves_;
		systemGroupNames_ = other.systemGroupNames_;
		extends_ = other.extends_;
		attributes_ = other.attributes_;
	}
	return *this;
}

BusDefinition::~BusDefinition() {
	systemGroupNames_.clear();
}

QSharedPointer<LibraryComponent> BusDefinition::clone() const {
	return QSharedPointer<LibraryComponent>(new BusDefinition(*this));
}

void BusDefinition::write(QFile& file) {
	// create a writer instance and set it to operate on the given file
	QXmlStreamWriter writer(&file);

	// writer automatically adds white spaces to make document more readable
	writer.setAutoFormatting(true);
	// writer uses one tab as indentation
	writer.setAutoFormattingIndent(-1);

	// call the base class implementation to write the top comment and
	// vlvn info. It also starts the <spirit:busDefinition> element
	LibraryComponent::write(writer);

	// set the attributes
	setXMLNameSpaceAttributes(attributes_);

	// write the attributes for the spirit:busDefinition element
	General::writeAttributes(writer, attributes_);

	// call base class to write the VLNV info
	LibraryComponent::writeVLNV(writer);

	// if description is specified in the base class
	if (!LibraryComponent::description_.isEmpty()) {
		writer.writeTextElement("spirit:description", description_);
	}

	writer.writeTextElement("spirit:directConnection",
			General::bool2Str(directConnection_));

	writer.writeTextElement("spirit:isAddressable",
			General::bool2Str(isAddressable_));

	if (extends_.isValid()) {
		writer.writeEmptyElement("spirit:extends");
		General::writeVLNVAttributes(writer, &extends_);
	}

	// write only if value defined
	if (maxMasters_ > 0) {
		writer.writeTextElement("spirit:maxMasters",
				QString::number(maxMasters_));
	}

	// write only if value defined
	if (maxSlaves_ > 0) {
		writer.writeTextElement("spirit:maxSlaves",
				QString::number(maxSlaves_));
	}

	if (systemGroupNames_.size() != 0) {
		writer.writeStartElement("spirit:systemGroupNames");

		// write only the defined systemGroupnames
		for (int i = 0; i < systemGroupNames_.size(); ++i) {
			writer.writeTextElement("spirit:systemGroupName",
					systemGroupNames_.at(i));
		}

		// systemGroupNames
		writer.writeEndElement();
	}

	// if contains kactus2 attributes
	if (!kactus2Attributes_.isEmpty()) {
		writer.writeStartElement("spirit:vendorExtensions");
		writer.writeStartElement("kactus2:extensions");

		writeKactus2Attributes(writer);

		writer.writeEndElement(); // kactus2:extensions
		writer.writeEndElement(); // spirit:vendorExtensions
	}

	writer.writeEndElement(); // spirit:busDefinition
	writer.writeEndDocument();
	return;
}

bool BusDefinition::isValid( QStringList& errorList ) const {
	bool valid = true;

	if (!vlnv_) {
		errorList.append(QObject::tr("No vlnv specified for the bus definition."));
		valid = false;
	}
	else if (!vlnv_->isValid(errorList, QObject::tr("containing bus definition"))) {
		valid = false;
	}

	return valid;
}

bool BusDefinition::isValid() const {
	bool valid = true;

	if (!vlnv_) {
		valid = false;
	}
	else if (!vlnv_->isValid()) {
		valid = false;
	}

	return valid;
}

void BusDefinition::setMaxSlaves(int maxSlaves) {
	maxSlaves_ = maxSlaves;
}
void BusDefinition::setMaxMasters(int maxMasters) {
	maxMasters_ = maxMasters;
}

void BusDefinition::setIsAddressable(bool isAddressable) {
	isAddressable_ = isAddressable;
}

int BusDefinition::getMaxMasters() const {
	return maxMasters_;
}

bool BusDefinition::getIsAddressable() const {
	return isAddressable_;
}

void BusDefinition::setSystemGroupNames( const QStringList& systemGroupNames ) {
	systemGroupNames_ = systemGroupNames;
}

bool BusDefinition::getDirectConnection() const {
	return directConnection_;
}

VLNV BusDefinition::getExtends() const {
	return extends_;
}

void BusDefinition::setDirectConnection(bool directConnection) {
	directConnection_ = directConnection;
}

void BusDefinition::setExtends(const VLNV& extends) {

	extends_ = extends;
}

const QStringList& BusDefinition::getSystemGroupNames() {
	return systemGroupNames_;
}

int BusDefinition::getMaxSlaves() const {
	return maxSlaves_;
}

// BusDefinition does not have any dependency files. It only has VLNV
// dependencies.
const QStringList BusDefinition::getDependentFiles() const {
	return QStringList();
}

const QList<VLNV> BusDefinition::getDependentVLNVs() const {
	QList<VLNV> vlnvList;

	// append the vlnv of the busDefinition that is extended by this
	// busDefinition
	if (extends_.isValid()) {
		vlnvList.append(extends_);
	}
	return vlnvList;
}

void BusDefinition::setAttributes(const QMap<QString, QString>& attributes) {
	attributes_.clear();
	attributes_ = attributes;
}

const QMap<QString, QString>& BusDefinition::getAttributes() {
	return attributes_;
}

void BusDefinition::setVlnv( const VLNV& vlnv ) {
	LibraryComponent::setVlnv(vlnv);
	LibraryComponent::vlnv_->setType(VLNV::BUSDEFINITION);
}

