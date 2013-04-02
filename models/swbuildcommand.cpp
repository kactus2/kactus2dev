/* 
 *	Created on:	26.3.2013
 *	Author:		Antti Kamppi
 *	File name:	swbuildcommand.cpp
 *	Project:		Kactus 2
*/

#include "swbuildcommand.h"
#include <models/generaldeclarations.h>

#include <QObject>

SWBuildCommand::SWBuildCommand( QDomNode& swBuildNode ):
fileType_(),
command_(),
flags_(),
replaceDefault_(false) {

	for (int i = 0; i < swBuildNode.childNodes().count(); ++i) {
		QDomNode node = swBuildNode.childNodes().at(i);

		if (node.isComment()) {
			continue;
		}
		else if (node.nodeName() == QString("kactus2:fileType")) {
			fileType_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("spirit:command")) {
			command_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("spirit:flags")) {
			flags_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("spirit:replaceDefaultFlags")) {
			const QString value = node.childNodes().at(0).nodeValue();
			replaceDefault_ = General::str2Bool(value, false);
		}
	}
}

SWBuildCommand::SWBuildCommand():
fileType_(),
	command_(),
	flags_(),
	replaceDefault_(false) {
}

SWBuildCommand::SWBuildCommand( const SWBuildCommand& other ):
fileType_(other.fileType_),
	command_(other.command_),
	flags_(other.flags_),
	replaceDefault_(other.replaceDefault_) {
}

SWBuildCommand& SWBuildCommand::operator=( const SWBuildCommand& other ) {

	if (&other != this) {
		fileType_ = other.fileType_;
		command_ = other.command_;
		flags_ = other.flags_;
		replaceDefault_ = other.replaceDefault_;
	}

	return *this;
}

SWBuildCommand::~SWBuildCommand() {
}

void SWBuildCommand::write( QXmlStreamWriter& writer, bool withinHWComp) {
	writer.writeStartElement("kactus2:SWBuildCommand");

	writer.writeTextElement("kactus2:fileType", fileType_);

	// only HW components contain the command
	if (withinHWComp) {
		writer.writeTextElement("spirit:command", command_);
	}

	writer.writeTextElement("spirit:flags", flags_);

	writer.writeTextElement("spirit:replaceDefaultFlags", General::bool2Str(replaceDefault_));

	writer.writeEndElement(); // kactus2:SWBuildCommand
}

bool SWBuildCommand::isValid( QStringList& errorList, const QString& parentIdentifier ) const {
	if (fileType_.isEmpty()) {
		errorList.append(QObject::tr("No file type specified for SW build command within %1.").arg(
			parentIdentifier));
		return false;
	}

	return true;
}

bool SWBuildCommand::isValid() const {
	return !fileType_.isEmpty();
}

QString SWBuildCommand::getFileType() const {
	return fileType_;
}

void SWBuildCommand::setFileType( const QString& fileType ) {
	fileType_ = fileType;
}

QString SWBuildCommand::getCommand() const {
	return command_;
}

void SWBuildCommand::setCommand( const QString& command ) {
	command_ = command;
}

QString SWBuildCommand::getFlags() const {
	return flags_;
}

void SWBuildCommand::setFlags( const QString& flags ) {
	flags_ = flags;
}

bool SWBuildCommand::getReplaceDefaultFlags() const {
	return replaceDefault_;
}

void SWBuildCommand::setReplaceDefaultFlags( bool replaceDefault ) {
	replaceDefault_ = replaceDefault;
}
