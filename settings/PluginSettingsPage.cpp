//-----------------------------------------------------------------------------
// File: PluginSettingsPage.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 19.02.2013
//
// Description:
// Settings page for adjusting plugin settings.
//-----------------------------------------------------------------------------

#include "PluginSettingsPage.h"

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::PluginSettingsPage(QSettings& settings)
    : settings_(settings)
{

}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::~PluginSettingsPage()
//-----------------------------------------------------------------------------
PluginSettingsPage::~PluginSettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::validate()
//-----------------------------------------------------------------------------
bool PluginSettingsPage::validate()
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::apply()
//-----------------------------------------------------------------------------
void PluginSettingsPage::apply()
{
}

//-----------------------------------------------------------------------------
// Function: PluginSettingsPage::onPageChange()
//-----------------------------------------------------------------------------
bool PluginSettingsPage::onPageChange()
{
    return true;
}
