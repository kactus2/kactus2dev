//-----------------------------------------------------------------------------
// File: SettingsUpdater.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 15.01.2013
//
// Description:
// Settings updater which upgrades Kactus2 settings up-to-date based on a
// configuration file.
//-----------------------------------------------------------------------------

#ifndef SETTINGSUPDATER_H
#define SETTINGSUPDATER_H

#include <QWidget>

//-----------------------------------------------------------------------------

namespace SettingsUpdater
{
    /*!
     *  Upgrades the settings if not up-to-date.
     */
    void runUpgrade(QWidget* parent);
}

#endif // SETTINGSUPDATER_H
