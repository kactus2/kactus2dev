//-----------------------------------------------------------------------------
// File: SVDGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 14.04.2021
//
// Description:
// Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------

#ifndef SVDGENERATORPLUGIN_H
#define SVDGENERATORPLUGIN_H

#include "svdgeneratorplugin_global.h"

#include <KactusAPI/include/CLIGenerator.h>
#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/IGeneratorPlugin.h>
#include <Plugins/SVDGenerator/ConnectivityGraphUtilities.h>

//-----------------------------------------------------------------------------
//! Plugin for creating a CMSIS System View Description listing.
//-----------------------------------------------------------------------------
class SVDGENERATORPLUGIN_EXPORT SVDGeneratorPlugin : public QObject, public IGeneratorPlugin, public CLIGenerator
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.svdgeneratorplugin" FILE "svdgeneratorplugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:

	/*!
     *  The constructor.
     */
	SVDGeneratorPlugin();

	/*!
     *  The destructor.
     */
    virtual ~SVDGeneratorPlugin() = default;

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
     *  Returns the licence holder of the plugin.
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
     *  Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();

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
     * Runs the generation.
     *
     *     @param [in] utility              Utilities for enabling plugin execution.
     *     @param [in] component            The component to run the generation for.
     *     @param [in] design               The design to run the generation for.
     *     @param [in] designConfiguration  The design configuration to run the generation for.
     *     @param [in] viewName             The component view name to run the generation for.
     *     @param [in] outputDirectory      The output directory for the generation results.
     */
    virtual void runGenerator(IPluginUtility* utility,
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration,
        QString const& viewName,
        QString const& outputDirectory) override final;

    /*!
     *  Gets the output format generated by the plugin.
     *
     *      @return The output file format generated by the plugin.
     */
    virtual QString getOutputFormat() const;

private:
	// Disable copying.
	SVDGeneratorPlugin(SVDGeneratorPlugin const& rhs);
    SVDGeneratorPlugin& operator=(SVDGeneratorPlugin const& rhs);

    /*!
     *  Saves the generated file to the selected file set of the top component.
     *
     *      @param [in] utility         The plugin utility interface.
     *      @param [in] svdFiles        The generated file paths.
     *      @param [in] component       The top level component.
     *      @param [in] fileSetName     Name of the target file set.
     */
    void saveToFileset(IPluginUtility* utility, QStringList const& svdFiles, QSharedPointer<Component> component,
        QString const& fileSetName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

};

#endif // SVDGENERATORPLUGIN_H