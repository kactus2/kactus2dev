/* 
 *	Created on:	27.3.2013
 *	Author:		Antti Kamppi
 *	File name:	bspbuildcommand.cpp
 *	Project:		Kactus 2
*/

#include "bspbuildcommand.h"


BSPBuildCommand::BSPBuildCommand( QDomNode& bspNode ):
fileType_(),
command_(),
arguments_(),
cpuName_() {

	for (int i = 0; i < bspNode.childNodes().count(); ++i) {
		QDomNode node = bspNode.childNodes().at(i);

		if (node.isComment()) {
			continue;
		}
		else if (node.nodeName() == QString("kactus2:fileType")) {
			fileType_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("spirit:command")) {
			command_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("kactus2:arguments")) {
			arguments_ = node.childNodes().at(0).nodeValue();
		}
		else if (node.nodeName() == QString("kactus2:cpuName")) {
			cpuName_ = node.childNodes().at(0).nodeValue();
		}
	}
}

BSPBuildCommand::BSPBuildCommand():
fileType_(),
	command_(),
	arguments_(),
	cpuName_() {
}

BSPBuildCommand::BSPBuildCommand( const BSPBuildCommand& other ):
fileType_(other.fileType_),
	command_(other.command_),
	arguments_(other.arguments_),
	cpuName_(other.cpuName_) {
}

BSPBuildCommand& BSPBuildCommand::operator=( const BSPBuildCommand& other ) {

	if (&other != this) {
		fileType_ = other.fileType_;
		command_ = other.command_;
		arguments_ = other.arguments_;
		cpuName_ = other.cpuName_;
	}

	return *this;
}

BSPBuildCommand::~BSPBuildCommand() {
}

void BSPBuildCommand::write( QXmlStreamWriter& writer ) {
	writer.writeStartElement("kactus2:BSPBuildCommand");

	writer.writeTextElement("kactus2:fileType", fileType_);
	writer.writeTextElement("spirit:command", command_);
	writer.writeTextElement("kactus2:arguments", arguments_);
	writer.writeTextElement("kactus2:cpuName", cpuName_);

	writer.writeEndElement(); // kactus2:BSPBuildCommand
}

bool BSPBuildCommand::isValid( const QStringList& cpuNames, QStringList& errorList, const QString& parentIdentifier ) const {
	
	// if the cpu name refers to non-existing CPU
	if (!cpuName_.isEmpty() && !cpuNames.contains(cpuName_)) {
		errorList.append(QObject::tr("Invalid reference to CPU %1 within %2").arg(
			cpuName_).arg(parentIdentifier));
		return false;
	}

	return true;
}

bool BSPBuildCommand::isValid( const QStringList& cpuNames ) const {
	
	// if the cpu name refers to non-existing CPU
	if (!cpuName_.isEmpty() && !cpuNames.contains(cpuName_)) {
		return false;
	}

	// cpu name refers to valid CPU
	return true;
}

QString BSPBuildCommand::getFileType() const {
	return fileType_;
}

void BSPBuildCommand::setFileType( const QString& fileType ) {
	fileType_ = fileType;
}

QString BSPBuildCommand::getCommand() const {
	return command_;
}

void BSPBuildCommand::setCommand( const QString& command ) {
	command_ = command;
}

QString BSPBuildCommand::getArguments() const {
	return arguments_;
}

void BSPBuildCommand::setArguments( const QString& args ) {
	arguments_ = args;
}

QString BSPBuildCommand::getCPUName() const {
	return cpuName_;
}

void BSPBuildCommand::setCPUName( const QString& cpuName ) {
	cpuName_ = cpuName;
}
