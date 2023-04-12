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

class RenodeCPUDetailRoutes;
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
     *  Get the configuration object of a single CPU.
     *
     *      @param [in] component   The containing component.
     *      @param [in] utility     Utilities for enabling plugin execution.
     *
     *      @return The configuration object.
     */
    QJsonObject getConfigurationObject(QSharedPointer<Component> component, IPluginUtility* utility);

    /*!
     *  Get the configuration document.
     *
     *      @param [in] component   The containing component.
     *      @param [in] utility     Utilities for enabling plugin execution.
     *
     *      @return The configuration document.
     */
    QJsonDocument getConfigurationDocument(QSharedPointer<Component> component, IPluginUtility* utility);

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

    /*!
     *  Create a configuration file.
     *
     *      @param [in] renodeData          Container for renode generator data.
     *      @param [in] selectedView        Name of the view to use in the generator.
     *      @param [in] saveToFileSet       Flag for save to file set.
     *      @param [in] selectedFileSet     Name of the target file set.
     *      @param [in] folderPath          Path for the renode files.
     *      @param [in] writeCPU            Flag for writing a CPU file.
     *      @param [in] writeMemory         Flag for writing a memory file.
     *      @param [in] writePeripherals    Flag for writing a peripherals file.
     *      @param [in] library             Interface for accessing the library.
     *      @param [in] topComponent        Top level component.
     */
    void createConfigureFile(QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeData, QString const& selectedView,
        bool saveToFileSet, QString const& selectedFileSet, QString const& folderPath, bool writeCPU, bool writeMemory, bool writePeripherals,
        LibraryInterface* library, QSharedPointer<Component> topComponent);

    /*!
     *  Get the file set for configurations.
     *
     *      @param [in] topComponent    Top level component.
     *
     *      @return File set for configurations.
     */
    QSharedPointer<FileSet> getConfigurationFileSet(QSharedPointer<Component> topComponent);

    /*!
     *  Get the configuration file for renode generator.
     *
     *      @param [in] configurationFileSet    File set for configurations.
     *
     *      @return Configuration file for renode generator.
     */
    QSharedPointer<File> getConfigurationFile(QSharedPointer<FileSet> configurationFileSet);

    /*!
     *  Create a JSON document for storing the generator configurations.
     *
     *      @param [in] renodeData          Container for renode generator data.
     *      @param [in] selectedView        Name of the view to use in the generator.
     *      @param [in] saveToFileSetFlag   Flag for save to file set.
     *      @param [in] selectedFileSet     Name of the target file set.
     *      @param [in] folderPath          Path for the renode files.
     *      @param [in] writeCPU            Flag for writing a CPU file.
     *      @param [in] writeMemory         Flag for writing a memory file.
     *      @param [in] writePeripherals    Flag for writing a peripherals file.
     *
     *      @return The JSON configuration document.
     */
    QJsonDocument createJsonDocument(QVector<QSharedPointer<RenodeCPUDetailRoutes> > renodeData, QString const& selectedView,
        bool saveToFileSetFlag, QString const& selectedFileSet, QString const& folderPath, bool writeCPU, bool writeMemory, bool writePeripherals);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility provided by call runGenerator.
    IPluginUtility* utility_;
};

#endif // RENODEGENERATORPLUGIN_H
