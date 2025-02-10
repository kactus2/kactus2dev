//-----------------------------------------------------------------------------
// File: IPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 27.6.2012
//
// Description:
// Base plugin interface.
//-----------------------------------------------------------------------------

#ifndef IPLUGIN_H
#define IPLUGIN_H

#include "PluginSettingsModel.h"

#include <QString>
#include <QList>
#include <QtPlugin>
#include <QWidget>

//-----------------------------------------------------------------------------
//! All plugin interfaces inherit from a common IPlugin interface which all plugins must implement.
//! It is used by Kactus2 to acquire plugin information.
//! Most of the plugins do not inherit this interface directly, but some derived interface.
//-----------------------------------------------------------------------------
class IPlugin
{
public:

	//! \brief Contains options for required external program paths for plugin.
	struct ExternalProgramRequirement {
		
		//! \brief Identifies the program to run.
		QString name_;

		//! \brief Explains the required program for user.
		QString description_;

		//! \brief Contains the filters for the QFileDialog to select correct type of files.
		QString filters_;
	};

    /*!
     *  Destructor.
     */
    virtual ~IPlugin() {}

    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const = 0;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const = 0;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString getDescription() const = 0;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const = 0;

    /*!
     *  Returns the license of the plugin (e.g. GPL2).
     */
    virtual QString getLicense() const = 0;

    /*!
     *  Returns the license holder i.e. who is the plugin licensed to (e.g. Public or Tampere University of Technology).
     */
    virtual QString getLicenseHolder() const = 0;

    /*!
     *  This can be used to show custom plugin-specific settings UI in the Kactus2 settings dialog.
	 *  A QWidget should be returned even if the plugin has no custom settings.
     */
    virtual QWidget* getSettingsWidget() = 0;

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel() = 0;

    /*!
     *  This can be used to indicate what external programs the plugin needs in order to operate correctly.
	 *  This allows Kactus2 to show settings to set paths where those external program are located.
     */
	virtual QList<ExternalProgramRequirement> getProgramRequirements() = 0;
};

Q_DECLARE_INTERFACE(IPlugin, "com.tut.Kactus2.IPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IPLUGIN_H
