/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "slaveinterface.h"
#include "generaldeclarations.h"

#include <IPXACTmodels/XmlUtils.h>

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QXmlStreamWriter>

// struct constructor
SlaveInterface::Bridge::Bridge(QDomNode& bridgeNode):
masterRef_(),
opaque_(false) {

	QDomNamedNodeMap attributeMap = bridgeNode.attributes();
	masterRef_ = attributeMap.namedItem(QString("spirit:masterRef")).
			nodeValue();
	masterRef_ = XmlUtils::removeWhiteSpace(masterRef_);

	QString opaque = attributeMap.namedItem(QString("spirit:opaque")).
			nodeValue();
	opaque = XmlUtils::removeWhiteSpace(opaque);

	opaque_ = General::str2Bool(opaque, false);
	return;
}

SlaveInterface::Bridge::Bridge():
masterRef_(),
opaque_(false) {
}

// struct constructor
SlaveInterface::FileSetRefGroup::FileSetRefGroup(QDomNode& fileSetNode):
group_(), 
fileSetRefs_() {

	// go through child nodes
	for (int i = 0; i < fileSetNode.childNodes().count(); ++i) {
		QDomNode tempNode = fileSetNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:group")) {

			// strip whitespace characters
			group_ = tempNode.childNodes().at(0).nodeValue();
			group_ = XmlUtils::removeWhiteSpace(group_);
		}

		else if (tempNode.nodeName() == QString("spirit:fileSetRef")) {


			// strip whitespace characters
			QString temp = tempNode.childNodes().at(0).nodeValue();
			temp = XmlUtils::removeWhiteSpace(temp);
			fileSetRefs_.append(temp);
		}
	}
	return;
}

// class constructor
SlaveInterface::SlaveInterface(QDomNode& slaveNode):
memoryMapRef_(),
bridges_(),
fileSetRefGroup_() {

	// go through all child elements
	for (int i = 0; i < slaveNode.childNodes().count(); ++i) {
		QDomNode tempNode = slaveNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:memoryMapRef")) {
			// the wanted value is an attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();

			// strip whitespace characters
			memoryMapRef_ = attributeMap.namedItem(QString(
					"spirit:memoryMapRef")).nodeValue();
			memoryMapRef_ = XmlUtils::removeWhiteSpace(memoryMapRef_);
		}

		else if (tempNode.nodeName() == QString("spirit:bridge")) {
			bridges_.append(QSharedPointer<SlaveInterface::Bridge>(
					new SlaveInterface::Bridge(tempNode)));
		}

		else if (tempNode.nodeName() == QString("spirit:fileSetRefGroup")) {
			fileSetRefGroup_.append(
					QSharedPointer<SlaveInterface::FileSetRefGroup>(
							new SlaveInterface::FileSetRefGroup(tempNode)));
		}
	}
	return;
}

SlaveInterface::SlaveInterface():
memoryMapRef_(), 
bridges_(), 
fileSetRefGroup_() {
}

SlaveInterface::SlaveInterface( const SlaveInterface& other ):
memoryMapRef_(other.memoryMapRef_),
bridges_(),
fileSetRefGroup_() {

	foreach (QSharedPointer<Bridge> bridge, other.bridges_) {
		if (bridge) {
			QSharedPointer<Bridge> copy = QSharedPointer<Bridge>(
				new Bridge(*bridge));
			bridges_.append(copy);
		}
	}

	foreach (QSharedPointer<FileSetRefGroup> refGroup, other.fileSetRefGroup_) {
		if (refGroup) {
			QSharedPointer<FileSetRefGroup> copy = QSharedPointer<FileSetRefGroup>(
				new FileSetRefGroup(*refGroup.data()));
			fileSetRefGroup_.append(copy);
		}
	}
}

SlaveInterface& SlaveInterface::operator=( const SlaveInterface& other ) {
	if (this != &other) {
		memoryMapRef_ = other.memoryMapRef_;

		bridges_.clear();
		foreach (QSharedPointer<Bridge> bridge, other.bridges_) {
			if (bridge) {
				QSharedPointer<Bridge> copy = QSharedPointer<Bridge>(
					new Bridge(*bridge));
				bridges_.append(copy);
			}
		}

		fileSetRefGroup_.clear();
		foreach (QSharedPointer<FileSetRefGroup> refGroup, other.fileSetRefGroup_) {
			if (refGroup) {
				QSharedPointer<FileSetRefGroup> copy = QSharedPointer<FileSetRefGroup>(
					new FileSetRefGroup(*refGroup.data()));
				fileSetRefGroup_.append(copy);
			}
		}
	}
	return *this;
}

SlaveInterface::~SlaveInterface() {
	bridges_.clear();
	fileSetRefGroup_.clear();
}

void SlaveInterface::write(QXmlStreamWriter& writer) {
	writer.writeStartElement("spirit:slave");

	// if optional element is not empty
	if (!memoryMapRef_.isEmpty()) {
		writer.writeEmptyElement("spirit:memoryMapRef");
		writer.writeAttribute("spirit:memoryMapRef", memoryMapRef_);
	}

	// write all spirit:bridge elements
	for (int i = 0; i < bridges_.size(); ++i) {
		writer.writeEmptyElement("spirit:bridge");
		writer.writeAttribute("spirit:masterRef", bridges_.at(i)->masterRef_);
		writer.writeAttribute("spirit:opaque", General::bool2Str(bridges_.at(i)->opaque_));
	}

	// write all filesetRefGroups
	for (int i = 0; i < fileSetRefGroup_.size(); ++i) {
		// write the name of the group
		writer.writeTextElement("spirit:group",
				fileSetRefGroup_.at(i)->group_);

		// write all the fileSetRefs for this group
		for (int i = 0; i < fileSetRefGroup_.at(i)->fileSetRefs_.size(); ++i) {
			writer.writeTextElement("spirit:fileSetRef",
					fileSetRefGroup_.at(i)->fileSetRefs_.at(i));
		}
	}
	writer.writeEndElement(); // spirit:slave
}

bool SlaveInterface::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	bool valid = true;

	foreach (QSharedPointer<Bridge> bridge, bridges_) {
		if (bridge->masterRef_.isEmpty()) {
			errorList.append(QObject::tr("Bridge did not contain a master ref"
				" within %1").arg(parentIdentifier));
			valid = false;
		}
	}

	return valid;
}

bool SlaveInterface::isValid() const {
	foreach (QSharedPointer<Bridge> bridge, bridges_) {
		if (bridge->masterRef_.isEmpty()) {
			return false;
		}
	}

	return true;
}

const QList<QSharedPointer<SlaveInterface::Bridge> >& SlaveInterface::getBridges() const {
	return bridges_;
}

QList<QSharedPointer<SlaveInterface::Bridge> >& SlaveInterface::getBridges() {
	return bridges_;
}

void SlaveInterface::setMemoryMapRef(const QString& memoryMapRef) {
	memoryMapRef_ = memoryMapRef;
}

void SlaveInterface::setFileSetRefGroup(const
		QList<QSharedPointer<FileSetRefGroup> >& fileSetRefGroup) {
	// delete old fileSetRefs
	fileSetRefGroup_.clear();

	// save new fileSetRefs
	fileSetRefGroup_ = fileSetRefGroup;
}

void SlaveInterface::setBridges(const QList<QSharedPointer<Bridge> >& bridges) {
	// delete old bridge elements
	bridges_.clear();

	// save new elements
	bridges_ = bridges;
}

const QList<QSharedPointer<SlaveInterface::FileSetRefGroup> >&
SlaveInterface::getFileSetRefGroup() {
	return fileSetRefGroup_;
}

QString SlaveInterface::getMemoryMapRef() const {
	return memoryMapRef_;
}

bool SlaveInterface::hasBridge() const {
	return !bridges_.isEmpty();
}

QStringList SlaveInterface::getMasterReferences() const {
	QStringList names;
	foreach (QSharedPointer<SlaveInterface::Bridge> bridge, bridges_) {
		names.append(bridge->masterRef_);
	}
	return names;
}
