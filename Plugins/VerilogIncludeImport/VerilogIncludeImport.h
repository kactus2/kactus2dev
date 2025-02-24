//-----------------------------------------------------------------------------
// File: VerilogIncludeImport.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 30.03.2015
//
// Description:
// Import plugin for Verilog definitions.
//-----------------------------------------------------------------------------

#ifndef VERILOGINCLUDEIMPORT_H
#define VERILOGINCLUDEIMPORT_H

#include "verilogincludeimport_global.h"

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/ImportPlugin.h>
#include <KactusAPI/include/IncludeImportPlugin.h>
#include <KactusAPI/include/HighlightSource.h>

#include <Plugins/VerilogImport/VerilogParameterParser.h>

#include <QSharedPointer>
#include <QObject>

class Component;
class Parameter;
//-----------------------------------------------------------------------------
//! Import plugin for Verilog definitions.
//-----------------------------------------------------------------------------
class VERILOGINCLUDEIMPORT_EXPORT VerilogIncludeImport : public QObject, public IncludeImportPlugin, public HighlightSource
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.verilogincludeimportplugin" FILE "verilogincludeimportplugin.json")
   
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportPlugin)
    Q_INTERFACES(IncludeImportPlugin)

public:

	//! The constructor.
	VerilogIncludeImport();

	//! The destructor.
	~VerilogIncludeImport();

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
     *  Returns the license of the plugin.
     */
    virtual QString getLicense() const;

    /*!
     *  Returns the license holder of the plugin.
     */
    virtual QString getLicenseHolder() const;

    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

	//! \brief Returns the external program requirements of the plugin.
	 virtual QList<ExternalProgramRequirement> getProgramRequirements();

    /*!
     *  Returns the supported import file types.
     *
     *    @return The file types the import plugin supports.
     */
    virtual QStringList getSupportedFileTypes() const;

    /*!
     *  Gets any compatibility warnings for the plugin.
     *
     *    @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const;

    /*!
     *  Get component declarations from the selected input file.
     *
     *    @param [in] input   The selected input file.
     *
     *    @return List of component declarations found in the selected input.
     */
    virtual QStringList getFileComponents(QString const& input) const override final;

    /*!
     *  Get the name of the selected component declaration.
     *
     *    @param [in] componentDeclaration    The selected component declaration.
     *
     *    @return Name of the selected component declaration.
     */
    virtual QString getComponentName(QString const& componentDeclaration) const override final;

    /*!
     *  Parses the given input and creates parameters from Verilog defines.
     *
     *    @param [in] input                   The input text to parse.
     *    @param [in] componentDeclaration    Declaration of the selected component.
     *    @param [in] targetComponent         The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QString const& componentDeclaration,
        QSharedPointer<Component> targetComponent) override final;

    /*!
     *  Sets the given highlighter to be used by the plugin.
     *
     *    @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

private:

	// Disable copying.
	VerilogIncludeImport(VerilogIncludeImport const& rhs);
	VerilogIncludeImport& operator=(VerilogIncludeImport const& rhs);
        
    /*!
     *  Highlights the given definition.
     *
     *    @param [in] definition   The definition to highlight.
     */
    void highlightDefinition(QString const& definition);

    /*!
     *  Creates a parameter from the given definition and adds it to the component.
     *  Existing parameter with the same name will be updated and no new parameter is added.
     *
     *    @param [in] definition          The definition to create the parameter from.
     *    @param [in] targetComponent     The component to add the parameter to.
     */
    void createParameterFromDefinition(QString const& definition, QSharedPointer<Component> targetComponent);

    /*!
     *  Finds a parameter in the given component by the given name.
     *
     *    @param [in] targetComponent     The component the search in.
     *    @param [in] name                The name of the model parameter to find.
     *
     *    @return The parameter with the given name or null if no parameter matches the name.
     */
    QSharedPointer<Parameter> findParameterByName(QSharedPointer<Component> targetComponent, 
        QString const& name) const;

    /*!
     *  Parse the included verilog parameters.
     *
     *    @param [in] input               The input text to parse.
     *    @param [in] targetComponent     The component to add the parameter to.
     */
    void parseParameters(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Get the areas from the input text that do not contain verilog modules.
     *
     *    @param [in] input   The selected input.
     *
     *    @return List of non module text areas.
     */
    QStringList getNonModuleAreas(QString const& input) const;

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parser for verilog parameters.
    VerilogParameterParser parameterParser_;

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // VERILOGINCLUDEIMPORT_H
