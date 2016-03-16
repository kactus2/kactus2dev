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

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT fileSet element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileSetReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    FileSetReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileSetReader();

    /*!
     *  Creates a new file set from a given file set node.
     *
     *      @param [in] fileSetNode     XML description of the file set.
     */
    QSharedPointer<FileSet> createFileSetFrom(QDomNode const& fileSetNode) const;

private:

    //! No copying allowed.
    FileSetReader(FileSetReader const& rhs);
    FileSetReader& operator=(FileSetReader const& rhs);

    /*!
     *  Reads the groups.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseGroups(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;

    /*!
     *  Reads the files.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseFiles(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;

    /*!
     *  Reads the default file builders.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseDefaultFileBuilders(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;

    /*!
     *  Reads the dependencies.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseDependencies(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;

    /*!
     *  Reads the functions.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseFunctions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;

    /*!
     *  Reads the function entry point.
     *
     *      @param [in] functionElement     XML description of the function.
     *      @param [in] newFunction         The new function item.
     */
    void parseFunctionEntryPoint(QDomElement const& functionElement, QSharedPointer<Function> newFunction) const;

    /*!
     *  Reads the function return type.
     *
     *      @param [in] functionElement     XML description of the function.
     *      @param [in] newFunction         The new function item.
     */
    void parseFunctionReturnType(QDomElement const& functionElement, QSharedPointer<Function> newFunction) const;

    /*!
     *  Reads the function arguments.
     *
     *      @param [in] functionElement     XML description of the function.
     *      @param [in] newFunction         The new function item.
     */
    void parseFunctionArguments(QDomElement const& functionElement, QSharedPointer<Function> newFunction) const;

    /*!
     *  Reads the function disabled value.
     *
     *      @param [in] functionElement     XML description of the function.
     *      @param [in] newFunction         The new function item.
     */
    void parseFunctionDisabled(QDomElement const& functionElement, QSharedPointer<Function> newFunction) const;

    /*!
     *  Reads the function source files.
     *
     *      @param [in] functionElement     XML description of the function.
     *      @param [in] newFunction         The new function item.
     */
    void parseFunctionSourceFiles(QDomElement const& functionElement, QSharedPointer<Function> newFunction) const;

    /*!
     *  Reads the file set extensions.
     *
     *      @param [in] fileSetElement  XML description of the file set.
     *      @param [in] newFileSet      The new file set item.
     */
    void parseFileSetExtensions(QDomElement const& fileSetElement, QSharedPointer<FileSet> newFileSet) const;
};

#endif // FILESETREADER_H