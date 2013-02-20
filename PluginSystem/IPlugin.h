//-----------------------------------------------------------------------------
// File: IPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Base plugin interface.
//-----------------------------------------------------------------------------

#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "PluginSettingsWidget.h"

#include <QString>
#include <QtPlugin>

//-----------------------------------------------------------------------------
//! Plugin information interface.
//-----------------------------------------------------------------------------
class IPlugin
{
public:
    /*!
     *  Destructor.
     */
    virtual ~IPlugin() {}

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString const& getName() const = 0;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const = 0;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const = 0;

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget() = 0;
};

Q_DECLARE_INTERFACE(IPlugin, "com.tut.Kactus2.IPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IPLUGIN_H
