//-----------------------------------------------------------------------------
// File: VerilogImporter.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 23.09.2014
//
// Description:
// Import plugin for verilog source files.
//-----------------------------------------------------------------------------

#ifndef VERILOGIMPORTER_H
#define VERILOGIMPORTER_H

#include "verilogimport_global.h"

#include "VerilogPortParser.h"
#include "VerilogParameterParser.h"

#include <Plugins/PluginSystem/ImportPlugin/ExpressionSupport.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <QSharedPointer>
#include <QString>

class Component;
class View;

//-----------------------------------------------------------------------------
//! Import plugin for verilog source files.
//-----------------------------------------------------------------------------
class VERILOGIMPORT_EXPORT VerilogImporter: public QObject, public ImportPlugin, public HighlightSource, 
    public ExpressionSupport
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.verilogimportplugin" FILE "verilogimportplugin.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportPlugin)

public:

	//! The constructor.
	VerilogImporter();

	//! The destructor.
	~VerilogImporter();
    
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
    virtual PluginSettingsWidget* getSettingsWidget();

    /*! 
     *   Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();
    
    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import parser supports.
     */
    virtual QStringList getSupportedFileTypes() const;
    
    /*!
     *  Gets any compatibility warnings for the plugin.
     *
     *      @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const;

    /*!
     *   Parses a verilog input, sets up an rtl view and creates model parameters and ports.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Sets the given visualizer to be used by the import.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

    /*!
     *  Sets the parser to use for expressions.
     *
     *      @param [in] parser   The parser to use.
     */
    virtual void setExpressionParser(QSharedPointer<ExpressionParser> parser);

private:

	// Disable copying.
	VerilogImporter(VerilogImporter const& rhs);
	VerilogImporter& operator=(VerilogImporter const& rhs);
    
    
    /*!
     *  Checks if the given input has a valid module declaration.
     *
     *      @param [in] input   The input to search for module declaration.
     *
     *      @return True, if a valid module was found, otherwise false.
     */
    bool hasModuleDeclaration(QString const& input);

    /*!
     *  Highlights the module section in the input.
     *
     *      @param [in] fileContent   The input file content to highlight entity in.
     */
    void highlightModule(QString const& input);

    /*!
     *  Parses the model name from the input and sets it in the rtl view.
     *
     *      @param [in] input   The input text to parse the model name from.
     */
    void importModelName(QString const& input, QSharedPointer<Component> targetComponent);
   
    /*!
     *  Finds a flat (rtl) view from the target component or creates one, if none are found.
     *
     *      @return Flat view to set up with model name and environmental identifiers.
     */
    View* findOrCreateFlatView(QSharedPointer<Component> targetComponent) const;
    
    /*!
     *  Sets the language and environmental identifiers in the rtl view.
     */
    void setLanguageAndEnvironmentalIdentifiers(QSharedPointer<Component> targetComponent) const;    

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port parser to use for importing ports.
    VerilogPortParser portParser_;

    //! The parameter parser to use for importing model parameters.
    VerilogParameterParser parameterParser_;

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // VERILOGIMPORTER_H
