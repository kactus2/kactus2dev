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
// Function: FileSetReader::FileSetReader()
//-----------------------------------------------------------------------------
FileSetReader::FileSetReader(QObject* parent /* = 0 */) :
CommonItemsReader(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileSetReader::~FileSetReader()
//-----------------------------------------------------------------------------
FileSetReader::~FileSetReader()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetReader::createFileSetFrom()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> FileSetReader::createFileSetFrom(QDomNode const& fileSetNode) const
{
    QDomElement fileSetElement = fileSetNode.toElement();
    QSharedPointer<FileSet> newFileSet (new FileSet());

    NameGroupReader nameGroupReader;
    nameGroupReader.parseNameGroup(fileSetNode, newFileSet);

    parseGroups(fileSetElement, newFileSet);

    parseFiles(fileSetElement, newFileSet);

    parseDefaultFileBuilders(fileSetElement, newFileSet);

    parseDependencies(fileSetElement, newFileSet);

    parseFunctions(fileSetElement, newFileSet);

    parseFileSetExtensions(fileSetElement, newFileSet);

    return newFileSet;
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseGroups()
//-----------------------------------------------------------------------------
void FileSetReader::parseGroups(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const
{
    QDomNodeList groupNodeList = fileSetElement.elementsByTagName("ipxact:group");

    for (int groupIndex = 0; groupIndex < groupNodeList.count(); ++groupIndex)
    {
        QString newGroup = groupNodeList.at(groupIndex).firstChild().nodeValue();
        newFileSet->getGroups()->append(newGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFiles()
//-----------------------------------------------------------------------------
void FileSetReader::parseFiles(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const
{
	// Find all file-elements.
    QDomNodeList fileNodeList = fileSetElement.elementsByTagName("ipxact:file");

    if (!fileNodeList.isEmpty())
    {
        FileReader fileReader;
        for (int fileIndex = 0; fileIndex < fileNodeList.count(); ++fileIndex)
		{
			// Read each file with the file reader.
            QSharedPointer<File> newFile = fileReader.createFileFrom(fileNodeList.at(fileIndex));
			// Then append the file to the list.
            newFileSet->getFiles()->append(newFile);
        }
    }

	// Sort the files in the file set.
	newFileSet->sortFiles();
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSetReader::parseDefaultFileBuilders(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet)
    const
{
    QDomNodeList fileBuilderNodeList = fileSetElement.elementsByTagName("ipxact:defaultFileBuilder");

    if (!fileBuilderNodeList.isEmpty())
    {
        FileBuilderReader fileBuilderReader;
        for (int fileBuilderIndex = 0; fileBuilderIndex < fileBuilderNodeList.count(); ++fileBuilderIndex)
        {
            QSharedPointer<FileBuilder> newFileBuilder =
                fileBuilderReader.createDefaultFileBuilderFrom(fileBuilderNodeList.at(fileBuilderIndex));
            newFileSet->getDefaultFileBuilders()->append(newFileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseDependencies()
//-----------------------------------------------------------------------------
void FileSetReader::parseDependencies(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const
{
    for (int childnodeIndex = 0; childnodeIndex < fileSetElement.childNodes().count(); ++childnodeIndex)
    {
        QDomNode childNode = fileSetElement.childNodes().at(childnodeIndex);

        if (childNode.nodeName() == "ipxact:dependency")
        {
            QString dependency = childNode.firstChild().nodeValue();
            newFileSet->getDependencies()->append(dependency);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctions()
//-----------------------------------------------------------------------------
void FileSetReader::parseFunctions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const
{
    QDomNodeList functionNodeList = fileSetElement.elementsByTagName("ipxact:function");

    for (int functionIndex = 0; functionIndex < functionNodeList.count(); ++functionIndex)
    {
        QDomElement functionElement = functionNodeList.at(functionIndex).toElement();

        QString fileReference = functionElement.firstChildElement("ipxact:fileRef").firstChild().nodeValue();
        QSharedPointer<Function> newFunction (new Function(fileReference));

        if (functionElement.hasAttribute("replicate"))
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
void FileSetReader::parseFunctionEntryPoint(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction) const
{
    QDomElement entryPointElement = functionElement.firstChildElement("ipxact:entryPoint");
    if (!entryPointElement.isNull())
    {
        QString entryPoint = entryPointElement.firstChild().nodeValue();
        newFunction->setEntryPoint(entryPoint);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionReturnType()
//-----------------------------------------------------------------------------
void FileSetReader::parseFunctionReturnType(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction) const
{
    QDomElement returnTypeElement = functionElement.firstChildElement("ipxact:returnType");
    if (!returnTypeElement.isNull())
    {
        QString returnType = returnTypeElement.firstChild().nodeValue();
        newFunction->setReturnType(returnType);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionArguments()
//-----------------------------------------------------------------------------
void FileSetReader::parseFunctionArguments(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction) const
{
    QDomNodeList argumentNodeList = functionElement.elementsByTagName("ipxact:argument");

    if (!argumentNodeList.isEmpty())
    {
        NameGroupReader nameGroupReader;
        
        for (int argumentIndex = 0; argumentIndex < argumentNodeList.count(); ++argumentIndex)
        {
            QDomNode argumentNode = argumentNodeList.at(argumentIndex);

            QSharedPointer<NameValuePair> newArgument (new NameValuePair());

            nameGroupReader.parseNameGroup(argumentNode, newArgument);

            QString value = argumentNode.firstChildElement("ipxact:value").firstChild().nodeValue();
            newArgument->setValue(value);

            parseVendorExtensions(argumentNode, newArgument);

            newFunction->getArguments()->append(newArgument);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionDisabled()
//-----------------------------------------------------------------------------
void FileSetReader::parseFunctionDisabled(QDomElement const& functionElement, QSharedPointer<Function> newFunction)
    const
{
    QDomElement disabledElement = functionElement.firstChildElement("ipxact:disabled");
    if (!disabledElement.isNull())
    {
        QString disabledValue = disabledElement.firstChild().nodeValue();
        newFunction->setDisabled(disabledValue);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFunctionSourceFiles()
//-----------------------------------------------------------------------------
void FileSetReader::parseFunctionSourceFiles(QDomElement const& functionElement,
    QSharedPointer<Function> newFunction) const
{
    QDomNodeList sourceFileNodeList = functionElement.elementsByTagName("ipxact:sourceFile");

    for (int i = 0; i < sourceFileNodeList.count(); ++i)
    {
        QDomNode sourceNode = sourceFileNodeList.at(i);

        QString sourceName = sourceNode.firstChildElement("ipxact:sourceName").firstChild().nodeValue();
        QString sourceType = sourceNode.firstChildElement("ipxact:fileType").firstChild().nodeValue();

        QSharedPointer<Function::SourceFile> newSourceFile (new Function::SourceFile(sourceName, sourceType));

        newFunction->getSourceFiles()->append(newSourceFile);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetReader::parseFileSetExtensions()
//-----------------------------------------------------------------------------
void FileSetReader::parseFileSetExtensions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet)
    const
{
    QDomElement extensionsElement = fileSetElement.firstChildElement("ipxact:vendorExtensions");
    QDomElement idExtension = extensionsElement.firstChildElement("kactus2:fileSetId");
    if (!idExtension.isNull())
    {
        QString fileSetID = idExtension.firstChild().nodeValue();
        newFileSet->setFileSetId(fileSetID);
    }

    parseVendorExtensions(fileSetElement, newFileSet);
}