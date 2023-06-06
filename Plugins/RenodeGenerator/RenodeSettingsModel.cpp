//-----------------------------------------------------------------------------
// File: RenodeSettingsModel.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 05.06.2023
//
// Description:
// Interface for renode generator settings model.
//-----------------------------------------------------------------------------

#include "RenodeSettingsModel.h"

#include <QFile>

#include <QDir>
#include <QCoreApplication>

namespace
{
    //! Key for peripheral template configuration folder.
	QString const TEMPLATEFOLDERKEY = "PythonPeripheralTemplateFolder";
};

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::RenodeSettingsModel()
//-----------------------------------------------------------------------------
RenodeSettingsModel::RenodeSettingsModel():
PluginSettingsModel()
{

}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::loadSettings()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::loadSettings(QSettings& settings)
{
// 	QString currentPath = QDir::currentPath();
// 	QString executablePath = QCoreApplication::applicationDirPath();

	templateFolderPath_ = settings.value(TEMPLATEFOLDERKEY).toString();
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::saveSettings()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::saveSettings(QSettings& settings)
{
// 	bool testi = false;

	if (templateFolderPath_.isEmpty())
	{
		settings.remove(TEMPLATEFOLDERKEY);
	}
	else
	{
		settings.setValue(TEMPLATEFOLDERKEY, templateFolderPath_);
	}
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::validateSettings()
//-----------------------------------------------------------------------------
bool RenodeSettingsModel::validateSettings() const
{
	return true;
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::getFolderPath()
//-----------------------------------------------------------------------------
QString RenodeSettingsModel::getFolderPath() const
{
	return templateFolderPath_;
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::setFolderPath()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::setFolderPath(QString const& newFolderPath)
{
	templateFolderPath_ = newFolderPath;
}
