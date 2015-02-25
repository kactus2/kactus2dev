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

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/IGeneratorPlugin.h>

#include <IPXACTmodels/vlnv.h>

#include <QObject>
#include <QSharedPointer>
#include <QtPlugin>

class Component;
class GeneratorConfiguration;
class LibraryComponent;

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
    virtual QString const& getName() const;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString const& getVersion() const;

    /*!
     *  Returns the description of the plugin.
     */
    virtual QString const& getDescription() const;

    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString const& getVendor() const;

    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString const& getLicence() const;

    /*!
     *  Returns the licence holder of the plugin.
     */
    virtual QString const& getLicenceHolder() const;
    
    /*!
     *  Returns the settings widget.
     */
    virtual PluginSettingsWidget* getSettingsWidget();

    /*!
     *  Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();

    /*!
     *  Returns the icon for the generator.
     */
    virtual QIcon getIcon() const;

    /*!
     *  Checks whether the generator supports generation for the given library component.
     *
     *      @param [in] libComp		The library component for which to check support.
     *      @param [in] libDesConf	The optional design configuration object.
     *      @param [in] libDes		The optional design object.
     *
     *      @return True, if the generator supports the given component, otherwise false.
     */
    virtual bool checkGeneratorSupport(QSharedPointer<LibraryComponent const> libComp, 
        QSharedPointer<LibraryComponent const> libDesConf = QSharedPointer<LibraryComponent const>() , 
        QSharedPointer<LibraryComponent const> libDes = QSharedPointer<LibraryComponent const>()) const;

    /*!
     *  Runs the generator.
     *
     *      @param [in]			utility			The plugin utility to use.
     *      @param [in,out]	    libComp			The library component for which the generator is run.
     *      @param [in]	        libDesConf		The optional design configuration object for the generator.
     *      @param [in]	        libDes			The optional design object.
     */
    virtual void runGenerator(IPluginUtility* utility, 
        QSharedPointer<LibraryComponent> libComp, 
        QSharedPointer<LibraryComponent> libDesConf = QSharedPointer<LibraryComponent>(), 
        QSharedPointer<LibraryComponent> libDes = QSharedPointer<LibraryComponent>());

protected:

    /*!
     *  Finds the possible view names for generation.
     *
     *      @param [in,out]	    libComp			The library component for which the generator is run.
     *      @param [in]	        libDes   		The design which the generator is run.
     *      @param [in]	        libDesConf      The design configuration object for which the generator is run.
     *
     *      @return List of possible view names for which to run the generation.
     */
    QStringList findPossibleViewNames(QSharedPointer<LibraryComponent> libComp, 
        QSharedPointer<LibraryComponent> libDes, 
        QSharedPointer<LibraryComponent> libDesConf) const;

    /*!
     *  Checks if the generator could be configured.
     *
     *      @param [in] possibleViewNames   The list of possible view names for generation.
     *
     *      @return True, if configuration was successful, otherwise false.
     */
    virtual bool couldConfigure(QStringList const& possibleViewNames) const;

    /*!
     *  Gets the configuration for the generation.
     *
     *      @return The configuration to use in generation.
     */
    virtual QSharedPointer<GeneratorConfiguration> getConfiguration();

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
     *  Checks if the generated file should be added to a file set in the top component.
     *
     *      @return True, if the file should be added to file set, otherwise false.
     */
    virtual bool outputFileAndViewShouldBeAddedToTopComponent() const;
    
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
     *
     *      @param [in] activeViewName   The name of the top component active view.
     *
     */
    void addGeneratedFileToFileSet(QString const& activeViewName) const;    

    /*!
     *  Creates a name for the file set containing the generated file.
     *
     *      @param [in] activeViewName   The name of the top component active view.
     *
     *      @return Name for the generated file set.
     */
    QString fileSetNameForActiveView(QString const& activeViewName) const;

    /*!
     *  Adds an RTL view to the top component for the generated model.
     *
     *      @param [in] activeViewName   The name of the top component active view.
     */
    void addRTLViewToTopComponent(QString const& activeViewName) const;

    //! Saves the changes made to the top component.
    void saveChanges() const;

    /*!
     *  Finds all the views in containing component referencing the given design or design configuration VLNV.
     *
     *      @param [in] containingComponent     The component whose views to search through.
     *      @param [in] targetReference         The reference to find in views.
     *
     *      @return The names of the views referencing the given VLNV.
     */
    QStringList findReferencingViews(QSharedPointer<Component> containingComponent, VLNV targetReference) const;

     //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The plugin utility to use while running generation.
    IPluginUtility* utility_;

    //! The top component for which to run the generation.
    QSharedPointer<Component> topComponent_;

    //! The path to selected output file.
    QString outputFile_;

    //! The configuration for file generation.
    QSharedPointer<GeneratorConfiguration> configuration_;
};

#endif // VERILOGGENERATORPLUGIN_H