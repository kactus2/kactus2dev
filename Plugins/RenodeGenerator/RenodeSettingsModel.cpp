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
	QString const TEMPLATEFILEKEY = "PythonPeripheralTemplateFile";

    //! Path to the default Renode generator peripheral template configuration.
    QString const DEFAULTPATH = QCoreApplication::applicationDirPath() + QString("/templates/defaultTemplateConfiguration.json");
};

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::RenodeSettingsModel()
//-----------------------------------------------------------------------------
RenodeSettingsModel::RenodeSettingsModel() :
PluginSettingsModel()
{

}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::loadSettings()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::loadSettings(QSettings& settings)
{
    templateFilePath_ = settings.value(TEMPLATEFILEKEY).toString();
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::saveSettings()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::saveSettings(QSettings& settings)
{
	if (templateFilePath_.isEmpty() || !QFile::exists(templateFilePath_))
	{
		settings.remove(TEMPLATEFILEKEY);
	}
	else
	{
		settings.setValue(TEMPLATEFILEKEY, templateFilePath_);
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
// Function: RenodeSettingsModel::getFilePath()
//-----------------------------------------------------------------------------
QString RenodeSettingsModel::getFilePath() const
{
    if (!templateFilePath_.isEmpty())
    {
        return templateFilePath_;
    }
    else
    {
        return DEFAULTPATH;
    }
}

//-----------------------------------------------------------------------------
// Function: RenodeSettingsModel::setFilePath()
//-----------------------------------------------------------------------------
void RenodeSettingsModel::setFilePath(QString const& newFolderPath)
{
    if (newFolderPath == DEFAULTPATH)
    {
        templateFilePath_.clear();
    }
    else
    {
        templateFilePath_ = newFolderPath;
    }
}
