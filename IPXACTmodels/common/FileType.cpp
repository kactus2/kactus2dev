//-----------------------------------------------------------------------------
// File: FileType.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 10.09.2015
//
// Description:
// NameSpace FileTypes is used to store functions related to file types.
//-----------------------------------------------------------------------------

#include "FileType.h"

//-----------------------------------------------------------------------------
// Function: FileTypes::isIpXactFileType()
//-----------------------------------------------------------------------------
bool FileTypes::isIpXactFileType(QString const& fileType, Document::Revision docRevision )
{
	if (docRevision == Document::Revision::Std14)
	{
		return FILE_TYPES.contains(fileType);
	}
	else if (docRevision == Document::Revision::Std22)
	{

		return FILE_TYPES.contains(fileType) || FILE_TYPES_2022.contains(fileType);
	}
	else
	{
		return false;
	}
}

//-----------------------------------------------------------------------------
// Function: FileType::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileTypes::getFileTypes( QSettings& settings, QString const& fileSuffix )
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
// Function: FileType::getFileTypes()
//-----------------------------------------------------------------------------
QStringList FileTypes::getFileTypes( QSettings& settings, QFileInfo const& file )
{
    return FileTypes::getFileTypes(settings, file.suffix());
}
