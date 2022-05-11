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
#include "Plugins/VerilogImport/VerilogInstanceParser.h"

#include <KactusAPI/include/ExpressionSupport.h>
#include <KactusAPI/include/ImportPlugin.h>
#include <KactusAPI/include/HighlightSource.h>

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
	virtual ~VerilogImporter() = default;
    
    /*!
     *  Returns the name of the plugin.
     */
    virtual QString getName() const override final;

    /*!
     *  Returns the version of the plugin.
     */
    virtual QString getVersion() const override final;
    
    /*!
     *  Returns the description of the plugin.
     */
    virtual QString getDescription() const override final;
  
    /*!
     *  Returns the vendor of the plugin.
     */
    virtual QString getVendor() const override final;
    
    /*!
     *  Returns the licence of the plugin.
     */
    virtual QString getLicence() const override final;
    
    /*!
     *  Returns the licence holder of the plugin.
     */
    virtual QString getLicenceHolder() const override final;
    
    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget() override final;

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel() override final {return nullptr;}

    /*! 
     *   Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements() override final;
    
    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import parser supports.
     */
    virtual QStringList getSupportedFileTypes() const override final;
    
    /*!
     *  Gets any compatibility warnings for the plugin.
     *
     *      @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const override final;

    /*!
     *  Get component declarations from the selected input file.
     *
     *      @param [in] input   The selected input file.
     *
     *      @return List of component declarations found in the selected input.
     */
    virtual QStringList getFileComponents(QString const& input) const override final;

    /*!
     *  Get the name of the selected component declaration.
     *
     *      @param [in] componentDeclaration    The selected component declaration.
     *
     *      @return Name of the selected component declaration.
     */
    virtual QString getComponentName(QString const& componentDeclaration) const override final;

    /*!
     *   Parses a verilog input, sets up an rtl view and creates model parameters and ports.
     *
     *      @param [in] input                   The input text to parse.
     *      @param [in] componentDeclaration    Declaration of the selected component.
     *      @param [in] targetComponent         The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QString const& componentDeclaration,
        QSharedPointer<Component> targetComponent) override final;

    /*!
     *  Sets the given visualizer to be used by the import.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter) override final;

    /*!
     *  Sets the parser to use for expressions.
     *
     *      @param [in] parser   The parser to use.
     */
    virtual void setExpressionParser(QSharedPointer<ExpressionParser> parser) override final;

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
     *      @param [in] input               The input file content to highlight entity in.
     *      @param [in] moduleDeclaration   Selected module declaration from the file.
     */
    void highlightModule(QString const& input, QString const& moduleDeclaration);

    /*!
     *  Parses the model name from the input and sets it in the rtl view.
     *
     *      @param [in] moduleDeclaration                   The selected module declaration.
     *      @param [in/out] targetComponentInstantiation    The component instance to set the name in.
     */
    void importModelName(QString const& moduleDeclaration,
        QSharedPointer<ComponentInstantiation> targetComponentInstantiation);

    /*!
     *  Finds a flat (rtl) view from the target component or creates one, if none are found.
     *
     *      @return Flat view to set up with model name and environmental identifiers.
     */
    QSharedPointer<View> findOrCreateFlatView(QSharedPointer<Component> targetComponent) const;
    
    /*!
     *  Sets the language and environmental identifiers in the rtl view.
     */
    void setLanguageAndEnvironmentalIdentifiers(QSharedPointer<Component> targetComponent,
		QSharedPointer<ComponentInstantiation>& targetComponentInstantiation) const;    

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The port parser to use for importing ports.
    VerilogPortParser portParser_;

    //! The parameter parser to use for importing model parameters.
    VerilogParameterParser parameterParser_;

    //! Instance parser used for importing contained component instances.
    VerilogInstanceParser instanceParser_;

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // VERILOGIMPORTER_H
