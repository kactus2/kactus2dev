//-----------------------------------------------------------------------------
// File: LibraryLoader.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.04.2018
//
// Description:
//
//-----------------------------------------------------------------------------

#ifndef LIBRARYLOADER_H
#define LIBRARYLOADER_H

#include "DocumentFileAccess.h"

#include <common/ui/MessageMediator.h>

#include <QObject>

class LibraryLoader : public QObject
{
	Q_OBJECT
public:

	/*! The constructor.
	*
	*      @param [in] messageChannel	The message channel for notifications.
	*      @param [in] parent			The parent object.	
	*/
    explicit LibraryLoader(MessageMediator* messageChannel, QObject* parent = nullptr);

	//! The destructor.
     ~LibraryLoader() = default;

	//! Struct for load targets.
    struct LoadTarget
    {
        QString path; //!< The path to IP-XACT file.
        VLNV vlnv;    //!< The VLNV defined in the file.

		//! Constructor.
        LoadTarget(VLNV targetVLNV = VLNV(), QString targetPath = QString()): path(targetPath), vlnv(targetVLNV) {}
    };

	/*! Cleans the directory structure.
	*
	*      @param [in] changedDirectories	Changed directories (e.g. after a remove).
	*/
    void clean(QStringList changedDirectories);

	/*! Searches for IP-XACT files and returns any found targets.
	*
	*      @return The found IP-XACT targets.
	*/
    QVector<LoadTarget> parseLibrary();

private:

	/*! Searches through a single directory for IP-XACT files.
	*
	*      @param [in]		directoryPath	The directory path to search.
	*      @param [in/out]	vlnvPaths		The already found targets where any new IP-XACT files will be added.
	*/
    void parseDirectory(QString const& directoryPath, QVector<LoadTarget>& vlnvPaths);

    /*! Searches through a single file for IP-XACT files.
	*
	*      @param [in]		filePath	The file path to search.
	*      @param [in/out]	vlnvPaths	The already found targets where any new IP-XACT files will be added.
	*/
    void parseFile(QString const& filePath, QVector<LoadTarget>& vlnvPaths);

    /*! Finds the VLNV in the given file.
    *
    *      @param [in]		path	The file path to search.
    *
    *      @return The VLNV found in the given file.
    */
    VLNV getDocumentVLNV(QString const& path);

    /*! Clear the empty directories from the disk within given path.
     *
     *      @param [in] dirPath The path that is removed until one of the base library locations is reached.
     *      @param [in] libraryLocations Contains the base library locations user has defined.
     *
    */
    void clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations);

    /*! Check if the path contains one of the given paths.
     *
     *      @param [in] path The path to a directory.
     *      @param [in] pathsToSearch Paths that are checked if they are contained in the given path.
     *
    */
    bool containsPath(QString const& path, QStringList const& pathsToSearch) const;

    //! Channel for user notifications.
    MessageMediator* messageChannel_;

    //! Utilities for file access.
    DocumentFileAccess fileAccess_;
};


#endif // LIBRARYLOADER_H
