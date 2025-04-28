//-----------------------------------------------------------------------------
// File: FileWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Writer class for IP-XACT file element.
//-----------------------------------------------------------------------------

#include "FileWriter.h"

#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/FileType.h>

#include <QMap>


//-----------------------------------------------------------------------------
// Function: FileWriter::writeFile()
//-----------------------------------------------------------------------------
void FileWriter::writeFile(QXmlStreamWriter& writer, QSharedPointer<File> file, Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:file"));

    Details::writeAttributes(writer, file);

    writer.writeTextElement(QStringLiteral("ipxact:name"), file->name());

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsWriter::writeIsPresent(writer, file->getIsPresent());
    }

    Details::writeFileTypes(writer, file, docRevision);

    Details::writeIsStructural(writer, file);

    Details::writeIsIncludeFile(writer, file);

    Details::writeLogicalName(writer, file);

    Details::writeExportedNames(writer, file);

    Details::writeBuildCommand(writer, file);

    Details::writeDependencies(writer, file);

    Details::writeDefines(writer, file);

    Details::writeImageTypes(writer, file);

    CommonItemsWriter::writeDescription(writer, file->getDescription());

    Details::writeFileExtensions(writer, file);

    writer.writeEndElement(); // ipxact:file
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeFileType()
//-----------------------------------------------------------------------------
void FileWriter::writeFileType(QXmlStreamWriter& writer, FileType const& fileType,
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:fileType"));

    if (FileTypes::isIpXactFileType(fileType.type_, docRevision))
    {
        if (docRevision == Document::Revision::Std22 && fileType.libext_.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("libext"), fileType.libext_);
        }

        writer.writeCharacters(fileType.type_);
    }
    else
    {
        writer.writeAttribute(QStringLiteral("user"), fileType.type_);

        if (docRevision == Document::Revision::Std22 && fileType.libext_.isEmpty() == false)
        {
            writer.writeAttribute(QStringLiteral("libext"), fileType.libext_);
        }

        writer.writeCharacters(QStringLiteral("user"));
    }

    writer.writeEndElement(); // ipxact:fileType
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeAttributes()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (!file->getFileId().isEmpty())
    {
        writer.writeAttribute(QStringLiteral("fileId"), file->getFileId());
    }

    QMapIterator<QString, QString> attributeIterator(file->getAttributes());

    while (attributeIterator.hasNext())
    {
        attributeIterator.next();
        writer.writeAttribute(attributeIterator.key(), attributeIterator.value());
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeFileType()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeFileTypes(QXmlStreamWriter& writer, QSharedPointer<File> file,
    Document::Revision docRevision)
{
    for (auto const& fileType : *file->getFileTypes())
    {
        writeFileType(writer, fileType, docRevision);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeIsStructural()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeIsStructural(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (file->isStructural())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isStructural"), QStringLiteral("true"));
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeIsIncludeFile()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeIsIncludeFile(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (file->isIncludeFile())
    {
        writer.writeStartElement(QStringLiteral("ipxact:isIncludeFile"));

        if (file->hasExternalDeclarations())
        {
            writer.writeAttribute(QStringLiteral("externalDeclarations"), QStringLiteral("true"));
        }

        writer.writeCharacters(QStringLiteral("true"));

        writer.writeEndElement(); // ipxact:isIncludeFile
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeLogicalName()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeLogicalName(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (!file->getLogicalName().isEmpty())
    {
        writer.writeStartElement(QStringLiteral("ipxact:logicalName"));

        if (file->isLogicalNameDefault())
        {
            writer.writeAttribute(QStringLiteral("default"), QStringLiteral("true"));
        }

        writer.writeCharacters(file->getLogicalName());

        writer.writeEndElement(); // ipxact:logicalName
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeExportedNames()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeExportedNames(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    for (QString const& exportedName : *file->getExportedNames())
    {
        writer.writeTextElement(QStringLiteral("ipxact:exportedName"), exportedName);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeBuildCommand()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeBuildCommand(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (file->getBuildCommand())
    {
        writer.writeStartElement(QStringLiteral("ipxact:buildCommand"));

        QSharedPointer<BuildCommand> buildCommand = file->getBuildCommand();

        CommonItemsWriter::writeNonEmptyElement(writer, QStringLiteral("ipxact:command"), buildCommand->getCommand());

        if (!buildCommand->getFlags().isEmpty())
        {
            writer.writeStartElement(QStringLiteral("ipxact:flags"));

            if (!buildCommand->getFlagsAppend().isEmpty())
            {
                writer.writeAttribute(QStringLiteral("append"), buildCommand->getFlagsAppend());
            }

            writer.writeCharacters(buildCommand->getFlags());
            writer.writeEndElement(); // ipxact:flags
        }

        CommonItemsWriter::writeNonEmptyElement(writer,
            QStringLiteral("ipxact:replaceDefaultFlags"), buildCommand->getReplaceDefaultFlags());

        CommonItemsWriter::writeNonEmptyElement(writer,
            QStringLiteral("ipxact:targetName"), buildCommand->getTargetName());

        writer.writeEndElement(); // ipxact:buildCommand
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeDefines()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeDependencies(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    for (QString const& dependency : *file->getDependencies())
    {
        writer.writeTextElement(QStringLiteral("ipxact:dependency"), dependency);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeDefines()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeDefines(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    if (!file->getDefines()->isEmpty())
    {
        for (QSharedPointer<NameValuePair> define : *file->getDefines())
        {
            writer.writeStartElement(QStringLiteral("ipxact:define"));

            NameGroupWriter::writeNameGroup(writer, define);

            writer.writeTextElement(QStringLiteral("ipxact:value"), define->getValue());

            CommonItemsWriter::writeVendorExtensions(writer, define);

            writer.writeEndElement(); // ipxact:define
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeImageTypes()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeImageTypes(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    for (QString const& imageType : *file->getImageTypes())
    {
        writer.writeTextElement(QStringLiteral("ipxact:imageType"), imageType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeFileExtensions()
//-----------------------------------------------------------------------------
void FileWriter::Details::writeFileExtensions(QXmlStreamWriter& writer, QSharedPointer<File> file)
{
    CommonItemsWriter::writeVendorExtensions(writer, file);
}