/* 
 *  	Created on: 17.2.2012
 *      Author: Antti Kamppi
 * 		filename: segment.cpp
 *		Project: Kactus 2
 */

#include "segment.h"

#include <generaldeclarations.h>

#include "GenericVendorExtension.h"
#include "XmlUtils.h"
#include "VendorExtension.h"

#include <QStringList>

Segment::Segment( QDomNode &segmentNode ):
NameGroup(),
addressOffset_(),
offsetAttributes_(),
range_(),
rangeAttributes_(),
vendorExtensions_()
{

	for (int i = 0; i < segmentNode.childNodes().count(); ++i) {
		QDomNode tempNode = segmentNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:addressOffset")) {
			addressOffset_ = tempNode.childNodes().at(0).nodeValue();

			offsetAttributes_ = XmlUtils::parseAttributes(tempNode);
		}

		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			rangeAttributes_ = XmlUtils::parseAttributes(tempNode);
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

Segment::Segment():
NameGroup(),
addressOffset_(),
offsetAttributes_(),
range_(),
rangeAttributes_(),
vendorExtensions_()
{

}

Segment::Segment( const Segment& other ):
NameGroup(other),
addressOffset_(other.addressOffset_),
offsetAttributes_(other.offsetAttributes_),
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
vendorExtensions_(other.vendorExtensions_)
{
}

Segment::~Segment() {
}

Segment& Segment::operator=( const Segment& other ) {

	if (this != &other) {
		NameGroup::operator=(other);
		addressOffset_ = other.addressOffset_;
		offsetAttributes_ = other.offsetAttributes_;
		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;
        vendorExtensions_ = other.vendorExtensions_;
	}
	return *this;
}

void Segment::write( QXmlStreamWriter& writer ) {

	writer.writeStartElement("spirit:segment");

	if (!name().isEmpty()) {
		writer.writeTextElement("spirit:name", name());
	}

	if (!displayName().isEmpty()) {
		writer.writeTextElement("spirit:displayName", displayName());
	}

	if (!description().isEmpty()) {
		writer.writeTextElement("spirit:description", description());
	}

	if (!addressOffset_.isEmpty()) {
		writer.writeStartElement("spirit:addressOffset");
		
		XmlUtils::writeAttributes(writer, offsetAttributes_);
		writer.writeCharacters(addressOffset_);

		writer.writeEndElement(); // spirit:addressOffset
	}

	if (!range_.isEmpty()) {
		writer.writeStartElement("spirit:range");

		XmlUtils::writeAttributes(writer, rangeAttributes_);
		writer.writeCharacters(range_);

		writer.writeEndElement(); // spirit:range
	}

    if (!vendorExtensions_.isEmpty())
    {
        writer.writeStartElement("spirit:vendorExtensions");
        XmlUtils::writeVendorExtensions(writer, vendorExtensions_);
        writer.writeEndElement(); // spirit:vendorExtensions
    }

	writer.writeEndElement(); // spirit:segment
}

bool Segment::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;
	const QString thisIdentifier(QObject::tr("segment %1").arg(name()));

	if (name().isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No name set for segment within %1").arg(parentIdentifier));
	}

	if (addressOffset_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No address offset set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	if (range_.isEmpty()) {
		valid = false;
		errorList.append(QObject::tr("No range set for segment %1 within %2").arg(
			thisIdentifier).arg(parentIdentifier));
	}

	return valid;
}

bool Segment::isValid() const {
	
	if (name().isEmpty()) {
		return false;
	}
	else if (addressOffset_.isEmpty()) {
		return false;
	}
	else if (range_.isEmpty()) {
		return false;
	}

	return true;
}

QString Segment::getAddressOffset() const {
	return addressOffset_;
}

QString Segment::getRange() const {
	return range_;
}

const QMap<QString, QString>& Segment::getOffsetAttributes() const {
	return offsetAttributes_;
}

const QMap<QString, QString>& Segment::getRangeAttributes() const {
	return rangeAttributes_;
}

void Segment::setOffset( const QString& addressOffset ) {
	addressOffset_ = addressOffset;
}

void Segment::setRange( const QString& range ) {
	range_ = range;
}
