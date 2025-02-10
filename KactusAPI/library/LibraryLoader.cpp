//-----------------------------------------------------------------------------
// File: LibraryLoader.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 07.04.2018
//
// Description:
// Class for IP-XACT file search and type parsing.
//-----------------------------------------------------------------------------

#include "LibraryLoader.h"

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QSettings>
#include <QXmlStreamReader>

//-----------------------------------------------------------------------------
// Function: LibraryLoader::parseLibrary()
//-----------------------------------------------------------------------------
QVector<LibraryLoader::LoadTarget> LibraryLoader::parseLibrary(MessageMediator const* messageChannel) const
{
    QVector<LoadTarget> vlnvPaths;
    QStringList xmlFilter{ QStringLiteral("*.xml") };

    for (QString const& location : QSettings().value(QStringLiteral("Library/ActiveLocations")).toStringList())
    {        
        QDirIterator fileIterator(location, xmlFilter, QDir::Files,
            QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);

        while (fileIterator.hasNext())
        {
            QString filePath(fileIterator.next());

            VLNV vlnv = getDocumentVLNV(filePath, messageChannel);
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
VLNV LibraryLoader::getDocumentVLNV(QString const& path, MessageMediator const* messageChannel) const
{
    QFile documentFile(path);
    if (!documentFile.open(QFile::ReadOnly))
    {
        messageChannel->showError(QObject::tr("File %1 could not be read.").arg(path));
        return VLNV();
    }

    QXmlStreamReader documentReader(&documentFile);
    documentReader.readNextStartElement();

    QString type = documentReader.qualifiedName().toString();
    if (type.startsWith(QStringLiteral("spirit:")))
    {
        messageChannel->showMessage(QObject::tr("File %1 contains an IP-XACT description not compatible "
            "with the supported standards and could not be read.").arg(path));
        documentFile.close();
        return VLNV();
    }

    if (!type.startsWith(QStringLiteral("ipxact:")) && !type.startsWith(QStringLiteral("kactus2:")))
    {
        return VLNV();
    }

    // Find the first element of the VLVN.
    while (documentReader.readNextStartElement() &&
        documentReader.qualifiedName().compare(QStringLiteral("ipxact:vendor")) != 0)
    {
        // Empty loop on purpose.
    }

    QString vlnvString = documentReader.readElementText();
    for (int i = 0; i < 3; ++i)
    {
        documentReader.readNextStartElement();
        vlnvString.append(':');
        vlnvString.append(documentReader.readElementText());
    }

    documentFile.close();

    VLNV documentVLNV(VLNV::string2Type(type), vlnvString);
    if (!documentVLNV.isValid())
    {
        messageChannel->showError(QObject::tr("File %1 contains an invalid IP-XACT identifier %2.").arg(path,
            vlnvString));
    }

    return documentVLNV;
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
    // As long as the path is not the same as search path but still contains the search path,
    // it is a parent directory of the path.
    return std::any_of(pathsToSearch.cbegin(), pathsToSearch.cend(), [&path](auto const& searchPath)
        {
            return path.contains(searchPath) && path.compare(searchPath) != 0;
        });
}
