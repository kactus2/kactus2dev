//-----------------------------------------------------------------------------
// File: PluginSettingsWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 20.02.2013
//
// Description:
// Base interface for plugin settings model.
//-----------------------------------------------------------------------------

#ifndef PLUGINSETTINGSMODEL_H
#define PLUGINSETTINGSMODEL_H

#include <QSettings>

//-----------------------------------------------------------------------------
//! Base interface for plugin settings model.
//-----------------------------------------------------------------------------
class PluginSettingsModel
{
public:
    /*!
     *  Constructor.
     */
    PluginSettingsModel() {}

    /*!
     *  Destructor.
     */
    virtual ~PluginSettingsModel() {}
    
    /*!
     *  Loads plugin settings from the given settings store. Is called when Kactus2 loads the plugin.
     *
     *      @param [in] settings The settings store.
     */
    virtual void loadSettings(QSettings& /*settings*/) {/* By default, do nothing. */}
    
    /*!
     *  Saves plugin settings to the given settings store. Is called when Kactus2 applies changes to settings.
     *
     *      @param [in,out] settings The settings store.
     */
    virtual void saveSettings(QSettings& /*settings*/) {/* By default, do nothing. */ }

    /*!
     *  Validates the settings of the plugin.
     */
    virtual bool validateSettings() const {return true;}
};

//-----------------------------------------------------------------------------

#endif // PLUGINSETTINGSMODEL_H
