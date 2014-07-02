//-----------------------------------------------------------------------------
// File: SettingsPage.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 01.07.2014
//
// Description:
// Base class for all settings pages.
//-----------------------------------------------------------------------------

#include "SettingsPage.h"

//-----------------------------------------------------------------------------
// Function: SettingsPage()
//-----------------------------------------------------------------------------
SettingsPage::SettingsPage(QSettings& settings) : 
    PropertyPageView(),
    settings_(settings)    
{

}

//-----------------------------------------------------------------------------
// Function: ~SettingsPage()
//-----------------------------------------------------------------------------
SettingsPage::~SettingsPage()
{

}

//-----------------------------------------------------------------------------
// Function: onPageChange()
//-----------------------------------------------------------------------------
bool SettingsPage::onPageChange()
{
    // Do not change the page if the settings are invalid.
    return validate();
}

//-----------------------------------------------------------------------------
// Function: SettingsPage::settings()
//-----------------------------------------------------------------------------
QSettings& SettingsPage::settings() const
{
    return settings_;
}
