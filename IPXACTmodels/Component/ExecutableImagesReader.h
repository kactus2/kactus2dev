//-----------------------------------------------------------------------------
// File: ExecutableImagesReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Anton Hagqvist
// Date: 27.5.2026
//
// Description:
// Reader for IP-XACT executableImage elements.
//-----------------------------------------------------------------------------

#ifndef EXECUTABLEIMAGESREADER_H
#define EXECUTABLEIMAGESREADER_H

#include "ExecutableImage.h"

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader for IP-XACT executableImage elements.
//-----------------------------------------------------------------------------
namespace ExecutableImagesReader
{
    /*!
     *  Creates ExecutableImages from a given parent node.
     *
     *    @param [in] executableImageNode     XML description of the containing element.
     *    @param [in] docRevision             The standard revision to use.
     *
     *    @return The created executable image.
     */
    [[nodiscard]] IPXACTMODELS_EXPORT QSharedPointer<QList<QSharedPointer<ExecutableImage> > > parseAndCreateExecutableImages(
        QDomNode const& parentNode, Document::Revision docRevision);

    namespace Details
    {
        /*!
        *  Creates a new ExecutableImage from a given executableImage node.
        *
        *    @param [in] executableImageNode    XML description of the executable image.
        *    @param [in] docRevision            The standard revision to use.
        *
        *    @return The created executable image.
        */
        [[nodiscard]] QSharedPointer<ExecutableImage> parseAndCreateExecutableImage(
            QDomNode const& parentNode, Document::Revision docRevision);

        /*!
         *  Parse language tools from an executable image node.
         *
         *    @param [in] languageToolsElement    The language tools XML element.
         *    @param [in] image                   The executable image to populate.
         */
        void parseLanguageTools(QDomElement const& languageToolsElement,
            QSharedPointer<ExecutableImage> image);

        /*!
         *  Parse linker command file from language tools element.
         *
         *    @param [in] linkerCommandFileElement    The linker command file XML element.
         *    @param [in] languageTools               The language tools to populate.
         */
        void parseLinkerCommandFile(QDomElement const& linkerCommandFileElement,
            QSharedPointer<LanguageTools> languageTools);
    }
};

#endif // EXECUTABLEIMAGESREADER_H
