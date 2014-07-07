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
#include <library/LibraryManager/libraryinterface.h>

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::PluginUtilityAdapter()
//-----------------------------------------------------------------------------
PluginUtilityAdapter::PluginUtilityAdapter(LibraryInterface* libraryInterface, QWidget* parentWidget, 
    QObject* parent) : 
      QObject(parent),
      libraryInterface_(libraryInterface),
      parentWidget_(parentWidget)
{

}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::~PluginUtilityAdapter()
//-----------------------------------------------------------------------------
PluginUtilityAdapter::~PluginUtilityAdapter()
{

}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::printError()
//-----------------------------------------------------------------------------
void PluginUtilityAdapter::printError(QString const& message)
{
    emit errorMessage(message);
}

//-----------------------------------------------------------------------------
// Function: PluginUtilityAdapter::printInfo()
//-----------------------------------------------------------------------------
void PluginUtilityAdapter::printInfo(QString const& message)
{
    emit infoMessage(message);
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
