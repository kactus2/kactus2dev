//-----------------------------------------------------------------------------
// File: LibraryLoader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.04.2018
//
// Description:
//
//-----------------------------------------------------------------------------

#include "LibraryLoader.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QXmlStreamReader>

//-----------------------------------------------------------------------------
// Function: LibraryLoader::LibraryLoader()
//-----------------------------------------------------------------------------
LibraryLoader::LibraryLoader(MessageMediator* messageChannel) : 
    messageChannel_(messageChannel)
{

}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::parseLibrary()
//-----------------------------------------------------------------------------
QVector<LibraryLoader::LoadTarget> LibraryLoader::parseLibrary()
{
    QVector<LoadTarget> vlnvPaths;
    QStringList xmlFilter(QLatin1String("*.xml"));

    QStringList locations = QSettings().value(QStringLiteral("Library/ActiveLocations")).toStringList();    
    for (QString const& location : locations)
    {        
        QDirIterator fileIterator(location, xmlFilter, QDir::Files,
            QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while (fileIterator.hasNext())
        {
            QString filePath(fileIterator.next());

            VLNV vlnv = getDocumentVLNV(filePath);
            if (vlnv.isValid())
            {
                vlnvPaths.append(LoadTarget(vlnv, filePath));
            }
        }
    }

    return vlnvPaths;
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::clean()
//-----------------------------------------------------------------------------
void LibraryLoader::clean(QStringList const& changedDirectories) const
{
    QStringList libraryLocations = QSettings().value(QStringLiteral("Library/Locations")).toStringList();

    for (QString const& changedDirectory : changedDirectories)
    {
        clearDirectoryStructure(changedDirectory, libraryLocations);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::getDocumentVLNV()
//-----------------------------------------------------------------------------
VLNV LibraryLoader::getDocumentVLNV(QString const& path)
{
    QFile documentFile(path);
    if (!documentFile.open(QFile::ReadOnly))
    {
        messageChannel_->showError(QObject::tr("File %1 could not be read.").arg(path));
        return VLNV();
    }

    QXmlStreamReader documentReader(&documentFile);
    documentReader.readNextStartElement();

    QString type = documentReader.qualifiedName().toString();
    if (type.startsWith(QLatin1String("spirit:")))
    {
        messageChannel_->showMessage(QObject::tr("File %1 contains an IP-XACT description not compatible "
            "with the 1685-2014 standard and could not be read.").arg(path));
        documentFile.close();
        return VLNV();
    }

    // Find the first element of the VLVN.
    while(documentReader.readNextStartElement() &&
        documentReader.qualifiedName().compare(QLatin1String("ipxact:vendor")) != 0)
    {
        // Empty loop on purpose.
    }

    QString vendor = documentReader.readElementText();

    documentReader.readNextStartElement();
    QString library = documentReader.readElementText();

    documentReader.readNextStartElement();
    QString name = documentReader.readElementText();

    documentReader.readNextStartElement();
    QString version = documentReader.readElementText();
    
    documentFile.close();

    return VLNV(type, vendor, library, name, version);
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::clearDirectoryStructure()
//-----------------------------------------------------------------------------
void LibraryLoader::clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations) const
{
    QDir dir(dirPath);

    while (containsPath(QDir::cleanPath(dir.absolutePath()), libraryLocations))
    {
        QString directoryName = dir.dirName();

        // if not possible to move up anymore (the dir could possibly have been destroyed already).
        if (!dir.cdUp())
        {
            return;
        }

        // if the directory is not empty then it can't be removed and we can stop.
        if (!dir.rmdir(directoryName))
        {
            return;
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::containsPath()
//-----------------------------------------------------------------------------
bool LibraryLoader::containsPath(QString const& path, QStringList const& pathsToSearch) const
{
    for (QString const& searchPath : pathsToSearch)
    {
        // As long as the path is not the same as search path but still contains the search path,
        // it is a parent directory of the path.
        if (path.contains(searchPath) && path.compare(searchPath) != 0)
        {
            return true;
        }
    }

    // None of the paths to search were contained in the path.
    return false;
}
