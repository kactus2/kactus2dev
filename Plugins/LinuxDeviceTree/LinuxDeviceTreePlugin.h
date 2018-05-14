//-----------------------------------------------------------------------------
// File: LinuxDeviceTreePlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Copyright: Tampere University of Technology
// Author: Mikko Teuho
// Date: 08.05.2018
//
// Description:
// Generator plugin for Linux Device Tree.
//-----------------------------------------------------------------------------

#ifndef LINUXDEVICETREEPLUGIN_H
#define LINUXDEVICETREEPLUGIN_H

#include "LinuxDeviceTreeGenerator_global.h"

#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/IPluginUtility.h>

#include <QObject>

//-----------------------------------------------------------------------------
//! Generator plugin for Linux Device Tree.
//-----------------------------------------------------------------------------
class LINUXDEVICETREEGENERATOR_EXPORT LinuxDeviceTreePlugin : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
        Q_PLUGIN_METADATA(IID "kactus2.plugins.LinuxDeviceTreePlugin" FILE "LinuxDeviceTreePlugin.json")

        Q_INTERFACES(IPlugin)
        Q_INTERFACES(IGeneratorPlugin)

public:

    /*!
     *  The constructor.
     */
    LinuxDeviceTreePlugin();

    /*!
     *  The destructor.
     */
    ~LinuxDeviceTreePlugin();
    
    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenceHolder() const;

    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;
    
    /*!
     *  Checks whether the generator may run for the given component or design. 
     *
     *      @param [in] component	        The component for which to check support. If design is not null, component
	 *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     *
     *      @return True, if the generator may run the given component. Otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<Component const> component,
        QSharedPointer<Design const> design, QSharedPointer<DesignConfiguration const> designConfiguration) const;
     
    /*!
     *  Runs the generation, creating new files and/or modifying the IP-XACT metadata. The function has
	 *  also access to the parent window widget, so that it can show dialogs for the user to aid the generation.
     *
     *      @param [in] utility			    The plugin utility interface.
     *      @param [in] component	        The component for which to check support. If design is not null, component
     *                                      will refer to design or designConfiguration.
     *      @param [in] design	            The design, if the generator is ran for a design.
     *      @param [in] designConfiguration The design configuration for design, if it is not null.
     */
    virtual void runGenerator(IPluginUtility* utility, QSharedPointer<Component> component,
        QSharedPointer<Design> design, QSharedPointer<DesignConfiguration> designConfiguration);

     /*!
      * Returns the external program requirements of the plugin.
      */
     virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();

public slots:

    /*!
     *  Called when an error is reported to us.
     *
	 *      @param [in] report			The error message.
	 */
    void onErrorReport(const QString& report);

private:
    //! The plugin utility provided by call runGenerator.
    IPluginUtility* utility_;
};

#endif // LINUXDEVICETREEPLUGIN_H
