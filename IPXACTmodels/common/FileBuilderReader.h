//-----------------------------------------------------------------------------
// File: FileBuilderReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Reader class for file builder element.
//-----------------------------------------------------------------------------

#ifndef FILEBUILDERREADER_H
#define FILEBUILDERREADER_H

#include "FileBuilder.h"

#include <IPXACTmodels/ipxactmodels_global.h>
#include <IPXACTmodels/common/CommonItemsReader.h>

#include "Document.h"

#include <QDomNode>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Reader class for file builder element.
//-----------------------------------------------------------------------------
namespace FileBuilderReader
{
    /*!
     *  Reads the default file builder.
     *
     *      @param [in] fileBuilderNode     XML description of the default file builder.
     *      @param [in] docRevision         The applied IP-XACT standard revision.
     *
     *      @return The created default file builder.
     */
    IPXACTMODELS_EXPORT QSharedPointer<FileBuilder> createFileBuilderFrom(QDomNode const& fileBuilderNode,
        Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Reads the file builder file type.
         *
         *      @param [in] filebuilderNode     XML description of the file builder.
         *      @param [in] newFileBuilder      The selected file builder.
         *      @param [in] docRevision         The applied IP-XACT standard revision.
         */
        void parseFileType(QDomNode const& filebuilderNode, QSharedPointer<FileBuilder> newFileBuilder,
            Document::Revision docRevision);

        /*!
         *  Reads the file builder command, flags and replace default flags value.
         *
         *      @param [in] filebuilderNode     XML description of the file builder.
         *      @param [in] newFileBuilder      The selected file builder.
         */
        void parseBuildModel(QDomNode const& fileBuilderNode, QSharedPointer<FileBuilder> newFileBuilder);
    }
};

#endif // FILEBUILDERREADER_H