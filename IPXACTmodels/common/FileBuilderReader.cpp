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
FileBuilderReader::FileBuilderReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
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
QSharedPointer<FileBuilder> FileBuilderReader::createDefaultFileBuilderFrom(QDomNode const& fileBuilderNode) const
{
    QSharedPointer<FileBuilder> newFilebuilder (new FileBuilder());

    parseFileType(fileBuilderNode, newFilebuilder);

    parseBuildModel(fileBuilderNode, newFilebuilder);

    parseVendorExtensions(fileBuilderNode, newFilebuilder);

    return newFilebuilder;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseFileType()
//-----------------------------------------------------------------------------
void FileBuilderReader::parseFileType(QDomNode const& filebuilderNode, QSharedPointer<FileBuilder> newFileBuilder)
    const
{
    QDomElement fileTypeElement = filebuilderNode.firstChildElement("ipxact:fileType");

    QString fileType = fileTypeElement.firstChild().nodeValue();
    if (fileType == "user")
    {
        fileType = fileTypeElement.attribute("user");
    }

    newFileBuilder->setFileType(fileType);
}

//-----------------------------------------------------------------------------
// Function: FileBuilderReader::parseBuildModel()
//-----------------------------------------------------------------------------
void FileBuilderReader::parseBuildModel(QDomNode const& fileBuilderNode,
    QSharedPointer<FileBuilder> newFileBuilder) const
{
    QString command = fileBuilderNode.firstChildElement("ipxact:command").firstChild().nodeValue();
    QString flags = fileBuilderNode.firstChildElement("ipxact:flags").firstChild().nodeValue();
    QString replaceFlags = fileBuilderNode.firstChildElement("ipxact:replaceDefaultFlags").firstChild().nodeValue();

    newFileBuilder->setCommand(command);
    newFileBuilder->setFlags(flags);
    newFileBuilder->setReplaceDefaultFlags(replaceFlags);
}