/* 
 *
 *  Created on: 14.7.2010
 *      Author: Antti Kamppi
 */

#include "librarycomponent.h"

#include "generaldeclarations.h"
#include "GenericVendorExtension.h"

#include "vlnv.h"
#include <IPXACTmodels/kactusExtensions/KactusAttribute.h>

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
#include "XmlUtils.h"

LibraryComponent::LibraryComponent(QDomDocument &doc):
vlnv_(), 
description_(),
kactus2Attributes_(),
topComments_(),
vendorExtensions_()
{
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
        else if (children.at(i).nodeName() == QString("spirit:vendorExtensions"))
        {
            parseVendorExtensions(children.at(i));
        }
	}    
}

// the default constructor
LibraryComponent::LibraryComponent():
vlnv_(), 
description_(),
kactus2Attributes_(),
vendorExtensions_()
{
	vlnv_ = QSharedPointer<VLNV>(new VLNV());
}

LibraryComponent::LibraryComponent(const VLNV &vlnv):
vlnv_(), 
description_(),
kactus2Attributes_(),
topComments_(),
vendorExtensions_()
{
    vlnv_ = QSharedPointer<VLNV>(new VLNV(vlnv));
}

LibraryComponent::LibraryComponent( const LibraryComponent &other ):
vlnv_(),
description_(other.description_),
kactus2Attributes_(other.kactus2Attributes_),
topComments_(other.topComments_),
vendorExtensions_()
{
	if (other.vlnv_) {
		vlnv_ = QSharedPointer<VLNV>(new VLNV(*other.vlnv_));
	}
	else
    {
		vlnv_ = QSharedPointer<VLNV>(new VLNV());
    }

    copyVendorExtensions(other);
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

		topComments_ = other.topComments_;

        copyVendorExtensions(other);
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
	type = XmlUtils::removeWhiteSpace(type);

	// the vendor information
	QDomNodeList vendorList = doc.elementsByTagName(SPIRIT_VENDOR);
	QString vendor(vendorList.item(0).childNodes().item(0).nodeValue());
	vendor = XmlUtils::removeWhiteSpace(vendor);

	// the library information
	QDomNodeList libraryList = doc.elementsByTagName(SPIRIT_LIBRARY);
	QString library(libraryList.item(0).childNodes().item(0).nodeValue());
	library = XmlUtils::removeWhiteSpace(library);

	// The name information
	QDomNodeList nameList = doc.elementsByTagName(SPIRIT_NAME);
	QString name(nameList.item(0).childNodes().item(0).nodeValue());
	name = XmlUtils::removeWhiteSpace(name);

	// The version information
	QDomNodeList versionList = doc.elementsByTagName(SPIRIT_VERSION);
	QString version(versionList.item(0).childNodes().item(0).nodeValue());
	version = XmlUtils::removeWhiteSpace(version);

	// if one of the mandatory elements is missing then return invalid VLNV.
	if (type.isNull() || vendor.isNull() || library.isNull() ||	name.isNull() || version.isNull())
    {
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

	// write the top comments for the XML file
	foreach (QString comment, topComments_) {
		writer.writeComment(comment);
	}

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
    vlnv_->writeAsElements(writer);
}

void LibraryComponent::writeKactus2Attributes( QXmlStreamWriter& writer ) {
	writer.writeStartElement("kactus2:kts_attributes");
	
	// write the attribute describing the product hierarchy
	if (kactus2Attributes_.contains("kts_productHier")) {
		writer.writeTextElement("kactus2:kts_productHier", 
			kactus2Attributes_.value("kts_productHier"));
	}

	// write the implementation attribute
	if (kactus2Attributes_.contains("kts_implementation")) {
		writer.writeTextElement("kactus2:kts_implementation", kactus2Attributes_.value("kts_implementation"));
	}

	// if firmness is defined
	if (kactus2Attributes_.contains("kts_firmness")) {
		writer.writeTextElement("kactus2:kts_firmness",
			kactus2Attributes_.value("kts_firmness"));
	}

	writer.writeEndElement(); // kactus2:kts_attributes
}

void LibraryComponent::parseKactus2Attributes( QDomNode& attributeNode ) {
	
	// go through all the child nodes of the fileNode
	for (int i = 0; i < attributeNode.childNodes().count(); ++i) {
		QDomNode tempNode = attributeNode.childNodes().at(i);

		if (tempNode.nodeName() == "kactus2:kts_productHier") {
			kactus2Attributes_.insert("kts_productHier", tempNode.childNodes().at(0).nodeValue());
		}
		else if (tempNode.nodeName() == "kactus2:kts_implementation") {
			QString value = tempNode.childNodes().at(0).nodeValue();
			kactus2Attributes_.insert("kts_implementation", value);
		}
		else if (tempNode.nodeName() == "kactus2:kts_firmness") {
			kactus2Attributes_.insert("kts_firmness", tempNode.childNodes().at(0).nodeValue());
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

//-----------------------------------------------------------------------------
// Function: LibraryComponent::writeVendorExtensions()
//-----------------------------------------------------------------------------
void LibraryComponent::writeVendorExtensions(QXmlStreamWriter& writer) const
{
    XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
}

void LibraryComponent::setTopComments( const QString& comment ) {
	QStringList comments = comment.split("\n");
	topComments_ = comments;
}

//-----------------------------------------------------------------------------
// Function: LibraryComponent::setVendorExtensions()
//-----------------------------------------------------------------------------
void LibraryComponent::setVendorExtensions(QList<QSharedPointer<VendorExtension> > vendorExtensions)
{
    vendorExtensions_ = vendorExtensions;
}

//-----------------------------------------------------------------------------
// Function: LibraryComponent::getVendorExtensions()
//-----------------------------------------------------------------------------
QList<QSharedPointer<VendorExtension> > LibraryComponent::getVendorExtensions() const
{
    return vendorExtensions_;
}

void LibraryComponent::setTopComments( const QStringList& comments ) {
	topComments_ = comments;
}

QStringList LibraryComponent::getTopComments() const {
	return topComments_;
}

const QStringList LibraryComponent::getDependentDirs() const {
	return QStringList();
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::setVersion()
//-----------------------------------------------------------------------------
void LibraryComponent::setVersion(QString versionNumber)
{
    foreach(QSharedPointer<VendorExtension> extension, vendorExtensions_)
    {
        if (extension->type() == "kactus2:version")
        {
            extension.dynamicCast<Kactus2Value>()->setValue(versionNumber);
            return;
        }
    }

    vendorExtensions_.append(QSharedPointer<Kactus2Value>(new Kactus2Value("kactus2:version", versionNumber)));
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::parseVendorExtensions()
//-----------------------------------------------------------------------------
void LibraryComponent::parseVendorExtensions(QDomNode const& extensionsNode)
{
    int vendorExtensionCount = extensionsNode.childNodes().count();
    for(int index = 0; index < vendorExtensionCount; index++)
    {
        QDomNode childNode = extensionsNode.childNodes().at(index);

        if (childNode.nodeName() == QString("kactus2:version"))
        {
            setVersion(childNode.childNodes().at(0).nodeValue());
        }

        else if (!childNode.nodeName().contains("kactus2:"))
        {
            vendorExtensions_.append(QSharedPointer<VendorExtension>(new GenericVendorExtension(childNode)));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: librarycomponent::copyVendorExtensions()
//-----------------------------------------------------------------------------
void LibraryComponent::copyVendorExtensions(const LibraryComponent & other)
{
    foreach (QSharedPointer<VendorExtension> extension, other.vendorExtensions_)
    {
        vendorExtensions_.append(QSharedPointer<VendorExtension>(extension->clone()));
    }
}
