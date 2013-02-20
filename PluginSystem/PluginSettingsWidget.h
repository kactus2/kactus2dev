//-----------------------------------------------------------------------------
// File: PluginSettingsWidget.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 20.02.2013
//
// Description:
// Base interface for plugin settings widgets.
//-----------------------------------------------------------------------------

#ifndef PLUGINSETTINGSWIDGET_H
#define PLUGINSETTINGSWIDGET_H

#include <QWidget>
#include <QSettings>

//-----------------------------------------------------------------------------
//! Base interface for plugin settings widgets.
//-----------------------------------------------------------------------------
class PluginSettingsWidget : public QWidget
{
public:
    /*!
     *  Constructor.
     */
    PluginSettingsWidget() {}

    /*!
     *  Destructor.
     */
    virtual ~PluginSettingsWidget() {}

    /*!
     *  Loads settings from the given settings store.
     *
     *      @param [in] settings The settings store.
     */
    virtual void loadSettings(QSettings& /*settings*/) {}

    /*!
     *  Saves settings to the given settings store.
     *
     *      @param [in,out] settings The settings store.
     */
    virtual void saveSettings(QSettings& /*settings*/) {}

    /*!
     *  Validates the contents of the widget.
     */
    virtual bool validate() const { return true; }
};

//-----------------------------------------------------------------------------

#endif // PLUGINSETTINGSWIDGET_H
