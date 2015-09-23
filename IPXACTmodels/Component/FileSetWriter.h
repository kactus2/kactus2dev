//-----------------------------------------------------------------------------
// File: FileSetWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 21.09.2015
//
// Description:
// Writer class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------

#ifndef FILESETWRITER_H
#define FILESETWRITER_H

#include "FileSet.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileSetWriter : public CommonItemsWriter
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  Owner of this writer.
     */
    FileSetWriter(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileSetWriter();

    /*!
     *  Write a file set to an XML file.
     *
     *      @param [in] writer  The used xml writer.
     *      @param [in] file    The file set to be written.
     */
    void writeFileSet(QXmlStreamWriter& writer, QSharedPointer<FileSet> fileSet) const;

private:

    //! No copying allowed.
    FileSetWriter(FileSetWriter const& rhs);
    FileSetWriter& operator=(FileSetWriter const& rhs);

    /*!
     *  Write the file set groups.
     *
     *      @param [in] writer  Used XML writer.
     *      @param [in] groups  The file set groups.
     */
    void writeGroups(QXmlStreamWriter& writer, QSharedPointer<QStringList> groups) const;

    /*!
     *  Write the files.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] fileSetFiles    The files.
     */
    void writeFiles(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<File> > > fileSetFiles) const;

    /*!
     *  Write the default file builders.
     *
     *      @param [in] writer                  Used XML writer.
     *      @param [in] defaultFileBuilders     The default file builders.
     */
    void writeDefaultFileBuilders(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders) const;

    /*!
     *  Write the dependencies.
     *
     *      @param [in] writer          Used XML writer.
     *      @param [in] dependencies    The dependencies of the file set.
     */
    void writeDependencies(QXmlStreamWriter& writer, QSharedPointer<QStringList> dependencies) const;

    /*!
     *  Write the functions.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] functions   The functions.
     */
    void writeFunctions(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Function> > > functions)
        const;

    /*!
     *  Write the function entry point.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] function    The selected function.
     */
    void writeFunctionEntryPoint(QXmlStreamWriter& writer, QSharedPointer<Function> function) const;

    /*!
     *  Write the function return type.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] function    The selected function.
     */
    void writeFunctionReturnType(QXmlStreamWriter& writer, QSharedPointer<Function> function) const;

    /*!
     *  Write the function arguments.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] arguments   The function arguments.
     */
    void writeFunctionArguments(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments) const;

    /*!
     *  Write the function disabled value.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] function    The selected function.
     */
    void writeFunctionDisabledValue(QXmlStreamWriter& writer, QSharedPointer<Function> function) const;

    /*!
     *  Write the function source files.
     *
     *      @param [in] writer      Used XML writer.
     *      @param [in] function    The selected function.
     */
    void writeFunctionSourceFiles(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > sourceFiles) const;
};

#endif // FILESETWRITER_H