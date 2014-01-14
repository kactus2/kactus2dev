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
#include <QList>
#include <QtPlugin>

//-----------------------------------------------------------------------------
//! Plugin information interface.
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
     *  Returns the vendor of the plugin.
     */
    virtual QString const& getVendor() const = 0;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString const& getLicence() const = 0;

    /*!
     *  Returns the licence holder of the plugin.
     */
    virtual QString const& getLicenceHolder() const = 0;

    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget() = 0;

	//! \brief Returns the external program requirements of the plugin.
	 virtual QList<ExternalProgramRequirement> getProgramRequirements() = 0;
};

Q_DECLARE_INTERFACE(IPlugin, "com.tut.Kactus2.IPlugin/1.0")

//-----------------------------------------------------------------------------

#endif // IPLUGIN_H
