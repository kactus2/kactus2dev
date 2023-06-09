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
#include "FileType.h"

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::FileBuilderWriter()
//-----------------------------------------------------------------------------
FileBuilderWriter::FileBuilderWriter() : CommonItemsWriter()
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
void FileBuilderWriter::writeDefaultFileBuilder(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder,
    Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:defaultFileBuilder"));

    writeFileType(writer, fileBuilder, docRevision);

    writeBuildModel(writer, fileBuilder);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeVendorExtensions(writer, fileBuilder);
    }

    writer.writeEndElement(); // ipxact:defaultFileBuilder
}

//-----------------------------------------------------------------------------
// Function: FileBuilderWriter::writeFileType()
//-----------------------------------------------------------------------------
void FileBuilderWriter::writeFileType(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder,
    Document::Revision docRevision) const
{
    writer.writeStartElement(QStringLiteral("ipxact:fileType"));

    if (!FileTypes::isIpXactFileType(fileBuilder->getFileType().type_, docRevision))
    {
        writer.writeAttribute(QStringLiteral("user"), fileBuilder->getFileType().type_);

        if (docRevision == Document::Revision::Std22 && fileBuilder->getFileType().libext_.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("libext"), fileBuilder->getFileType().libext_);
        }

        writer.writeCharacters(QStringLiteral("user"));
    }
    else
    {
        if (docRevision == Document::Revision::Std22 && fileBuilder->getFileType().libext_.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("libext"), fileBuilder->getFileType().libext_);
        }

        writer.writeCharacters(fileBuilder->getFileType().type_);
    }

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