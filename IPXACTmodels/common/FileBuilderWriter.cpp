//-----------------------------------------------------------------------------
// File: FileBuilderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Writer class for fileBuilder element.
//-----------------------------------------------------------------------------

#include "FileBuilderWriter.h"
#include "FileBuilder.h"
#include "FileTypes.h"

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::FileBuilderWriter()
//-----------------------------------------------------------------------------
FileBuilderWriter::FileBuilderWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::~FileBuilderWriter()
//-----------------------------------------------------------------------------
FileBuilderWriter::~FileBuilderWriter()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::writeDefaultFileBuilder()
//-----------------------------------------------------------------------------
void FileBuilderWriter::writeDefaultFileBuilder(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder)
    const
{
    writer.writeStartElement(QStringLiteral("ipxact:defaultFileBuilder"));

    writeFileType(writer, fileBuilder);

    writeBuildModel(writer, fileBuilder);

    writeVendorExtensions(writer, fileBuilder);

    writer.writeEndElement(); // ipxact:defaultFileBuilder
}

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::writeFileType()
//-----------------------------------------------------------------------------
void FileBuilderWriter::writeFileType(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder) const
{
    writer.writeStartElement(QStringLiteral("ipxact:fileType"));

    if (!FileTypes::isIpXactFileType(fileBuilder->getFileType()))
    {
        writer.writeAttribute(QStringLiteral("user"), fileBuilder->getUserFileType());
    }
    writer.writeCharacters(fileBuilder->getFileType());

    writer.writeEndElement(); // ipxact:fileType
}

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::writeBuildCommand()
//-----------------------------------------------------------------------------
void FileBuilderWriter::writeBuildModel(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder) const
{
    writer.writeTextElement(QStringLiteral("ipxact:command"), fileBuilder->getCommand());

    if (!fileBuilder->getFlags().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:flags"), fileBuilder->getFlags());
    }
    if (!fileBuilder->getReplaceDefaultFlags().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:replaceDefaultFlags"), fileBuilder->getReplaceDefaultFlags());
    }
}