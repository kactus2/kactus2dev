//-----------------------------------------------------------------------------
// File: FileReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 18.09.2015
//
// Description:
// Reader class for IP-XACT file element.
//-----------------------------------------------------------------------------

#ifndef FILEREADER_H
#define FILEREADER_H

#include "File.h"

#include <IPXACTmodels/common/Document.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT file element.
//-----------------------------------------------------------------------------
namespace FileReader
{
    /*!
     *  Creates a new file from a given file node.
     *
     *      @param [in] fileNode        XML description of the file.
     *      @param [in] docRevision     The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT QSharedPointer<File> createFileFrom(QDomNode const& fileNode, Document::Revision docRevision);

    /*!
     *  Creates a new file type from a given node.
     *
     *      @param [in] fileTypeNode    XML description of the file type.
     *      @param [in] docRevision     The applied IP-XACT standard revision.
     */
    FileType parseFileType(QDomNode const& fileTypeNode, Document::Revision docRevision);


    namespace Details
    {
        /*!
         *  Parses the file attributes.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseFileAttributes(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the file types.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseFileTypes(QDomElement const& fileElement, QSharedPointer<File> newFile, Document::Revision docRevision);

        /*!
         *  Parses the structural status.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseIsStructural(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the include file status.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseIsIncludeFile(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the logical name.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseLogicalName(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the exported names.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseExportedNames(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the file build command.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseBuildCommand(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the command used in the build command.
         *
         *      @param [in] buildCommandElement     XML description of the build command.
         *      @param [in] newBuildCommand         The new build command item.
         */
        void parseBuildCommandCommand(QDomElement const& buildCommandElement,
            QSharedPointer<BuildCommand> newBuildCommand);

        /*!
         *  Parses the build command flags.
         *
         *      @param [in] buildCommandElement     XML description of the build command.
         *      @param [in] newBuildCommand         The new build command item.
         */
        void parseBuildCommandFlags(QDomElement const& buildCommandElement,
            QSharedPointer<BuildCommand> newBuildCommand);

        /*!
         *  Parses the expression for build command flag replacement.
         *
         *      @param [in] buildCommandElement     XML description of the build command.
         *      @param [in] newBuildCommand         The new build command item.
         */
        void parseBuildCommandReplaceFlags(QDomElement const& buildCommandElement,
            QSharedPointer<BuildCommand> newBuildCommand);

        /*!
         *  Parses the build command target.
         *
         *      @param [in] buildCommandElement     XML description of the build command.
         *      @param [in] newBuildCommand         The new build command item.
         */
        void parseBuildCommandTarget(QDomElement const& buildCommandElement,
            QSharedPointer<BuildCommand> newBuildCommand);

        /*!
         *  Parses the dependencies.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseDependencies(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the defines.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseDefines(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the image types.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseImageTypes(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the description.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseDescription(QDomElement const& fileElement, QSharedPointer<File> newFile);

        /*!
         *  Parses the file vendor extensions.
         *
         *      @param [in] fileElement     XML description of the file.
         *      @param [in] newFile         The new file item.
         */
        void parseFileExtensions(QDomNode const& fileNode, QSharedPointer<File> newFile);
    }
};

#endif // FILEREADER_H