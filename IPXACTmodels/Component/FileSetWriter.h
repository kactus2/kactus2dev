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
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------
namespace FileSetWriter 
{

    /*!
     *  Write a file set to an XML file.
     *
     *    @param [in] writer          The used xml writer.
     *    @param [in] file            The file set to be written.
     *    @param [in] docRevision     The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT void writeFileSet(QXmlStreamWriter& writer, QSharedPointer<FileSet> fileSet,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the file set groups.
         *
         *    @param [in] writer  Used XML writer.
         *    @param [in] groups  The file set groups.
         */
        void writeGroups(QXmlStreamWriter& writer, QSharedPointer<QStringList> groups);

        /*!
         *  Write the files.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] fileSetFiles    The files.
         *    @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void writeFiles(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<File> > > fileSetFiles,
            Document::Revision docRevision);

        /*!
         *  Write the default file builders.
         *
         *    @param [in] writer                  Used XML writer.
         *    @param [in] defaultFileBuilders     The default file builders.
         *    @param [in] docRevision             The applied IP-XACT standard revision.
         */
        void writeDefaultFileBuilders(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<FileBuilder> > > defaultFileBuilders,
            Document::Revision docRevision);

        /*!
         *  Write the dependencies.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] dependencies    The dependencies of the file set.
         */
        void writeDependencies(QXmlStreamWriter& writer, QSharedPointer<QStringList> dependencies);

        /*!
         *  Write the functions.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] functions       The functions.
         *    @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void writeFunctions(QXmlStreamWriter& writer, QSharedPointer<QList<QSharedPointer<Function> > > functions,
            Document::Revision docRevision);

        /*!
         *  Write the function entry point.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] function    The selected function.
         */
        void writeFunctionEntryPoint(QXmlStreamWriter& writer, QSharedPointer<Function> function);

        /*!
         *  Write the function return type.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] function    The selected function.
         */
        void writeFunctionReturnType(QXmlStreamWriter& writer, QSharedPointer<Function> function);

        /*!
         *  Write the function arguments.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] arguments   The function arguments.
         */
        void writeFunctionArguments(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<NameValuePair> > > arguments);

        /*!
         *  Write the function disabled value.
         *
         *    @param [in] writer      Used XML writer.
         *    @param [in] function    The selected function.
         */
        void writeFunctionDisabledValue(QXmlStreamWriter& writer, QSharedPointer<Function> function);

        /*!
         *  Write the function source files.
         *
         *    @param [in] writer          Used XML writer.
         *    @param [in] function        The selected function.
         *    @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void writeFunctionSourceFiles(QXmlStreamWriter& writer,
            QSharedPointer<QList<QSharedPointer<Function::SourceFile> > > sourceFiles,
            Document::Revision docRevision);
    }
};

#endif // FILESETWRITER_H