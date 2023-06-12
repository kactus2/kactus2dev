//-----------------------------------------------------------------------------
// File: FileBuilderWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Writer class for fileBuilder element.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERWRITER_H
#define FILEBUILDERWRITER_H

#include "CommonItemsWriter.h"

#include <IPXACTmodels/ipxactmodels_global.h>

#include <IPXACTmodels/common/Document.h>

#include <QXmlStreamWriter>
#include <QSharedPointer>

class FileBuilder;

//-----------------------------------------------------------------------------
//! Writer class for fileBuilder element.
//-----------------------------------------------------------------------------
namespace FileBuilderWriter 
{
    /*!
     *  Write the default file builder.
     *
     *      @param [in] writer          The used xml writer.
     *      @param [in] nameGroup       The selected file builder.
     *      @param [in] docRevision     The applied IP-XACT standard revision.
     */
    void writeDefaultFileBuilder(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder,
        Document::Revision docRevision);

    namespace Details
    {

        /*!
         *  Write the build model of the file builder.
         *
         *      @param [in] writer          Used XML writer.
         *      @param [in] fileBuilder     The selected file builder.
         */
        void writeBuildModel(QXmlStreamWriter& writer, QSharedPointer<FileBuilder> fileBuilder);
    }
};

#endif // FILEBUILDERWRITER_H