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

#include <IPXACTmodels/Component/FileReader.h>

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::createDefaultFileBuilderFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FileBuilder> FileBuilderReader::createDefaultFileBuilderFrom(QDomNode const& fileBuilderNode, Document::Revision docRevision)
{
    QSharedPointer<FileBuilder> newFilebuilder (new FileBuilder());

    Details::parseFileType(fileBuilderNode, newFilebuilder, docRevision);

    Details::parseBuildModel(fileBuilderNode, newFilebuilder);

    if (docRevision == Document::Revision::Std14)
    {
        CommonItemsReader::parseVendorExtensions(fileBuilderNode, newFilebuilder);
    }

    return newFilebuilder;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseFileType()
//-----------------------------------------------------------------------------
void FileBuilderReader::Details::parseFileType(QDomNode const& filebuilderNode, QSharedPointer<FileBuilder> newFileBuilder,
    Document::Revision docRevision)
{
    QDomElement fileTypeElement = filebuilderNode.firstChildElement(QStringLiteral("ipxact:fileType"));

    newFileBuilder->setFileType(FileReader::parseFileType(fileTypeElement, docRevision));
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseBuildModel()
//-----------------------------------------------------------------------------
void FileBuilderReader::Details::parseBuildModel(QDomNode const& fileBuilderNode,
    QSharedPointer<FileBuilder> newFileBuilder)
{
    QString command = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:command")).firstChild().nodeValue();
    QString flags = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:flags")).firstChild().nodeValue();
    QString replaceFlags = fileBuilderNode.firstChildElement(QStringLiteral("ipxact:replaceDefaultFlags")).firstChild().nodeValue();

    newFileBuilder->setCommand(command);
    newFileBuilder->setFlags(flags);
    newFileBuilder->setReplaceDefaultFlags(replaceFlags);
}
