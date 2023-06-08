//-----------------------------------------------------------------------------
// File: FileTypes.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------

#include "FileTypes.h"

//-----------------------------------------------------------------------------
// Function: FileTypes::isIpXactFileType()
//-----------------------------------------------------------------------------
bool FileTypes::isIpXactFileType( const QString& fileType )
{
	return FILE_TYPES.contains(fileType);
}

//-----------------------------------------------------------------------------
// Function: FileTypes::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileTypes::getFileTypes( QSettings& settings, const QString& fileSuffix )
{
    QStringList types;

	settings.beginGroup(QStringLiteral("FileTypes"));
	for (QString const& type : settings.childGroups())
    {
		QString extensions = settings.value(type + QStringLiteral("/Extensions")).toString();

		for (QString const& extension : extensions.split(QLatin1Char(';'), Qt::SkipEmptyParts))
        {
			if (extension.compare(fileSuffix, Qt::CaseInsensitive) == 0)
            {
				types.append(type);
			}
		}
	}
    settings.endGroup();

	return types;
}

//-----------------------------------------------------------------------------
// Function: generaldeclarations::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileTypes::getFileTypes( QSettings& settings, const QFileInfo& file )
{
    return FileTypes::getFileTypes(settings, file.suffix());
}