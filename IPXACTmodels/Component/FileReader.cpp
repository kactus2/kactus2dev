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

#include <IPXACTmodels/common/CommonItemsReader.h>
#include <IPXACTmodels/common/NameGroupReader.h>

//-----------------------------------------------------------------------------
// Function: FileReader::createFileFrom()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileReader::createFileFrom(QDomNode const& fileNode, Document::Revision docRevision)
{
    QDomElement fileElement = fileNode.toElement();
    QString fileName = fileNode.firstChildElement(QStringLiteral("ipxact:name")).firstChild().nodeValue();

    QSharedPointer<File> newFile (new File(fileName));

    Details::parseFileAttributes(fileElement, newFile);

    if (docRevision == Document::Revision::Std14)
    {
        auto isPresent = CommonItemsReader::parseIsPresent(fileNode.firstChildElement(QStringLiteral("ipxact:isPresent")));
        newFile->setIsPresent(isPresent);
    }

    Details::parseFileTypes(fileElement, newFile, docRevision);

    Details::parseIsStructural(fileElement, newFile);

    Details::parseIsIncludeFile(fileElement, newFile);

    Details::parseLogicalName(fileElement, newFile);

    Details::parseExportedNames(fileElement, newFile);

    Details::parseBuildCommand(fileElement, newFile);

    Details::parseDependencies(fileElement, newFile);

    Details::parseDefines(fileElement, newFile);

    Details::parseImageTypes(fileElement, newFile);

    Details::parseDescription(fileElement, newFile);

    Details::parseFileExtensions(fileElement, newFile);

    return newFile;
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseFileType()
//-----------------------------------------------------------------------------
FileType FileReader::parseFileType(QDomNode const& fileTypeNode, Document::Revision docRevision)
{
    QString fileType = fileTypeNode.firstChild().nodeValue();

    QDomNamedNodeMap fileTypeAttributes = fileTypeNode.attributes();
    if (fileType == QLatin1String("user"))
    {
        fileType = fileTypeAttributes.namedItem(QStringLiteral("user")).nodeValue();
    }

    FileType type(fileType);

    if (docRevision == Document::Revision::Std22)
    {
        type.libext_ = fileTypeAttributes.namedItem(QStringLiteral("libext")).nodeValue();
    }

    return type;
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseFileAttributes()
//-----------------------------------------------------------------------------
void FileReader::Details::parseFileAttributes(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseFileTypes(QDomElement const& fileElement, QSharedPointer<File> newFile, Document::Revision docRevision)
{
    QDomNodeList fileTypeNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:fileType"));
    
    for (int fileTypeIndex = 0; fileTypeIndex < fileTypeNodeList.count(); ++fileTypeIndex)
    {
        QDomNode fileTypeNode = fileTypeNodeList.at(fileTypeIndex);

        newFile->getFileTypes()->append(parseFileType(fileTypeNode, docRevision));
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseIsStructural()
//-----------------------------------------------------------------------------
void FileReader::Details::parseIsStructural(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseIsIncludeFile(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseLogicalName(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseExportedNames(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseBuildCommand(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseBuildCommandCommand(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand)
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
void FileReader::Details::parseBuildCommandFlags(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand)
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
void FileReader::Details::parseBuildCommandReplaceFlags(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand)
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
void FileReader::Details::parseBuildCommandTarget(QDomElement const& buildCommandElement,
    QSharedPointer<BuildCommand> newBuildCommand)
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
void FileReader::Details::parseDependencies(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseDefines(QDomElement const& fileElement, QSharedPointer<File> newFile)
{
    QDomNodeList defineNodeList = fileElement.elementsByTagName(QStringLiteral("ipxact:define"));

    if (!defineNodeList.isEmpty())
    {
        for (int defineIndex = 0; defineIndex < defineNodeList.count(); ++defineIndex)
        {
            QDomNode defineNode = defineNodeList.at(defineIndex);

            QSharedPointer<NameValuePair> newDefine (new NameValuePair());

            NameGroupReader::parseNameGroup(defineNode, newDefine);

            QString value = defineNode.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
            newDefine->setValue(value);

            CommonItemsReader::parseVendorExtensions(defineNode, newDefine);

            newFile->getDefines()->append(newDefine);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileReader::parseImageTypes()
//-----------------------------------------------------------------------------
void FileReader::Details::parseImageTypes(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseDescription(QDomElement const& fileElement, QSharedPointer<File> newFile)
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
void FileReader::Details::parseFileExtensions(QDomNode const& fileNode, QSharedPointer<File> newFile)
{
    CommonItemsReader::parseVendorExtensions(fileNode, newFile);
}