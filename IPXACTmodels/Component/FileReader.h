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

#include <IPXACTmodels/common/CommonItemsReader.h>

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT file element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT FileReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    FileReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~FileReader();

    /*!
     *  Creates a new file from a given file node.
     *
     *      @param [in] fileNode    XML description of the file.
     */
    QSharedPointer<File> createFileFrom(QDomNode const& fileNode) const;

private:

    //! No copying allowed.
    FileReader(FileReader const& rhs);
    FileReader& operator=(FileReader const& rhs);

    /*!
     *  Parses the file attributes.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseFileAttributes(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the file types.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseFileTypes(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the structural status.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseIsStructural(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the include file status.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseIsIncludeFile(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the logical name.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseLogicalName(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the exported names.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseExportedNames(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the file build command.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseBuildCommand(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the command used in the build command.
     *
     *      @param [in] buildCommandElement     XML description of the build command.
     *      @param [in] newBuildCommand         The new build command item.
     */
    void parseBuildCommandCommand(QDomElement const& buildCommandElement,
        QSharedPointer<BuildCommand> newBuildCommand) const;

    /*!
     *  Parses the build command flags.
     *
     *      @param [in] buildCommandElement     XML description of the build command.
     *      @param [in] newBuildCommand         The new build command item.
     */
    void parseBuildCommandFlags(QDomElement const& buildCommandElement,
        QSharedPointer<BuildCommand> newBuildCommand) const;

    /*!
     *  Parses the expression for build command flag replacement.
     *
     *      @param [in] buildCommandElement     XML description of the build command.
     *      @param [in] newBuildCommand         The new build command item.
     */
    void parseBuildCommandReplaceFlags(QDomElement const& buildCommandElement,
        QSharedPointer<BuildCommand> newBuildCommand) const;

    /*!
     *  Parses the build command target.
     *
     *      @param [in] buildCommandElement     XML description of the build command.
     *      @param [in] newBuildCommand         The new build command item.
     */
    void parseBuildCommandTarget(QDomElement const& buildCommandElement,
        QSharedPointer<BuildCommand> newBuildCommand) const;

    /*!
     *  Parses the dependencies.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseDependencies(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the defines.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseDefines(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the image types.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseImageTypes(QDomElement const& fileElement, QSharedPointer<File> newFile) const;

    /*!
     *  Parses the file vendor extensions.
     *
     *      @param [in] fileElement     XML description of the file.
     *      @param [in] newFile         The new file item.
     */
    void parseFileExtensions(QDomNode const& fileNode, QSharedPointer<File> newFile) const;
};

#endif // FILEREADER_H