//-----------------------------------------------------------------------------
// File: FileHandler.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 08.10.2019
//
// Description:
// Handles data regarding files.
//-----------------------------------------------------------------------------

#include "FileHandler.h"

#include <IPXACTmodels/generaldeclarations.h>

#include <QFileInfo>
#include <QProcessEnvironment>
#include <QRegularExpression>
#include <QSettings>

//-----------------------------------------------------------------------------
// Function: FileHandler::constructFileSuffixTable()
//-----------------------------------------------------------------------------
QMultiMap<QString, QString> FileHandler::constructFileSuffixTable()
{
    QMultiMap<QString, QString> fileSuffixTable;

    QSettings settings;
    settings.beginGroup(QStringLiteral("FileTypes"));
    for (QString const& fileType : settings.childGroups())
    {
        settings.beginGroup(fileType);

        QStringList fileTypeExtensions = settings.value(QStringLiteral("Extensions")).toString().split(QLatin1Char(';'));
        for (QString const& extension : fileTypeExtensions)
        {
            fileSuffixTable.insert(extension, fileType);
        }
        settings.endGroup();
    }

    settings.endGroup();

    return fileSuffixTable;
}

//-----------------------------------------------------------------------------
// Function: FileHandler::getFileTypeForSuffix()
//-----------------------------------------------------------------------------
QString FileHandler::getFileTypeForSuffix(QMultiMap<QString, QString> const& fileSuffixTable, QString const& suffix)
{
    QMultiMapIterator<QString, QString> tableIterator(fileSuffixTable);
    while (tableIterator.hasNext())
    {
        tableIterator.next();

        if (tableIterator.key() == suffix)
        {
            return tableIterator.value();
        }
    }

    return QString();
}


//-----------------------------------------------------------------------------
// Function: FilesModel::resolvePath()
//-----------------------------------------------------------------------------
QString FileHandler::resolvePath(QString const& ipxactURI)
{
    auto resolvedURI = resolveURI(ipxactURI);

    QFileInfo fileInfo(resolvedURI);
    return fileInfo.filePath();
}

//-----------------------------------------------------------------------------
// Function: FilesModel::resolveURI()
//-----------------------------------------------------------------------------
QString FileHandler::resolveURI(QString const& ipxactURI)
{
    auto resolvedURI = ipxactURI;

    QRegularExpression envVariable("[$]{([^}]*)}");

    auto matchIterator = envVariable.globalMatch(resolvedURI);
    while (matchIterator.hasNext())
    {
        auto match = matchIterator.next();
        auto variableName = match.captured(1);
        auto value = QProcessEnvironment::systemEnvironment().value(variableName);

        resolvedURI.replace(match.captured(0), value);
    }

    return resolvedURI;
}

//-----------------------------------------------------------------------------
// Function: FilesModel::isValidURI()
//-----------------------------------------------------------------------------
bool FileHandler::isValidURI(QString const& relativeTo, QString const& ipxactURI)
{
    auto resolvedURI = resolveURI(ipxactURI);

    return filePathExists(relativeTo, resolvedURI) || isURI(resolvedURI);
}

//-----------------------------------------------------------------------------
// Function: FileHandler::filePathExists()
//-----------------------------------------------------------------------------
bool FileHandler::filePathExists(QString const& relativeTo, QString const& filePath)
{
    if (filePath.isEmpty())
    {
        return false;
    }

    QString absFilePath = General::getAbsolutePath(relativeTo, filePath);

    QFileInfo fileInfo(absFilePath);

    return fileInfo.exists();
}

//-----------------------------------------------------------------------------
// Function: FileHandler::isURI()
//-----------------------------------------------------------------------------
bool FileHandler::isURI(QString const& uri)
{
    return URL_VALIDITY_REG_EXP.match(uri).hasMatch();
}
