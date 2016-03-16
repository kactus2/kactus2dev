//-----------------------------------------------------------------------------
// File: BSPBuildCommand.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Kamppi
// Date: 27.03.2013
//
// Description:
// Specifies the command to build a board support package for CPU components.
//-----------------------------------------------------------------------------

#include "BSPBuildCommand.h"

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::BSPBuildCommand()
//-----------------------------------------------------------------------------
BSPBuildCommand::BSPBuildCommand(QDomNode const& bspNode):
fileType_(),
command_(),
arguments_(),
cpuName_()
{
    QDomElement bspElement = bspNode.toElement();

    fileType_ = bspElement.firstChildElement("kactus2:fileType").firstChild().nodeValue();
    command_ = bspElement.firstChildElement("ipxact:command").firstChild().nodeValue();
    arguments_ = bspElement.firstChildElement("kactus2:arguments").firstChild().nodeValue();
    cpuName_ = bspElement.firstChildElement("kactus2:cpuName").firstChild().nodeValue();
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::BSPBuildCommand()
//-----------------------------------------------------------------------------
BSPBuildCommand::BSPBuildCommand():
fileType_(),
	command_(),
	arguments_(),
	cpuName_()
{
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::BSPBuildCommand()
//-----------------------------------------------------------------------------
BSPBuildCommand::BSPBuildCommand(BSPBuildCommand const& other):
fileType_(other.fileType_),
	command_(other.command_),
	arguments_(other.arguments_),
	cpuName_(other.cpuName_)
{
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::operator=()
//-----------------------------------------------------------------------------
BSPBuildCommand& BSPBuildCommand::operator=(BSPBuildCommand const& other)
{
	if (&other != this)
    {
		fileType_ = other.fileType_;
		command_ = other.command_;
		arguments_ = other.arguments_;
		cpuName_ = other.cpuName_;
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::~BSPBuildCommand()
//-----------------------------------------------------------------------------
BSPBuildCommand::~BSPBuildCommand()
{
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::write()
//-----------------------------------------------------------------------------
void BSPBuildCommand::write(QXmlStreamWriter& writer)
{
	// if the whole element is completely empty then nothing needs to be written.
	if (isEmpty())
    {
		return;
	}

	writer.writeStartElement("kactus2:BSPBuildCommand");

	writer.writeTextElement("kactus2:fileType", fileType_);
	writer.writeTextElement("ipxact:command", command_);
	writer.writeTextElement("kactus2:arguments", arguments_);
	writer.writeTextElement("kactus2:cpuName", cpuName_);

	writer.writeEndElement(); // kactus2:BSPBuildCommand
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::isValid()
//-----------------------------------------------------------------------------
bool BSPBuildCommand::isValid(QStringList const& cpuNames, QVector<QString>& errorList,
    QString const& parentIdentifier) const
{
	
	// if the cpu name refers to non-existing CPU
	if (!cpuName_.isEmpty() && !cpuNames.contains(cpuName_))
    {
		errorList.append(QObject::tr("Invalid reference to CPU %1 within %2").arg(cpuName_, parentIdentifier));
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::isValid()
//-----------------------------------------------------------------------------
bool BSPBuildCommand::isValid(QStringList const& cpuNames ) const
{	
	// if the cpu name refers to non-existing CPU
	if (!cpuName_.isEmpty() && !cpuNames.contains(cpuName_))
    {
		return false;
	}

	// cpu name refers to valid CPU
	return true;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::isEmpty()
//-----------------------------------------------------------------------------
bool BSPBuildCommand::isEmpty() const
{
	return (fileType_.isEmpty() && command_.isEmpty() && arguments_.isEmpty() && cpuName_.isEmpty());
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::getFileType()
//-----------------------------------------------------------------------------
QString BSPBuildCommand::getFileType() const
{
	return fileType_;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::setFileType()
//-----------------------------------------------------------------------------
void BSPBuildCommand::setFileType(QString const& fileType)
{
	fileType_ = fileType;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::getCommand()
//-----------------------------------------------------------------------------
QString BSPBuildCommand::getCommand() const
{
	return command_;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::setCommand()
//-----------------------------------------------------------------------------
void BSPBuildCommand::setCommand(QString const& command)
{
	command_ = command;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::getArguments()
//-----------------------------------------------------------------------------
QString BSPBuildCommand::getArguments() const
{
	return arguments_;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::setArguments()
//-----------------------------------------------------------------------------
void BSPBuildCommand::setArguments(QString const& args)
{
	arguments_ = args;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::getCPUName()
//-----------------------------------------------------------------------------
QString BSPBuildCommand::getCPUName() const
{
	return cpuName_;
}

//-----------------------------------------------------------------------------
// Function: BSPBuildCommand::setCPUName()
//-----------------------------------------------------------------------------
void BSPBuildCommand::setCPUName(QString const& cpuName)
{
	cpuName_ = cpuName;
}
