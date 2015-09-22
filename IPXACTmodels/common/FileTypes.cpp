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
	for (unsigned int i = 0; i < FileTypes::FILE_TYPE_COUNT; ++i)
    {
		if (QString(FileTypes::FILE_TYPES[i]) == fileType)
        {
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Function: FileTypes::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileTypes::getFileTypes( QSettings& settings, const QString& fileSuffix )
{
    QStringList types;

	settings.beginGroup("FileTypes");
	foreach (QString const& type, settings.childGroups())
    {
		QString extensions = settings.value(type + "/Extensions").toString();

		foreach (QString extension, extensions.split(";", QString::SkipEmptyParts))
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