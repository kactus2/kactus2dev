//-----------------------------------------------------------------------------
// File: FileSetReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 22.09.2015
//
// Description:
// Reader class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------

#ifndef FILESETREADER_H
#define FILESETREADER_H

#include "FileSet.h"

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------
namespace FileSetReader 
{
    /*!
     *  Creates a new file set from a given file set node.
     *
     *      @param [in] fileSetNode     XML description of the file set.
     *      @param [in] docRevision     The applied IP-XACT standard revision.
     */
    IPXACTMODELS_EXPORT QSharedPointer<FileSet> createFileSetFrom(QDomNode const& fileSetNode, Document::Revision docRevision);

    namespace Details
    {
        /*!
         *  Reads the groups.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         */
        void parseGroups(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet);

        /*!
         *  Reads the files.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseFiles(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet, Document::Revision docRevision);

        /*!
         *  Reads the default file builders.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         *      @param [in] docRevision     The applied IP-XACT standard revision.
         */
        void parseDefaultFileBuilders(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet,
            Document::Revision docRevision);

        /*!
         *  Reads the dependencies.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         */
        void parseDependencies(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet);

        /*!
         *  Reads the functions.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         */
        void parseFunctions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet);

        /*!
         *  Reads the function entry point.
         *
         *      @param [in] functionElement     XML description of the function.
         *      @param [in] newFunction         The new function item.
         */
        void parseFunctionEntryPoint(QDomElement const& functionElement, QSharedPointer<Function> newFunction);

        /*!
         *  Reads the function return type.
         *
         *      @param [in] functionElement     XML description of the function.
         *      @param [in] newFunction         The new function item.
         */
        void parseFunctionReturnType(QDomElement const& functionElement, QSharedPointer<Function> newFunction);

        /*!
         *  Reads the function arguments.
         *
         *      @param [in] functionElement     XML description of the function.
         *      @param [in] newFunction         The new function item.
         */
        void parseFunctionArguments(QDomElement const& functionElement, QSharedPointer<Function> newFunction);

        /*!
         *  Reads the function disabled value.
         *
         *      @param [in] functionElement     XML description of the function.
         *      @param [in] newFunction         The new function item.
         */
        void parseFunctionDisabled(QDomElement const& functionElement, QSharedPointer<Function> newFunction);

        /*!
         *  Reads the function source files.
         *
         *      @param [in] functionElement     XML description of the function.
         *      @param [in] newFunction         The new function item.
         */
        void parseFunctionSourceFiles(QDomElement const& functionElement, QSharedPointer<Function> newFunction);

        /*!
         *  Reads the file set extensions.
         *
         *      @param [in] fileSetElement  XML description of the file set.
         *      @param [in] newFileSet      The new file set item.
         */
        void parseFileSetExtensions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet);
    }
};

#endif // FILESETREADER_H