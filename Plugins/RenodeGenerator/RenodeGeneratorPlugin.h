//-----------------------------------------------------------------------------
// File: RenodeGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 10.02.2023
//
// Description:
// Renode generator plugin.
//-----------------------------------------------------------------------------

#ifndef RENODEGENERATORPLUGIN_H
#define RENODEGENERATORPLUGIN_H

#include "renodegeneratorplugin_global.h"

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/IGeneratorPlugin.h>
#include <KactusAPI/include/IPluginUtility.h>
#include <KactusAPI/include/CLIGenerator.h>

class LibraryInterface;
class FileSet;
class File;

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

//-----------------------------------------------------------------------------
//! Renode generator plugin.
//-----------------------------------------------------------------------------
class RENODEGENERATORPLUGIN_EXPORT RenodeGeneratorPlugin : public QObject, public IGeneratorPlugin, public CLIGenerator
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.renodeGeneratorPlugin" FILE "renodeGeneratorPlugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:
    
    /*!
     *  The constructor.
     */
    RenodeGeneratorPlugin();
    
    /*!
     * The destructor.
     */
    ~RenodeGeneratorPlugin() = default;

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
    virtual QString getLicense() const;

    /*!
     *  Returns the holder of the licence of the plugin.
     */
    virtual QString getLicenseHolder() const;

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
        QSharedPointer<Design const> design,
        QSharedPointer<DesignConfiguration const> designConfiguration) const;

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
    virtual void runGenerator(IPluginUtility* utility,
        QSharedPointer<Component> component,
        QSharedPointer<Design> design,
        QSharedPointer<DesignConfiguration> designConfiguration);

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
     *  Returns the external program requirements of the plugin.
     * */
    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();
    
    /*!
     *  Gets the output format generated by the plugin.
     *
     *      @return The output file format generated by the plugin.
     */
    virtual QString getOutputFormat() const;

public slots:

    /*!
     *  Called when an error is reported to us.
     *
	 *      @param [in] report			The error message.
	 */
    void onErrorReport(const QString& report);

private:

    /*!
     *  Saves the generated file to the selected file set of the top component.
     *
     *      @param [in] utility         The plugin utility interface.
     *      @param [in] svdFiles        The generated file paths.
     *      @param [in] component       The top level component.
     *      @param [in] fileSetName     Name of the target file set.
     */
    void saveToFileset(IPluginUtility* utility,
        QStringList const& renodeFiles,
        QSharedPointer<Component> component,
        QString const& fileSetName);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
};

#endif // RENODEGENERATORPLUGIN_H
