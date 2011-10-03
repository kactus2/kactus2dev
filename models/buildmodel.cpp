/* 
 *
 *  Created on: 10.8.2010
 *      Author: Antti Kamppi
 */

#include "buildmodel.h"

#include <QMap>
#include <QString>
#include <QDomNode>
#include <QDomNamedNodeMap>
#include <QXmlStreamWriter>

// the constructor
BuildModel::BuildModel(QDomNode &node): command_(),
commandAttributes_(), flags_(), flagAttributes_(),
replaceDefaultFlags_(false), defaultFlagAttributes_() {

	for (int i = 0; i < node.childNodes().count(); ++i) {
		QDomNode tempNode = node.childNodes().at(i);

		if (tempNode.nodeName() == QString("spirit:command")) {
			command_ = tempNode.childNodes().at(0).nodeValue();
			General::parseAttributes(tempNode, commandAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:flags")) {
			flags_ = tempNode.childNodes().at(0).nodeValue();
			General::parseAttributes(tempNode, flagAttributes_);
		}

		else if (tempNode.nodeName() == QString("spirit:replaceDefaultFlags")) {
			QString temp = tempNode.childNodes().at(0).nodeValue();
			replaceDefaultFlags_ = General::str2Bool(temp, false);
			General::parseAttributes(tempNode, defaultFlagAttributes_);
		}
	}
}

BuildModel::BuildModel():
command_(), commandAttributes_(), 
flags_(), flagAttributes_(),
replaceDefaultFlags_(false), defaultFlagAttributes_() {
}

BuildModel::BuildModel( const BuildModel &other ):
command_(other.command_),
commandAttributes_(other.commandAttributes_),
flags_(other.flags_),
flagAttributes_(other.flagAttributes_),
replaceDefaultFlags_(other.replaceDefaultFlags_),
defaultFlagAttributes_(other.defaultFlagAttributes_) {
}

BuildModel & BuildModel::operator=( const BuildModel &other ) {
	if (this != &other) {
		command_ = other.command_;
		commandAttributes_ = other.commandAttributes_;
		flags_ = other.flags_;
		flagAttributes_ = other.flagAttributes_;
		replaceDefaultFlags_ = other.replaceDefaultFlags_;
		defaultFlagAttributes_ = other.defaultFlagAttributes_;
	}
	return *this;
}

// the destructor
BuildModel::~BuildModel() {
}

void BuildModel::write(QXmlStreamWriter& writer) {
	if (!command_.isEmpty()) {
		// start the spirit:command tag
		writer.writeStartElement("spirit:command");

		// write the attributes for the element
		General::writeAttributes(writer, commandAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(command_);
		writer.writeEndElement(); // spirit:command
	}

	if (!flags_.isEmpty()) {
		// start the spirit:flags tag
		writer.writeStartElement("spirit:flags");

		// write the attributes for the element
		General::writeAttributes(writer, flagAttributes_);

		// write the value of the element and close the tag
		writer.writeCharacters(flags_);
		writer.writeEndElement(); // spirit:flags
	}

	// start the spirit:replaceDefaultFlags tag
	writer.writeStartElement("spirit:replaceDefaultFlags");

	// write the attributes for the element
	General::writeAttributes(writer, defaultFlagAttributes_);

	// write the value of the element and close the tag
        writer.writeCharacters(General::bool2Str(replaceDefaultFlags_));
	writer.writeEndElement(); // spirit:replaceDefaultFlags
}

void BuildModel::setCommandAttributes(
		const QMap<QString, QString> &commandAttributes) {
	// delete the old command attributes
	commandAttributes_.clear();

	// save the new command attributes
	commandAttributes_ = commandAttributes;
}

const QMap<QString, QString>& BuildModel::getFlagAttributes() {
	return flagAttributes_;
}

bool BuildModel::getReplaceDefaultFlags() const {
	return replaceDefaultFlags_;
}

void BuildModel::setReplaceDefaultFlags(bool replaceDefaultFlags) {
	replaceDefaultFlags_ = replaceDefaultFlags;
}

void BuildModel::setFlags(const QString &flags) {
	flags_ = flags;
}

void BuildModel::setCommand(const QString &command) {
	command_ = command;
}

QString BuildModel::getCommand() const {
	return command_;
}

void BuildModel::setFlagAttributes(
		const QMap<QString, QString> &flagAttributes) {
	// delete the old flag attributes
	flagAttributes_.clear();

	// save the new flag attributes
	flagAttributes_ = flagAttributes;
}

void BuildModel::setDefaultFlagAttributes(
		const QMap<QString, QString> &defaultFlagAttributes) {
	// delete old flag attributes
	defaultFlagAttributes_.clear();

	// save the new default flag attributes
	defaultFlagAttributes_ = defaultFlagAttributes;
}

QString BuildModel::getFlags() const {
	return flags_;
}

const QMap<QString, QString>& BuildModel::getDefaultFlagAttributes() {
	return defaultFlagAttributes_;
}

const QMap<QString, QString>& BuildModel::getCommandAttributes() {
	return commandAttributes_;
}
