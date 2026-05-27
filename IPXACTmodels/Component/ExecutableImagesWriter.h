//-----------------------------------------------------------------------------
// File: ExecutableImagesWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.5.2026
//
// Description:
// Writer for IP-XACT executableImage elements.
//-----------------------------------------------------------------------------

#ifndef EXECUTABLEIMAGESWRITER_H
#define EXECUTABLEIMAGESWRITER_H

#include "ExecutableImage.h"

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QXmlStreamWriter>

//-----------------------------------------------------------------------------
//! Writer for IP-XACT executableImage element.
//-----------------------------------------------------------------------------
namespace ExecutableImagesWriter
{
    /*!
    *  Writes executable images of containing element to XML.
    *
    *    @param [in] writer                  The XML stream writer.
    *    @param [in] image                   The executable image to write.
    *    @param [in] docRevision             The standard revision to use.
    */
    IPXACTMODELS_EXPORT void writeExecutableImages(QXmlStreamWriter& writer,
        QSharedPointer<QList<QSharedPointer<ExecutableImage> > > images, Document::Revision docRevision);

    namespace Details
    {
        /*!
        *  Writes an executableImage to XML.
        *
        *    @param [in] writer                  The XML stream writer.
        *    @param [in] image                   The executable image to write.
        *    @param [in] docRevision             The standard revision to use.
        */
        void writeExecutableImage(QXmlStreamWriter& writer,
            QSharedPointer<ExecutableImage> image, Document::Revision docRevision);

        /*!
         *  Writes language tools for an executable image.
         *
         *    @param [in] writer          The XML stream writer.
         *    @param [in] languageTools   The language tools to write.
         */
        void writeLanguageTools(QXmlStreamWriter& writer, QSharedPointer<LanguageTools> languageTools);
    }
};

#endif // EXECUTABLEIMAGESWRITER_H
