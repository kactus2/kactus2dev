/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "mirroredslaveinterface.h"
#include "generaldeclarations.h"

#include <QMap>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QXmlStreamWriter>

// struct constructor
MirroredSlaveInterface::RemapAddress::RemapAddress(QDomNode& remapNode):
remapAddress_(),
prompt_(QString("Base Address:")),
state_(),
remapAttributes_() {
	remapAddress_ = remapNode.childNodes().at(0).nodeValue();

	// get the attributes
	General::parseAttributes(remapNode, remapAttributes_);

	QMap<QString, QString>::iterator i = remapAttributes_.find(
			QString("spirit:prompt"));
	// did the prompt attribute exist
	if (i != remapAttributes_.end()) {

		// save the attribute value and remove it from QMap to avoid
		// duplicates
		prompt_ = i.value();
		remapAttributes_.erase(i);
	}
	i = remapAttributes_.find(QString("spirit:state"));
	if (i != remapAttributes_.end()) {

		// save the attribute value and remove it from QMap to avoid
		// duplicates
		state_ = i.value();
		remapAttributes_.erase(i);
	}
	return;
}

MirroredSlaveInterface::RemapAddress::RemapAddress(const QString& remapAddress):
remapAddress_(remapAddress),
prompt_(QString("Base Address:")),
state_(),
remapAttributes_() {

}

// the constructor
MirroredSlaveInterface::MirroredSlaveInterface(QDomNode& mirrorNode):
				range_(), rangeAttributes_(), remapAddresses_() {
	QDomNode baseAddrNode;
	bool baseAddressFound = false;

	// find the spirit:baseAddresses node and start the parsing from it
	for (int i = 0; i < mirrorNode.childNodes().count(); ++i) {
		if (mirrorNode.childNodes().at(i).nodeName() ==
				QString("spirit:baseAddresses")) {
			baseAddrNode = mirrorNode.childNodes().at(i);
			baseAddressFound = true;
		}
	}
	// if the optional spirit:baseAddresses node was not found theres nothing
	// to parse
	if (!baseAddressFound) {
		return;
	}

	// start parsing
	for (int i = 0; i < baseAddrNode.childNodes().count(); ++i) {
		QDomNode tempNode = baseAddrNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:remapAddress")) {
			remapAddresses_.append(
					QSharedPointer<MirroredSlaveInterface::RemapAddress>(
							new MirroredSlaveInterface::RemapAddress(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:range")) {
			range_ = tempNode.childNodes().at(0).nodeValue();

			// get attributes
			General::parseAttributes(tempNode, rangeAttributes_);
		}
	}
	return;
}

MirroredSlaveInterface::MirroredSlaveInterface():
range_(), rangeAttributes_(), remapAddresses_(){
}

MirroredSlaveInterface::MirroredSlaveInterface( const MirroredSlaveInterface& other ):
range_(other.range_),
rangeAttributes_(other.rangeAttributes_),
remapAddresses_() {

	foreach (QSharedPointer<RemapAddress> remapAddr, other.remapAddresses_) {
		if (remapAddr) {
			QSharedPointer<RemapAddress> copy = QSharedPointer<RemapAddress>(
				new RemapAddress(*remapAddr.data()));
			remapAddresses_.append(copy);
		}
	}
}


MirroredSlaveInterface& MirroredSlaveInterface::operator=( const MirroredSlaveInterface& other ) {
	if (this != &other) {

		range_ = other.range_;
		rangeAttributes_ = other.rangeAttributes_;

		remapAddresses_.clear();
		foreach (QSharedPointer<RemapAddress> remapAddr, other.remapAddresses_) {
			if (remapAddr) {
				QSharedPointer<RemapAddress> copy = QSharedPointer<RemapAddress>(
					new RemapAddress(*remapAddr.data()));
				remapAddresses_.append(copy);
			}
		}
	}
	return *this;
}


// the destructor
MirroredSlaveInterface::~MirroredSlaveInterface() {
	remapAddresses_.clear();
}

void MirroredSlaveInterface::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:mirroredSlave");

	// if theres a base address defined
	if (remapAddresses_.size() != 0) {
		writer.writeStartElement("spirit:baseAddresses");

		// write all spirit:remapAddress elements
		for (int i = 0; i < remapAddresses_.size(); ++i) {
			writer.writeStartElement("spirit:remapAddress");

			// if mandatory element is not empty
			if (!remapAddresses_.at(i)->prompt_.isEmpty()) {

				writer.writeAttribute("spirit:prompt",
						remapAddresses_.at(i)->prompt_);
			}

			// if mandatory element is not empty
			if (!remapAddresses_.at(i)->state_.isEmpty()) {
				writer.writeAttribute("spirit:state",
						remapAddresses_.at(i)->state_);
			}

			// write rest of the attributes
			General::writeAttributes(writer,
					remapAddresses_.at(i)->remapAttributes_);

			writer.writeCharacters(remapAddresses_.at(i)->remapAddress_);
			writer.writeEndElement(); // spirit:remapAddress
		}

		if (!range_.isEmpty()) {
			writer.writeStartElement("spirit:range");
			General::writeAttributes(writer, rangeAttributes_);

			writer.writeCharacters(range_);
			writer.writeEndElement(); // spirit:range
		}

		writer.writeEndElement(); // spirit:baseAddresses
	}

	writer.writeEndElement(); // spirit:mirroredSlave
}

const QMap<QString,QString>& MirroredSlaveInterface::getRangeAttributes() {
	return rangeAttributes_;
}

void MirroredSlaveInterface::setRangeAttributes(const
		QMap<QString,QString>& rangeAttributes) {
	rangeAttributes_.clear();
	rangeAttributes_ = rangeAttributes;
}

QString MirroredSlaveInterface::getRange() const {
	return range_;
}

void MirroredSlaveInterface::setRange(const QString& range) {
	range_ = range;
}

void MirroredSlaveInterface::setRemapAddresses(const
		QList<QSharedPointer<RemapAddress> >& remapAddresses) {
	// delete old remapAddresses
	remapAddresses_.clear();

	// save new remapAddresses
	remapAddresses_ = remapAddresses;
}

const QList<QSharedPointer<MirroredSlaveInterface::RemapAddress> >&
MirroredSlaveInterface::getRemapAddresses() {
	return remapAddresses_;
}

void MirroredSlaveInterface::setRemapAddress( const QString& remapAddress ) {
	// remove previous remap addresses
	remapAddresses_.clear();

	// if the remap address is empty then don't add it
	if (remapAddress.isEmpty()) {
		return;
	}

	// create new remap address for the given address
	QSharedPointer<MirroredSlaveInterface::RemapAddress> remap(new MirroredSlaveInterface::RemapAddress(remapAddress));
	remapAddresses_.append(remap);
}

QString MirroredSlaveInterface::getRemapAddress( const QString& state /*= QString()*/ ) const {
	QString remapAddress;
	foreach (QSharedPointer<MirroredSlaveInterface::RemapAddress> remap, remapAddresses_) {

		// if state is left empty the first remap address is returned
		if (state.isEmpty()) {
			remapAddress = remap->remapAddress_;
			break;
		}
		// if the remap address for the specified state was found
		else if (remap->state_ == state) {
			remapAddress = remap->remapAddress_;
			break;
		}
	}

	return remapAddress;
}
