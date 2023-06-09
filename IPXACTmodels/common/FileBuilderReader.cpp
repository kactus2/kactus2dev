//-----------------------------------------------------------------------------
// File: FileBuilderReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Reader class for file builder element.
//-----------------------------------------------------------------------------

#include "FileBuilderReader.h"

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::FileBuilderReader()
//-----------------------------------------------------------------------------
FileBuilderReader::FileBuilderReader() : CommonItemsReader()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::~FileBuilderReader()
//-----------------------------------------------------------------------------
FileBuilderReader::~FileBuilderReader()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::createDefaultFileBuilderFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FileBuilder> FileBuilderReader::createDefaultFileBuilderFrom(QDomNode const& fileBuilderNode, Document::Revision docRevision) const
{
    QSharedPointer<FileBuilder> newFilebuilder (new FileBuilder());

    parseFileType(fileBuilderNode, newFilebuilder, docRevision);

    parseBuildModel(fileBuilderNode, newFilebuilder);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsReader::parseVendorExtensions(fileBuilderNode, newFilebuilder);
    }

    return newFilebuilder;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseFileType()
//-----------------------------------------------------------------------------
void FileBuilderReader::parseFileType(QDomNode const& filebuilderNode, QSharedPointer<FileBuilder> newFileBuilder,
    Document::Revision docRevision) const
{
    QDomElement fileTypeElement = filebuilderNode.firstChildElement(QStringLiteral("ipxact:fileType"));

    QString fileType = fileTypeElement.firstChild().nodeValue();
    if (fileType == QLatin1String("user"))
    {
        fileType = fileTypeElement.attribute(QStringLiteral("user"));
    }

    QString libext;
    if (docRevision == Document::Revision::Std22)
    {
        libext = fileTypeElement.attribute(QStringLiteral("libext"));
    }

    newFileBuilder->setFileType(fileType, libext);
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseBuildModel()
//-----------------------------------------------------------------------------
void FileBuilderReader::parseBuildModel(QDomNode const& fileBuilderNode,
    QSharedPointer<FileBuilder> newFileBuilder) const
{
    QString command = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:command")).firstChild().nodeValue();
    QString flags = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:flags")).firstChild().nodeValue();
    QString replaceFlags = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:replaceDefaultFlags")).firstChild().nodeValue();

    newFileBuilder->setCommand(command);
    newFileBuilder->setFlags(flags);
    newFileBuilder->setReplaceDefaultFlags(replaceFlags);
}
