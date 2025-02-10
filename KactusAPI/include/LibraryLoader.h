//-----------------------------------------------------------------------------
// File: LibraryLoader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.04.2018
//
// Description:
// Class for IP-XACT file search and type parsing.
//-----------------------------------------------------------------------------

#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H

#include "DocumentFileAccess.h"

#include <KactusAPI/include/MessageMediator.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QObject>

//-----------------------------------------------------------------------------
//! Class for IP-XACT file search and type parsing.
//-----------------------------------------------------------------------------
class LibraryLoader 
{
public:

	//! Struct for load targets.
    struct LoadTarget
    {
        QString path; //!< The path to IP-XACT file.
        VLNV vlnv;    //!< The VLNV defined in the file.

		//! Constructor.
        LoadTarget(VLNV const& targetVLNV = VLNV(), QString targetPath = QString()): 
            path(targetPath), vlnv(targetVLNV) {}
    };

	/*! Cleans the directory structure.
	*
	*      @param [in] changedDirectories	Changed directories (e.g. after a remove).
	*/
    void clean(QStringList const& changedDirectories) const;

	/*! Searches for IP-XACT files and returns any found targets.
	*
	*      @return The found IP-XACT targets.
	*/
    QVector<LoadTarget> parseLibrary(MessageMediator const* messageChannel) const;

private:

    /*! Finds the VLNV in the given file.
    *
    *      @param [in]		path	The file path to search.
    *
    *      @return The VLNV found in the given file.
    */
    VLNV getDocumentVLNV(QString const& path, MessageMediator const* messageChannel) const;

    /*! Clear the empty directories from the disk within given path.
     *
     *      @param [in] dirPath The path that is removed until one of the base library locations is reached.
     *      @param [in] libraryLocations Contains the base library locations user has defined.
     *
    */
    void clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations) const;

    /*! Check if the path contains one of the given paths.
     *
     *      @param [in] path The path to a directory.
     *      @param [in] pathsToSearch Paths that are checked if they are contained in the given path.
     *
    */
    bool containsPath(QString const& path, QStringList const& pathsToSearch) const;
};

Q_DECLARE_TYPEINFO(LibraryLoader::LoadTarget, Q_MOVABLE_TYPE);

#endif // LIBRARYLOADER_H
