//-----------------------------------------------------------------------------
// File: FileSetReader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Reader class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------

#include "FileSetReader.h"
#include "FileReader.h"

#include <IPXACTmodels/common/FileBuilderReader.h>
#include <IPXACTmodels/common/NameGroupReader.h>


//-----------------------------------------------------------------------------
// Function: FileSetReader::createFileSetFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> FileSetReader::createFileSetFrom(QDomNode const& fileSetNode, Document::Revision docRevision)
{
    QDomElement fileSetElement = fileSetNode.toElement();
    QSharedPointer<FileSet> newFileSet (new FileSet());

    NameGroupReader::parseNameGroup(fileSetNode, newFileSet);

    Details::parseGroups(fileSetElement, newFileSet);

    Details::parseFiles(fileSetElement, newFileSet, docRevision);

    Details::parseDefaultFileBuilders(fileSetElement, newFileSet, docRevision);

    Details::parseDependencies(fileSetElement, newFileSet);

    Details::parseFunctions(fileSetElement, newFileSet);

    Details::parseFileSetExtensions(fileSetElement, newFileSet);

    return newFileSet;
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseGroups()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseGroups(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet)
{
    QDomNodeList groupNodeList = fileSetElement.elementsByTagName(QStringLiteral("ipxact:group"));

    for (int groupIndex = 0; groupIndex < groupNodeList.count(); ++groupIndex)
    {
        QString newGroup = groupNodeList.at(groupIndex).firstChild().nodeValue();
        newFileSet->getGroups()->append(newGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFiles()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFiles(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet,
    Document::Revision docRevision)
{
	// Find all file-elements.
    QDomNodeList fileNodeList = fileSetElement.elementsByTagName(QStringLiteral("ipxact:file"));

    if (!fileNodeList.isEmpty())
    {
        for (int fileIndex = 0; fileIndex < fileNodeList.count(); ++fileIndex)
		{
			// Read each file with the file reader.
            QSharedPointer<File> newFile = FileReader::createFileFrom(fileNodeList.at(fileIndex), docRevision);
			// Then append the file to the list.
            newFileSet->getFiles()->append(newFile);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseDefaultFileBuilders(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet,
    Document::Revision docRevision)
{
    QDomNodeList fileBuilderNodeList = fileSetElement.elementsByTagName(QStringLiteral("ipxact:defaultFileBuilder"));

    if (!fileBuilderNodeList.isEmpty())
    {
        for (int fileBuilderIndex = 0; fileBuilderIndex < fileBuilderNodeList.count(); ++fileBuilderIndex)
        {
            QSharedPointer<FileBuilder> newFileBuilder =
                FileBuilderReader::createFileBuilderFrom(fileBuilderNodeList.at(fileBuilderIndex), docRevision);
            newFileSet->getDefaultFileBuilders()->append(newFileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseDependencies()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseDependencies(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet)
{
    for (int childnodeIndex = 0; childnodeIndex < fileSetElement.childNodes().count(); ++childnodeIndex)
    {
        QDomNode childNode = fileSetElement.childNodes().at(childnodeIndex);

        if (childNode.nodeName() == QStringLiteral("ipxact:dependency"))
        {
            QString dependency = childNode.firstChild().nodeValue();
            newFileSet->getDependencies()->append(dependency);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctions()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet)
{
    QDomNodeList functionNodeList = fileSetElement.elementsByTagName(QStringLiteral("ipxact:function"));

    for (int functionIndex = 0; functionIndex < functionNodeList.count(); ++functionIndex)
    {
        QDomElement functionElement = functionNodeList.at(functionIndex).toElement();

        QString fileReference = functionElement.firstChildElement(QStringLiteral("ipxact:fileRef")).firstChild().nodeValue();
        QSharedPointer<Function> newFunction (new Function(fileReference));

        if (functionElement.hasAttribute(QStringLiteral("replicate")))
        {
            newFunction->setReplicate(true);
        }

        parseFunctionEntryPoint(functionElement, newFunction);

        parseFunctionReturnType(functionElement, newFunction);

        parseFunctionArguments(functionElement, newFunction);

        parseFunctionDisabled(functionElement, newFunction);

        parseFunctionSourceFiles(functionElement, newFunction);

        newFileSet->getFunctions()->append(newFunction);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionEntryPoint()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctionEntryPoint(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction)
{
    QDomElement entryPointElement = functionElement.firstChildElement(QStringLiteral("ipxact:entryPoint"));
    if (!entryPointElement.isNull())
    {
        QString entryPoint = entryPointElement.firstChild().nodeValue();
        newFunction->setEntryPoint(entryPoint);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionReturnType()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctionReturnType(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction)
{
    QDomElement returnTypeElement = functionElement.firstChildElement(QStringLiteral("ipxact:returnType"));
    if (!returnTypeElement.isNull())
    {
        QString returnType = returnTypeElement.firstChild().nodeValue();
        newFunction->setReturnType(returnType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionArguments()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctionArguments(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction)
{
    QDomNodeList argumentNodeList = functionElement.elementsByTagName(QStringLiteral("ipxact:argument"));

    if (!argumentNodeList.isEmpty())
    {
        NameGroupReader nameGroupReader;
        
        for (int argumentIndex = 0; argumentIndex < argumentNodeList.count(); ++argumentIndex)
        {
            QDomNode argumentNode = argumentNodeList.at(argumentIndex);

            QSharedPointer<NameValuePair> newArgument (new NameValuePair());

            nameGroupReader.parseNameGroup(argumentNode, newArgument);

            QString value = argumentNode.firstChildElement(QStringLiteral("ipxact:value")).firstChild().nodeValue();
            newArgument->setValue(value);

            CommonItemsReader::parseVendorExtensions(argumentNode, newArgument);

            newFunction->getArguments()->append(newArgument);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionDisabled()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctionDisabled(QDomElement const& functionElement, QSharedPointer<Function> newFunction)
{
    QDomElement disabledElement = functionElement.firstChildElement(QStringLiteral("ipxact:disabled"));
    if (!disabledElement.isNull())
    {
        QString disabledValue = disabledElement.firstChild().nodeValue();
        newFunction->setDisabled(disabledValue);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionSourceFiles()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFunctionSourceFiles(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction)
{
    QDomNodeList sourceFileNodeList = functionElement.elementsByTagName(QStringLiteral("ipxact:sourceFile"));

    for (int i = 0; i < sourceFileNodeList.count(); ++i)
    {
        QDomNode sourceNode = sourceFileNodeList.at(i);

        QString sourceName = sourceNode.firstChildElement(QStringLiteral("ipxact:sourceName")).firstChild().nodeValue();
        QString sourceType = sourceNode.firstChildElement(QStringLiteral("ipxact:fileType")).firstChild().nodeValue();

        QSharedPointer<Function::SourceFile> newSourceFile (new Function::SourceFile(sourceName, sourceType));

        newFunction->getSourceFiles()->append(newSourceFile);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFileSetExtensions()
//-----------------------------------------------------------------------------
void FileSetReader::Details::parseFileSetExtensions(QDomElement const& fileSetElement,
    QSharedPointer<FileSet> newFileSet)
{
    QDomElement extensionsElement = fileSetElement.firstChildElement(QStringLiteral("ipxact:vendorExtensions"));
    QDomElement idExtension = extensionsElement.firstChildElement(QStringLiteral("kactus2:fileSetId"));
    if (!idExtension.isNull())
    {
        QString fileSetID = idExtension.firstChild().nodeValue();
        newFileSet->setFileSetId(fileSetID);
    }

    CommonItemsReader::parseVendorExtensions(fileSetElement, newFileSet);
}
