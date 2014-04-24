/* 
 *
 *  Created on: 9.8.2010
 *      Author: Antti Kamppi
 */

#include "buildcommand.h"
#include "generaldeclarations.h"
#include "buildmodel.h"
 
#include <QString>
#include <QMap>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

// the constructor
BuildCommand::BuildCommand(QDomNode &buildCommandNode):
BuildModel(buildCommandNode), targetName_(QString()),
targetNameAttributes_() {

	for (int i = 0; i < buildCommandNode.childNodes().count(); ++i) {
		QDomNode tempNode = buildCommandNode.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:targetName")) {
			targetName_ = tempNode.childNodes().at(0).nodeValue();
			General::parseAttributes(tempNode, targetNameAttributes_);
		}
	}
	return;
}

BuildCommand::BuildCommand():
BuildModel(), targetName_(),
targetNameAttributes_() {
}

BuildCommand::BuildCommand( const BuildCommand &other ):
BuildModel(other),
targetName_(other.targetName_),
targetNameAttributes_(other.targetNameAttributes_) {
}

BuildCommand & BuildCommand::operator=( const BuildCommand &other ) {
	if (this != &other) {
		BuildModel::operator=(other);

		targetName_ = other.targetName_;
		targetNameAttributes_ = other.targetNameAttributes_;
	}
	return *this;
}

// the destructor
BuildCommand::~BuildCommand() {
}

void BuildCommand::write(QXmlStreamWriter& writer) {
	// only write if there is something to write
	if (targetName_.isEmpty() && command_.isEmpty() && 
		flags_.isEmpty()) {
			return;
	}
	
	writer.writeStartElement("spirit:buildCommand");

	// call base class write() to write base class information
	BuildModel::write(writer);

	if (!targetName_.isEmpty()) {
		// start the spirit:targetName tag
		writer.writeStartElement("spirit:targetName");

		// write the attributes for the element
		General::writeAttributes(writer, targetNameAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(targetName_);
		writer.writeEndElement(); // spirit:targetName
	}

	writer.writeEndElement(); // spirit:buildCommand
}

const QMap<QString, QString>& BuildCommand::getTargetNameAttributes() {
	return targetNameAttributes_;
}

void BuildCommand::setTargetNameAttributes(
		const QMap<QString, QString> &targetNameAttributes) {
	// delete old target name attributes
	targetNameAttributes_.clear();

	// save new target name attributes
	targetNameAttributes_ = targetNameAttributes;
}

void BuildCommand::setTargetName(const QString &targetName) {
	targetName_ = targetName;
}

QString BuildCommand::getTargetName() const {
	return targetName_;
}