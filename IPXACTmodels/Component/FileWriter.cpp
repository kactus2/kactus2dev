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
#include <IPXACTmodels/common/FileTypes.h>

#include <QMap>

//-----------------------------------------------------------------------------
// Function: FileWriter::FileWriter()
//-----------------------------------------------------------------------------
FileWriter::FileWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileWriter::~FileWriter()
//-----------------------------------------------------------------------------
FileWriter::~FileWriter()
{

}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeFile()
//-----------------------------------------------------------------------------
void FileWriter::writeFile(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    writer.writeStartElement(QStringLiteral("ipxact:file"));

    writeAttributes(writer, file);

    writer.writeTextElement(QStringLiteral("ipxact:name"), file->name());

    writeIsPresent(writer, file->getIsPresent());

    writeFileTypes(writer, file);

    writeIsStructural(writer, file);

    writeIsIncludeFile(writer, file);

    writeLogicalName(writer, file);

    writeExportedNames(writer, file);

    writeBuildCommand(writer, file);

    writeDependencies(writer, file);

    writeDefines(writer, file);

    writeImageTypes(writer, file);

    writeDescription(writer, file);

    writeFileExtensions(writer, file);

    writer.writeEndElement(); // ipxact:file
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeAttributes()
//-----------------------------------------------------------------------------
void FileWriter::writeAttributes(QXmlStreamWriter& writer, QSharedPointer<File> file) const
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
void FileWriter::writeFileTypes(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    foreach (QString fileType, *file->getFileTypes())
    {
        writer.writeStartElement(QStringLiteral("ipxact:fileType"));

        if (FileTypes::isIpXactFileType(fileType))
        {
            writer.writeCharacters(fileType);
        }
        else
        {
            writer.writeAttribute(QStringLiteral("user"), fileType);
            writer.writeCharacters(QStringLiteral("user"));
        }

        writer.writeEndElement(); // ipxact:fileType
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeIsStructural()
//-----------------------------------------------------------------------------
void FileWriter::writeIsStructural(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    if (file->isStructural())
    {
        writer.writeTextElement(QStringLiteral("ipxact:isStructural"), QStringLiteral("true"));
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeIsIncludeFile()
//-----------------------------------------------------------------------------
void FileWriter::writeIsIncludeFile(QXmlStreamWriter& writer, QSharedPointer<File> file) const
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
void FileWriter::writeLogicalName(QXmlStreamWriter& writer, QSharedPointer<File> file) const
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
void FileWriter::writeExportedNames(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    foreach (QString exportedName, *file->getExportedNames())
    {
        writer.writeTextElement(QStringLiteral("ipxact:exportedName"), exportedName);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeBuildCommand()
//-----------------------------------------------------------------------------
void FileWriter::writeBuildCommand(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    if (file->getBuildCommand())
    {
        writer.writeStartElement(QStringLiteral("ipxact:buildCommand"));

        QSharedPointer<BuildCommand> buildCommand = file->getBuildCommand();
        if (!buildCommand->getCommand().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:command"), buildCommand->getCommand());
        }
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
        if (!buildCommand->getReplaceDefaultFlags().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:replaceDefaultFlags"), buildCommand->getReplaceDefaultFlags());
        }
        if (!buildCommand->getTargetName().isEmpty())
        {
            writer.writeTextElement(QStringLiteral("ipxact:targetName"), buildCommand->getTargetName());
        }

        writer.writeEndElement(); // ipxact:buildCommand
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeDefines()
//-----------------------------------------------------------------------------
void FileWriter::writeDependencies(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    foreach (QString dependency, *file->getDependencies())
    {
        writer.writeTextElement(QStringLiteral("ipxact:dependency"), dependency);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeDefines()
//-----------------------------------------------------------------------------
void FileWriter::writeDefines(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    if (!file->getDefines()->isEmpty())
    {
        NameGroupWriter nameGroupWriter;

        foreach (QSharedPointer<NameValuePair> define, *file->getDefines())
        {
            writer.writeStartElement(QStringLiteral("ipxact:define"));

            nameGroupWriter.writeNameGroup(writer, define);

            writer.writeTextElement(QStringLiteral("ipxact:value"), define->getValue());

            writeVendorExtensions(writer, define);

            writer.writeEndElement(); // ipxact:define
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeImageTypes()
//-----------------------------------------------------------------------------
void FileWriter::writeImageTypes(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    foreach (QString imageType, *file->getImageTypes())
    {
        writer.writeTextElement(QStringLiteral("ipxact:imageType"), imageType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeDescription()
//-----------------------------------------------------------------------------
void FileWriter::writeDescription(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    if (!file->getDescription().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:description"), file->getDescription());
    }
}

//-----------------------------------------------------------------------------
// Function: FileWriter::writeFileExtensions()
//-----------------------------------------------------------------------------
void FileWriter::writeFileExtensions(QXmlStreamWriter& writer, QSharedPointer<File> file) const
{
    if (!file->getPendingHash().isEmpty())
    {
        file->setLastHash(file->getPendingHash());
    }

    writeVendorExtensions(writer, file);
}