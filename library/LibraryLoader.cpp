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
#include <QFileInfo>
#include <QSettings>
#include <QXmlStreamReader>

//-----------------------------------------------------------------------------
// Function: LibraryLoader::LibraryLoader()
//-----------------------------------------------------------------------------
LibraryLoader::LibraryLoader(MessageMediator* messageChannel, QObject* parent) : QObject(parent),
    messageChannel_(messageChannel),
    fileAccess_(messageChannel)
{

}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::parseLibrary()
//-----------------------------------------------------------------------------
QVector<LibraryLoader::LoadTarget> LibraryLoader::parseLibrary()
{
    QVector<LoadTarget> vlnvPaths;

    QStringList locations = QSettings().value("Library/ActiveLocations", QStringList()).toStringList();

    for (QString const& location : locations)
    {
        parseDirectory(location, vlnvPaths);
    }

    return vlnvPaths;
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::clean()
//-----------------------------------------------------------------------------
void LibraryLoader::clean(QStringList changedDirectories)
{
    QStringList libraryLocations = QSettings().value("Library/Locations", QStringList()).toStringList();

    changedDirectories.removeDuplicates();
    for (QString const& changedDirectory : changedDirectories)
    {
        clearDirectoryStructure(changedDirectory, libraryLocations);
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::parseDirectory()
//-----------------------------------------------------------------------------
void LibraryLoader::parseDirectory(QString const& directoryPath, QVector<LoadTarget>& vlnvPaths)
{
    QDir directoryHandler(directoryPath);
    directoryHandler.setNameFilters(QStringList(QLatin1String("*.xml")));

    // Get list of files and folders.
    for (QFileInfo const& entryInfo :
        directoryHandler.entryInfoList(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::Readable))
    {
        if (entryInfo.isFile())
        {
            parseFile(entryInfo.absoluteFilePath(), vlnvPaths);
        }
        else if (entryInfo.isDir())
        {
            parseDirectory(entryInfo.absoluteFilePath(), vlnvPaths);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: LibraryLoader::parseFile()
//-----------------------------------------------------------------------------
void LibraryLoader::parseFile(QString const& filePath, QVector<LoadTarget>& vlnvPaths)
{
    VLNV vlnv = getDocumentVLNV(filePath);

    if (vlnv.isValid() == false)
    {
        return;
    }

    vlnvPaths.append(LoadTarget(vlnv, filePath));
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
void LibraryLoader::clearDirectoryStructure(QString const& dirPath, QStringList const& libraryLocations)
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
    foreach (QString const& searchPath, pathsToSearch)
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
