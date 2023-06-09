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
#include <IPXACTmodels/common/FileType.h>


//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFileSet()
//-----------------------------------------------------------------------------
void FileSetWriter::writeFileSet(QXmlStreamWriter& writer, QSharedPointer<FileSet> fileSet,
    Document::Revision docRevision)
{
    writer.writeStartElement(QStringLiteral("ipxact:fileSet"));

    NameGroupWriter::writeNameGroup(writer, fileSet);

    Details::writeGroups(writer, fileSet->getGroups());

    Details::writeFiles(writer, fileSet->getFiles(), docRevision);

    Details::writeDefaultFileBuilders(writer, fileSet->getDefaultFileBuilders(), docRevision);

    Details::writeDependencies(writer, fileSet->getDependencies());

    Details::writeFunctions(writer, fileSet->getFunctions(), docRevision);

    CommonItemsWriter::writeVendorExtensions(writer, fileSet);

    writer.writeEndElement(); // ipxact:file
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeGroups()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeGroups(QXmlStreamWriter& writer, QSharedPointer<QStringList> groups)
{
    for (QString const& singleGroup : *groups)
    {
        writer.writeTextElement(QStringLiteral("ipxact:group"), singleGroup);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFiles()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFiles(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<File> > > fileSetFiles, Document::Revision docRevision)
{
    if (!fileSetFiles->isEmpty())
    {
        for (QSharedPointer<File> file : *fileSetFiles)
        {
            FileWriter::writeFile(writer, file, docRevision);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeDefaultFileBuilders()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeDefaultFileBuilders(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders, 
    Document::Revision docRevision)
{
    if (!defaultFileBuilders->isEmpty())
    {
        for (QSharedPointer<FileBuilder> fileBuilder : *defaultFileBuilders)
        {
            FileBuilderWriter::writeDefaultFileBuilder(writer, fileBuilder, docRevision);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeDependencies()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeDependencies(QXmlStreamWriter& writer, QSharedPointer<QStringList> dependencies)
{
    for (QString const& singleDependency : *dependencies)
    {
        writer.writeTextElement(QStringLiteral("ipxact:dependency"), singleDependency);
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctions()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctions(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Function> > > functions, 
    Document::Revision docRevision)
{
    for (QSharedPointer<Function> singleFunction : *functions)
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

        writeFunctionSourceFiles(writer, singleFunction->getSourceFiles(), docRevision);

        writer.writeEndElement(); // ipxact:function
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionEntryPoint()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctionEntryPoint(QXmlStreamWriter& writer, QSharedPointer<Function> function)
{
    if (!function->getEntryPoint().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:entryPoint"), function->getEntryPoint());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeReturnType()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctionReturnType(QXmlStreamWriter& writer, QSharedPointer<Function> function)
{
    if (!function->getReturnType().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:returnType"), function->getReturnType());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionArguments()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctionArguments(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments)
{
    for (QSharedPointer<NameValuePair> singleArgument : *arguments)
    {
        writer.writeStartElement(QStringLiteral("ipxact:argument"));

        NameGroupWriter::writeNameGroup(writer, singleArgument);

        writer.writeTextElement(QStringLiteral("ipxact:value"), singleArgument->getValue());

        CommonItemsWriter::writeVendorExtensions(writer, singleArgument);

        writer.writeEndElement(); // ipxact:argument
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionDisabledValue()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctionDisabledValue(QXmlStreamWriter& writer, QSharedPointer<Function> function)
{
    if (!function->getDisabled().isEmpty())
    {
        writer.writeTextElement(QStringLiteral("ipxact:disabled"), function->getDisabled());
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetWriter::writeFunctionSourceFiles()
//-----------------------------------------------------------------------------
void FileSetWriter::Details::writeFunctionSourceFiles(QXmlStreamWriter& writer,
    QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > sourceFiles,
    Document::Revision docRevision)
{
    for (QSharedPointer<Function::SourceFile> source : *sourceFiles)
    {
        writer.writeStartElement(QStringLiteral("ipxact:sourceFile"));

        writer.writeTextElement(QStringLiteral("ipxact:sourceName"), source->getSourceName());

        FileWriter::writeFileType(writer, source->getFileType(), docRevision);

        writer.writeEndElement(); // ipxact:sourceFile
    }
}
