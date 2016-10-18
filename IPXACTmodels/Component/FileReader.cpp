//-----------------------------------------------------------------------------
// File: FileReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Reader class for IP-XACT file element.
//-----------------------------------------------------------------------------

#include "FileReader.h"

#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: FileReader::FileReader()
//-----------------------------------------------------------------------------
FileReader::FileReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileReader::~FileReader()
//-----------------------------------------------------------------------------
FileReader::~FileReader()
{

}

//-----------------------------------------------------------------------------
// Function: FileReader::createFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileReader::createFileFrom(QDomNode const& fileNode) const
{
    QDomElement fileElement = fileNode.toElement();
    QString fileName = fileNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();

    QSharedPointer<File> newFile (new File(fileName));

    parseFileAttributes(fileElement, newFile);

    newFile->setIsPresent(parseIsPresent(fileNode.firstChildElement(QStringLiteral("ipxact:isPresent"))));

    parseFileTypes(fileElement, newFile);

    parseIsStructural(fileElement, newFile);

    parseIsIncludeFile(fileElement, newFile);

    parseLogicalName(fileElement, newFile);

    parseExportedNames(fileElement, newFile);

    parseBuildCommand(fileElement, newFile);

    parseDependencies(fileElement, newFile);

    parseDefines(fileElement, newFile);

    parseImageTypes(fileElement, newFile);

    parseDescription(fileElement, newFile);

    parseFileExtensions(fileElement, newFile);

    return newFile;
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseFileAttributes()
//-----------------------------------------------------------------------------
void FileReader::parseFileAttributes(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNamedNodeMap fileAttributes = fileElement.attributes();

    if (!fileAttributes.isEmpty())
    {
        QMap<QString, QString> otherAttributes;

        for (int attributeIndex = 0; attributeIndex < fileAttributes.count(); ++attributeIndex)
        {
            QDomNode attributeNode = fileAttributes.item(attributeIndex);
            QString attributeKey = attributeNode.nodeName();
            QString attributeValue = attributeNode.firstChild().nodeValue();

            if (attributeKey == QLatin1String("fileId"))
            {
                QString fileId = attributeNode.firstChild().nodeValue();
                newFile->setFileId(fileId);
            }
            else
            {
                otherAttributes.insert(attributeKey, attributeValue);
            }
        }

        if (!otherAttributes.isEmpty())
        {
            newFile->setAttributes(otherAttributes);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseFileTypes()
//-----------------------------------------------------------------------------
void FileReader::parseFileTypes(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNodeList fileTypeNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:fileType"));
    
    for (int fileTypeIndex = 0; fileTypeIndex < fileTypeNodeList.count(); ++fileTypeIndex)
    {
        QDomNode fileTypeNode = fileTypeNodeList.at(fileTypeIndex);
        QString fileType = fileTypeNode.firstChild().nodeValue();

        if (fileType == QLatin1String("user"))
        {
            QDomNamedNodeMap fileTypeAttributes = fileTypeNode.attributes();
            fileType = fileTypeAttributes.namedItem(QStringLiteral("user")).nodeValue();
        }

        newFile->getFileTypes()->append(fileType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseIsStructural()
//-----------------------------------------------------------------------------
void FileReader::parseIsStructural(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomElement structuralElement = fileElement.firstChildElement(QStringLiteral("ipxact:isStructural"));

    if (!structuralElement.isNull())
    {
        newFile->setStructural(true);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseIsIncludeFile()
//-----------------------------------------------------------------------------
void FileReader::parseIsIncludeFile(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomElement includeElement = fileElement.firstChildElement(QStringLiteral("ipxact:isIncludeFile"));

    if (!includeElement.isNull())
    {
        newFile->setIncludeFile(true);

        if (!includeElement.attribute(QStringLiteral("externalDeclarations")).isNull())
        {
            newFile->setExternalDeclarations(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseLogicalName()
//-----------------------------------------------------------------------------
void FileReader::parseLogicalName(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomElement logicalNameElement = fileElement.firstChildElement(QStringLiteral("ipxact:logicalName"));

    if (!logicalNameElement.isNull())
    {
        QString logicalName = logicalNameElement.firstChild().nodeValue();
        newFile->setLogicalName(logicalName);

        if (!logicalNameElement.attribute(QStringLiteral("default")).isNull())
        {
            newFile->setLogicalNameDefault(true);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseExportedNames()
//-----------------------------------------------------------------------------
void FileReader::parseExportedNames(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNodeList exportedNameNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:exportedName"));

    for (int exportedIndex = 0; exportedIndex < exportedNameNodeList.count(); ++exportedIndex)
    {
        QString exportedName = exportedNameNodeList.at(exportedIndex).firstChild().nodeValue();
        newFile->getExportedNames()->append(exportedName);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseBuildCommand()
//-----------------------------------------------------------------------------
void FileReader::parseBuildCommand(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomElement buildCommandElement = fileElement.firstChildElement(QStringLiteral("ipxact:buildCommand"));

    if (!buildCommandElement.isNull())
    {
        QSharedPointer<BuildCommand> newBuildCommand (new BuildCommand());

        parseBuildCommandCommand(buildCommandElement, newBuildCommand);
        
        parseBuildCommandFlags(buildCommandElement, newBuildCommand);

        parseBuildCommandReplaceFlags(buildCommandElement, newBuildCommand);

        parseBuildCommandReplaceFlags(buildCommandElement, newBuildCommand);

        parseBuildCommandTarget(buildCommandElement, newBuildCommand);

        newFile->setBuildcommand(newBuildCommand);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseBuildCommandCommand()
//-----------------------------------------------------------------------------
void FileReader::parseBuildCommandCommand(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand) const
{
    QDomElement commandElement = buildCommandElement.firstChildElement(QStringLiteral("ipxact:command"));
    if (!commandElement.isNull())
    {
        newBuildCommand->setCommand(commandElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseBuildCommandFlags()
//-----------------------------------------------------------------------------
void FileReader::parseBuildCommandFlags(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand) const
{
    QDomElement flagsElement = buildCommandElement.firstChildElement(QStringLiteral("ipxact:flags"));
    if (!flagsElement.isNull())
    {
        newBuildCommand->setFlags(flagsElement.firstChild().nodeValue());
        if (!flagsElement.attribute(QStringLiteral("append")).isNull())
        {
            if (flagsElement.attribute(QStringLiteral("append")) == QLatin1String("true"))
            {
                newBuildCommand->setFlagsAppend(true);
            }
            else
            {
                newBuildCommand->setFlagsAppend(false);
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseBuildCommandReplaceFlags()
//-----------------------------------------------------------------------------
void FileReader::parseBuildCommandReplaceFlags(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand) const
{
    QDomElement replaceFlagsElement = buildCommandElement.firstChildElement(QStringLiteral("ipxact:replaceDefaultFlags"));
    if (!replaceFlagsElement.isNull())
    {
        newBuildCommand->setReplaceDefaultFlags(replaceFlagsElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseBuildCommandTargets()
//-----------------------------------------------------------------------------
void FileReader::parseBuildCommandTarget(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand) const
{
    QDomElement targetElement = buildCommandElement.firstChildElement(QStringLiteral("ipxact:targetName"));
    if (!targetElement.isNull())
    {
        newBuildCommand->setTargetName(targetElement.firstChild().nodeValue());
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseDependencies()
//-----------------------------------------------------------------------------
void FileReader::parseDependencies(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNodeList dependencyNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:dependency"));

    for (int dependencyIndex = 0; dependencyIndex < dependencyNodeList.count(); ++dependencyIndex)
    {
        QString dependency = dependencyNodeList.at(dependencyIndex).firstChild().nodeValue();
        newFile->getDependencies()->append(dependency);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseDefines()
//-----------------------------------------------------------------------------
void FileReader::parseDefines(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNodeList defineNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:define"));

    if (!defineNodeList.isEmpty())
    {
        NameGroupReader nameReader;
        
        for (int defineIndex = 0; defineIndex < defineNodeList.count(); ++defineIndex)
        {
            QDomNode defineNode = defineNodeList.at(defineIndex);

            QSharedPointer<NameValuePair> newDefine (new NameValuePair());

            nameReader.parseNameGroup(defineNode, newDefine);

            QString value = defineNode.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
            newDefine->setValue(value);

            parseVendorExtensions(defineNode, newDefine);

            newFile->getDefines()->append(newDefine);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseImageTypes()
//-----------------------------------------------------------------------------
void FileReader::parseImageTypes(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomNodeList imagetTypeNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:imageType"));

    for (int imageTypeIndex = 0; imageTypeIndex < imagetTypeNodeList.count(); ++imageTypeIndex)
    {
        QString imageType = imagetTypeNodeList.at(imageTypeIndex).firstChild().nodeValue();
        newFile->getImageTypes()->append(imageType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseDescription()
//-----------------------------------------------------------------------------
void FileReader::parseDescription(QDomElement const& fileElement, QSharedPointer<File> newFile) const
{
    QDomElement descriptionNameElement = fileElement.firstChildElement(QStringLiteral("ipxact:description"));

    if (!descriptionNameElement.isNull())
    {
        QString description = descriptionNameElement.firstChild().nodeValue();
        newFile->setDescription(description);
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseFileExtensions()
//-----------------------------------------------------------------------------
void FileReader::parseFileExtensions(QDomNode const& fileNode, QSharedPointer<File> newFile) const
{
    QDomElement extensionsElement = fileNode.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));

    QDomElement hashExtension = extensionsElement.firstChildElement(QStringLiteral("kactus2:hash"));
    if (!hashExtension.isNull())
    {
        QString hash = hashExtension.firstChild().nodeValue();
        newFile->setLastHash(hash);
    }

    parseVendorExtensions(fileNode, newFile);
}