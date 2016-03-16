//-----------------------------------------------------------------------------
// File: SWFileBuilder.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.10.2015
//
// Description:
// Specifies the build commands for software objects.
//-----------------------------------------------------------------------------

#include "SWFileBuilder.h"

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::SWFileBuilder()
//-----------------------------------------------------------------------------
SWFileBuilder::SWFileBuilder():
FileBuilder()
{
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::SWFileBuilder()
//-----------------------------------------------------------------------------
SWFileBuilder::SWFileBuilder(QDomNode const& swBuildNode): FileBuilder()
{
    QDomElement swBuildElement = swBuildNode.toElement();

    setFileType(swBuildElement.firstChildElement("kactus2:fileType").firstChild().nodeValue());

    setCommand(swBuildElement.firstChildElement("ipxact:command").firstChild().nodeValue());

    setFlags(swBuildElement.firstChildElement("ipxact:flags").firstChild().nodeValue());

    setReplaceDefaultFlags(
        swBuildElement.firstChildElement("ipxact:setReplaceDefaultFlags").firstChild().nodeValue());
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::SWFileBuilder()
//-----------------------------------------------------------------------------
SWFileBuilder::SWFileBuilder(SWFileBuilder const& other):
FileBuilder(other)
{

}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::clone()
//-----------------------------------------------------------------------------
VendorExtension* SWFileBuilder::clone() const
{
    return new SWFileBuilder(*this);
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::operator=()
//-----------------------------------------------------------------------------
SWFileBuilder& SWFileBuilder::operator=(SWFileBuilder const& other)
{
	if (&other != this)
    {
		FileBuilder::operator=(other);
	}

	return *this;
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::~SWFileBuilder()
//-----------------------------------------------------------------------------
SWFileBuilder::~SWFileBuilder()
{
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::write()
//-----------------------------------------------------------------------------
void SWFileBuilder::write(QXmlStreamWriter& writer) const
{
    writer.writeStartElement("kactus2:SWBuildCommand");

    writer.writeTextElement("kactus2:fileType", getFileType());

    writer.writeTextElement("ipxact:command", getCommand());

    writer.writeTextElement("ipxact:flags", getFlags());

    writer.writeTextElement("ipxact:replaceDefaultFlags", getReplaceDefaultFlags());

    writer.writeEndElement(); // kactus2:SWBuildCommand
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::isValid()
//-----------------------------------------------------------------------------
bool SWFileBuilder::isValid( QVector<QString>& errorList, const QString& parentIdentifier ) const
{
	if (getFileType().isEmpty())
    {
		errorList.append(QObject::tr("No file type specified for SW build command within %1.").arg(
			parentIdentifier));
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::isValid()
//-----------------------------------------------------------------------------
bool SWFileBuilder::isValid() const
{
	return !getFileType().isEmpty();
}

//-----------------------------------------------------------------------------
// Function: SWFileBuilder::type()
//-----------------------------------------------------------------------------
QString SWFileBuilder::type() const
{
    return QStringLiteral("kactus2:SWBuildCommand");
}
