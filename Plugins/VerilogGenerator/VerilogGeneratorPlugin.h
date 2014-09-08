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

#include <QObject>
#include <QSharedPointer>
#include <QtPlugin>

class LibraryComponent;
class Component;

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

private:
	// Disable copying.
	VerilogGeneratorPlugin(VerilogGeneratorPlugin const& rhs);
	VerilogGeneratorPlugin& operator=(VerilogGeneratorPlugin const& rhs);
  
    /*!
     *  Asks the user to select the output for the generation.     
     *
     *      @return The absolute path to the output file.
     */
    void selectOutputFile();

    /*!
     *  Checks if the output file for generation has been selected.     
     *
     *      @return True, if an output file has been selected, otherwise false.
     */
    bool outputFileSelected() const;

    /*!
     *  Checks if the generated file should be added to a file set in the top component.
     *
     *      @return True, if the file should be added to file set, otherwise false.
     */
    bool fileShouldBeAddedToFileset() const;
    
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
     */
    void addGeneratedFileToFileSet() const;
    
    //! The plugin utility to use while running generation.
    IPluginUtility* utility_;

    //! The top component for which to run the generation.
    QSharedPointer<Component> topComponent_;

    //! The path to selected output file.
    QString outputFile_;
};

#endif // VERILOGGENERATORPLUGIN_H