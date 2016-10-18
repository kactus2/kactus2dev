//-----------------------------------------------------------------------------
// File: FileSetWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------

#include "FileSetWriter.h"
#include "FileWriter.h"

#include <IPXACTmodels/common/FileBuilderWriter.h>
#include <IPXACTmodels/common/NameGroupWriter.h>
#include <IPXACTmodels/common/FileTypes.h>

//-----------------------------------------------------------------------------
// Function: FileSetWriter::FileSetWriter()
//-----------------------------------------------------------------------------
FileSetWriter::FileSetWriter(QObject* parent /* = 0 */) :
CommonItemsWriter(parent)
{

}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::~FileSetWriter()
//-----------------------------------------------------------------------------
FileSetWriter::~FileSetWriter()
{

}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFileSet()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFileSet(QXmlStreamWriter& writer, QSharedPointer<FileSet> fileSet) const
{
    writer.writeStartElement(QStringLiteral("ipxact:fileSet"));

    NameGroupWriter nameGroupWriter;
    nameGroupWriter.writeNameGroup(writer, fileSet);

    writeGroups(writer, fileSet->getGroups());

    writeFiles(writer, fileSet->getFiles());

    writeDefaultFileBuilders(writer, fileSet->getDefaultFileBuilders());

    writeDependencies(writer, fileSet->getDependencies());

    writeFunctions(writer, fileSet->getFunctions());

    writeVendorExtensions(writer, fileSet);

    writer.writeEndElement(); // ipxact:file
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeGroups()
//-----------------------------------------------------------------------------
void FileSetWriter::writeGroups(QXmlStreamWriter& writer, QSharedPointer<QStringList> groups) const
{
    foreach (QString singleGroup, *groups)
    {
        writer.writeTextElement(QStringLiteral("ipxact:group"), singleGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFiles()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFiles(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<File> > > fileSetFiles) const
{
    if (!fileSetFiles->isEmpty())
    {
        FileWriter fileWriter;
        foreach (QSharedPointer<File> file, *fileSetFiles)
        {
            fileWriter.writeFile(writer, file);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSetWriter::writeDefaultFileBuilders(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders) const
{
    if (!defaultFileBuilders->isEmpty())
    {
        FileBuilderWriter fileBuilderWriter;

        foreach (QSharedPointer<FileBuilder> fileBuilder, *defaultFileBuilders)
        {
            fileBuilderWriter.writeDefaultFileBuilder(writer, fileBuilder);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeDependencies()
//-----------------------------------------------------------------------------
void FileSetWriter::writeDependencies(QXmlStreamWriter& writer, QSharedPointer<QStringList> dependencies) const
{
    foreach (QString singleDependency, *dependencies)
    {
        writer.writeTextElement(QStringLiteral("ipxact:dependency"), singleDependency);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctions()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctions(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Function> > > functions) const
{
    foreach (QSharedPointer<Function> singleFunction, *functions)
    {
        writer.writeStartElement(QStringLiteral("ipxact:function"));

        if (singleFunction->getReplicate())
        {
            writer.writeAttribute(QStringLiteral("replicate"), QStringLiteral("true"));
        }

        writeFunctionEntryPoint(writer, singleFunction);

        writer.writeTextElement(QStringLiteral("ipxact:fileRef"), singleFunction->getFileRef());

        writeFunctionReturnType(writer, singleFunction);

        writeFunctionArguments(writer, singleFunction->getArguments());

        writeFunctionDisabledValue(writer, singleFunction);

        writeFunctionSourceFiles(writer, singleFunction->getSourceFiles());

        writer.writeEndElement(); // ipxact:function
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionEntryPoint()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctionEntryPoint(QXmlStreamWriter& writer, QSharedPointer<Function> function) const
{
    if (!function->getEntryPoint().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:entryPoint"), function->getEntryPoint());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeReturnType()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctionReturnType(QXmlStreamWriter& writer, QSharedPointer<Function> function) const
{
    if (!function->getReturnType().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:returnType"), function->getReturnType());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionArguments()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctionArguments(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments) const
{
    foreach (QSharedPointer<NameValuePair> singleArgument, *arguments)
    {
        writer.writeStartElement(QStringLiteral("ipxact:argument"));

        NameGroupWriter nameGroupWriter;
        nameGroupWriter.writeNameGroup(writer, singleArgument);

        writer.writeTextElement(QStringLiteral("ipxact:value"), singleArgument->getValue());

        writeVendorExtensions(writer, singleArgument);

        writer.writeEndElement(); // ipxact:argument
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionDisabledValue()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctionDisabledValue(QXmlStreamWriter& writer, QSharedPointer<Function> function) const
{
    if (!function->getDisabled().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:disabled"), function->getDisabled());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionSourceFiles()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFunctionSourceFiles(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > sourceFiles) const
{
    foreach (QSharedPointer<Function::SourceFile> source, *sourceFiles)
    {
        writer.writeStartElement(QStringLiteral("ipxact:sourceFile"));

        writer.writeTextElement(QStringLiteral("ipxact:sourceName"), source->getSourceName());

        writer.writeStartElement(QStringLiteral("ipxact:fileType"));

        if (FileTypes::isIpXactFileType(source->getFileType()))
        {
            writer.writeCharacters(source->getFileType());
        }
        else
        {
            writer.writeAttribute(QStringLiteral("user"), source->getFileType());
            writer.writeCharacters(QStringLiteral("user"));
        }

        writer.writeEndElement(); // ipxact:fileType

        writer.writeEndElement(); // ipxact:sourceFile
    }
}