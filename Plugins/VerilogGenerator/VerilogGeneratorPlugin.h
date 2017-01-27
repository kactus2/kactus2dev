//-----------------------------------------------------------------------------
// File: VerilogGeneratorPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 27.08.2014
//
// Description:
// Plugin for structural verilog generation.
//-----------------------------------------------------------------------------

#ifndef VERILOGGENERATORPLUGIN_H
#define VERILOGGENERATORPLUGIN_H

#include "veriloggeneratorplugin_global.h"
#include "VerilogWriterFactory/VerilogWriterFactory.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/GeneratorPlugin/IGeneratorPlugin.h>
#include <Plugins/PluginSystem/GeneratorPlugin/GeneratorConfiguration.h>

#include <IPXACTmodels/common/VLNV.h>

#include <QObject>
#include <QSharedPointer>
#include <QtPlugin>

class Component;
class ViewSelection;
class Document;
class View;
class ComponentInstantiation;
class FileSet;
class Design;
class DesignConfiguration;

//-----------------------------------------------------------------------------
//! Plugin for structural verilog generation.
//-----------------------------------------------------------------------------
class VERILOGGENERATORPLUGIN_EXPORT VerilogGeneratorPlugin : public QObject, public IGeneratorPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.veriloggeneratorplugin" FILE "veriloggeneratorplugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(IGeneratorPlugin)

public:

	//! The constructor.
	VerilogGeneratorPlugin();

	//! The destructor.
	virtual ~VerilogGeneratorPlugin();

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

protected:

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in,out]	    targetComponent	The component for which the generator is run.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(QSharedPointer<const Component> targetComponent) const;

    /*!
     *  Finds the possible views for generation.
     *
     *      @param [in] input			        The relevant IP-XACT documents.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findPossibleViews(GenerationTuple input) const;

    /*!
     *  Checks if the generator could be configured.
     *
	 *      @param [in] input			        The relevant IP-XACT documents.
     *
     *      @return True, if was successful, otherwise false.
     */
    virtual bool couldConfigure(GenerationTuple input);

    /*!
     *  Gets the configuration for the generation.
     *
     *      @return The configuration to use in generation.
     */
    virtual QSharedPointer<GeneratorConfiguration> getConfiguration();

public slots:

    /*!
     *  Called when an error is reported to us.
     *
	 *      @param [in] report			The error message.
	 */
	void onErrorReport(const QString& report);

private:
	// Disable copying.
	VerilogGeneratorPlugin(VerilogGeneratorPlugin const& rhs);
	VerilogGeneratorPlugin& operator=(VerilogGeneratorPlugin const& rhs);
    
    /*!
     *  Gets the default output path.     
     *
     *      @return The default output path.
     */
    QString defaultOutputPath() const;
    
    /*!
     *  Gets the relative path from the top component xml file to the given absolute path.
     *
     *      @param [in] filePath   The absolute path to the target file.
     *
     *      @return Relative path from the top component xml file to the target file.
     */
    QString relativePathFromXmlToFile(QString const& filePath) const;

    /*!
     *  Adds the generated file to a file set in the top component.
	 *  Will do nothing, if the fileSetName is empty.
     *
	 *      @param [in] activeView		The top component active view, which will refer to the instantiation.
	 *      @param [in] instantiation   The instantiation, which will have the fileSetName as reference.
	 *      @param [in] fileSetName		The name of the file set, where the file will be appended to.
     */
    void addGeneratedFileToFileSet(QSharedPointer<ViewSelection> configuration,
        QSharedPointer<QList<QSharedPointer<VerilogDocument> > > documents);

    /*!
     *  Inserts description to a generated file.
     *
	 *      @param [in] file		The file which needs to be described.
     */
    void insertFileDescription(QSharedPointer<File> file);

    //! Saves the changes made to the top component.
    void saveChanges() const;

    /*!
     *  Finds all the views in containing component referencing the given design or design configuration VLNV.
     *
     *      @param [in] containingComponent     The component whose views to search through.
     *      @param [in] targetReference         The reference to find in views.
     *
     *      @return The the views referencing the given VLNV.
     */
    QSharedPointer<QList<QSharedPointer<View> > > findReferencingViews(QSharedPointer<Component> containingComponent,
		VLNV targetReference) const;

     //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility to use while running generation.
    IPluginUtility* utility_;

    //! The top component for which to run the generation.
    QSharedPointer<Component> topComponent_;

    //! The configuration for the generation.
    QSharedPointer<GeneratorConfiguration> configuration_;

    //! The last values used by the generation.
    bool generateInterface_;
    bool generateMemory_;
    QString lastViewName_;
    QString lastFileSetName_;
};

#endif // VERILOGGENERATORPLUGIN_H