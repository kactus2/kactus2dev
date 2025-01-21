//-----------------------------------------------------------------------------
// File: FileWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 16.09.2015
//
// Description:
// Writer class for IP-XACT file element.
//-----------------------------------------------------------------------------

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include "File.h"

#include <IPXACTmodels/common/CommonItemsWriter.h>
#include <IPXACTmodels/common/Document.h>
#include <IPXACTmodels/ipxactmodels_global.h>

#include <QXmlStreamWriter>
#include <QObject>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Writer class for IP-XACT file element.
//-----------------------------------------------------------------------------
namespace  FileWriter
{
    /*!
     *  Write a file to an XML file.
     *
     *    @param [in] writer          The used xml writer.
     *    @param [in] file            The file to be written.
     *    @param [in] docRevision     The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT void writeFile(QXmlStreamWriter& writer, QSharedPointer<File> file, Document::Revision docRevision);
 
    /*!
     *  Write a file type to an XML file.
     *
     *    @param [in] writer          The used xml writer.
     *    @param [in] fileType        The file type to be written.
     *    @param [in] docRevision     The applied IP-XACT standard revision.
     */
    void writeFileType(QXmlStreamWriter& writer, FileType const& fileType,
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Write the file types.
         *
         *    @param [in] writer          The used xml writer.
         *    @param [in] file            The file to be written.
         *    @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void writeFileTypes(QXmlStreamWriter& writer, QSharedPointer<File> file, Document::Revision docRevision);

        /*!
         *  Write the file attributes.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeAttributes(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the structural status of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeIsStructural(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the include file status of the value.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeIsIncludeFile(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the logical name of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeLogicalName(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the exported names of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeExportedNames(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the build command of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeBuildCommand(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the dependencies of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeDependencies(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the defines of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeDefines(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the image types of the file.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeImageTypes(QXmlStreamWriter& writer, QSharedPointer<File> file);

        /*!
         *  Write the file extensions.
         *
         *    @param [in] writer  The used xml writer.
         *    @param [in] file    The file to be written.
         */
        void writeFileExtensions(QXmlStreamWriter& writer, QSharedPointer<File> file);
    }
};

#endif // FILEWRITER_H