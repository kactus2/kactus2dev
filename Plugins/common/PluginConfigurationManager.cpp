//-----------------------------------------------------------------------------
// File: PluginConfigurationManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring plugin editor configurations.
//-----------------------------------------------------------------------------

#include "PluginConfigurationManager.h"

//-----------------------------------------------------------------------------
// Function: PluginConfigurationManager::PluginConfigurationManager()
//-----------------------------------------------------------------------------
PluginConfigurationManager::PluginConfigurationManager(IPluginUtility* utility):
ConfigurationManager(utility->getLibraryInterface()),
utility_(utility)
{

}

//-----------------------------------------------------------------------------
// Function: PluginConfigurationManager::printError()
//-----------------------------------------------------------------------------
void PluginConfigurationManager::printError(QString const& errorMessage) const
{
    utility_->printError(errorMessage);
}
