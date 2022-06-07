//-----------------------------------------------------------------------------
// File: PluginUtilityAdapter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 7.7.2014
//
// Description:
// Generic utility for passing parent window and library interface for generator plugins.
//-----------------------------------------------------------------------------

#include "PluginUtilityAdapter.h"

#include <KactusAPI/include/MessageMediator.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::PluginUtilityAdapter()
//-----------------------------------------------------------------------------
PluginUtilityAdapter::PluginUtilityAdapter(LibraryInterface* libraryInterface, MessageMediator* messageChannel,
    QString const& versionString, QWidget* parentWidget) :
libraryInterface_(libraryInterface),
    messageChannel_(messageChannel),
    parentWidget_(parentWidget),
    versionString_(versionString)
{

}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::printError()
//-----------------------------------------------------------------------------
void PluginUtilityAdapter::printError(QString const& message)
{
    messageChannel_->showError(message);
}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::printInfo()
//-----------------------------------------------------------------------------
void PluginUtilityAdapter::printInfo(QString const& message)
{
    messageChannel_->showMessage(message);
}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::getLibraryInterface()
//-----------------------------------------------------------------------------
LibraryInterface* PluginUtilityAdapter::getLibraryInterface()
{
    return libraryInterface_;
}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::getParentWidget()
//-----------------------------------------------------------------------------
QWidget* PluginUtilityAdapter::getParentWidget()
{
    return parentWidget_;
}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::getKactusVersion()
//-----------------------------------------------------------------------------
QString PluginUtilityAdapter::getKactusVersion() const
{
	return versionString_;
}