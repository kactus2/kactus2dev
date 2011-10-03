/* 
 *
 *  Created on: 31.8.2010
 *      Author: Antti Kamppi
 */

#include "slaveinterface.h"
#include "generaldeclarations.h"

#include "../exceptions/parse_error.h"
#include "../exceptions/write_error.h"

#include <QString>
#include <QList>
#include <QSharedPointer>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QObject>
#include <QXmlStreamWriter>

// struct constructor
SlaveInterface::Bridge::Bridge(QDomNode& bridgeNode): masterRef_(),
opaque_(false) {

	QDomNamedNodeMap attributeMap = bridgeNode.attributes();
	masterRef_ = attributeMap.namedItem(QString("spirit:masterRef")).
			nodeValue();
	masterRef_ = General::removeWhiteSpace(masterRef_);

	QString opaque = attributeMap.namedItem(QString("spirit:opaque")).
			nodeValue();
	opaque = General::removeWhiteSpace(opaque);

	opaque_ = General::str2Bool(opaque, false);

	// if mandatory elements are missing
	if (masterRef_.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:masterRef"
				" is missing in spirit:bridge"));
	}

	if (opaque.isNull()) {
		throw Parse_error(QObject::tr("Mandatory element spirit:opaque is "
				"missing in spirit:bridge"));
	}
	return;
}

// struct constructor
SlaveInterface::FileSetRefGroup::FileSetRefGroup(QDomNode& fileSetNode):
							group_(), fileSetRefs_() {

	// go through child nodes
	for (int i = 0; i < fileSetNode.childNodes().count(); ++i) {
		QDomNode tempNode = fileSetNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:group")) {

			// strip whitespace characters
			group_ = tempNode.childNodes().at(0).nodeValue();
			group_ = General::removeWhiteSpace(group_);
		}

		else if (tempNode.nodeName() == QString("spirit:fileSetRef")) {


			// strip whitespace characters
			QString temp = tempNode.childNodes().at(0).nodeValue();
			temp = General::removeWhiteSpace(temp);
			fileSetRefs_.append(temp);
		}
	}
	return;
}

// class constructor
SlaveInterface::SlaveInterface(QDomNode& slaveNode):
							memoryMapRef_(), bridges_(), fileSetRefGroup_() {

	// go through all child elements
	for (int i = 0; i < slaveNode.childNodes().count(); ++i) {
		QDomNode tempNode = slaveNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:memoryMapRef")) {
			// the wanted value is an attribute
			QDomNamedNodeMap attributeMap = tempNode.attributes();

			// strip whitespace characters
			memoryMapRef_ = attributeMap.namedItem(QString(
					"spirit:memoryMapRef")).nodeValue();
			memoryMapRef_ = General::removeWhiteSpace(memoryMapRef_);
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

SlaveInterface::SlaveInterface()
    : memoryMapRef_(), bridges_(), fileSetRefGroup_()
{
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

		// if mandatory element is missing
		if (bridges_.at(i)->masterRef_.isEmpty()) {
			throw Write_error(QObject::tr("Mandatory element masterRef missing"
					" in spirit:bridge"));
		}
		else {
			writer.writeAttribute("spirit:masterRef",
					bridges_.at(i)->masterRef_);
		}
		writer.writeAttribute("spirit:opaque", General::bool2Str(
				bridges_.at(i)->opaque_));
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

const QList<QSharedPointer<SlaveInterface::Bridge> >&
SlaveInterface::getBridges() {
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

	// if a bridge exists
	if (bridges_.size() != 0) {
		return true;
	}
	return false;
}