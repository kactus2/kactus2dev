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
    explicit LibraryLoader(MessageMediator* messageChannel, QObject* parent = nullptr);

     ~LibraryLoader() = default;

    struct LoadTarget
    {
        QString path;
        VLNV vlnv;

        LoadTarget(VLNV targetVLNV = VLNV(), QString targetPath = QString()): path(targetPath), vlnv(targetVLNV) {}
    };

    void clean(QStringList changedDirectories);

    QVector<LoadTarget> parseLibrary();

signals:

public slots:

private:

    void parseDirectory(QString const& directoryPath, QVector<LoadTarget>& vlnvPaths);

    void parseFile(QString const& filePath, QVector<LoadTarget>& vlnvPaths);

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


    MessageMediator* messageChannel_;

    DocumentFileAccess fileAccess_;

};

#endif // LIBRARYLOADER_H
