//-----------------------------------------------------------------------------
// File: VHDLPackageImport.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Antti Sillanpää
// Date: 25.06.2015
//
// Description:
// Import plugin for VHDL package.
//-----------------------------------------------------------------------------

#ifndef VHDLPACKAGEIMPORT_H
#define VHDLPACKAGEIMPORT_H

#include "vhdlpackageimport_global.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportPlugin.h>
//#include <Plugins/PluginSystem/ImportPlugin/IncludeImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>

#include <QSharedPointer>
#include <QObject>

class Component;
class ModelParameter;
//-----------------------------------------------------------------------------
//! Import plugin for VHDL package.
//-----------------------------------------------------------------------------

class VHDLPACKAGEIMPORT_EXPORT VHDLPackageImport : public QObject, public ImportPlugin, public HighlightSource
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.vhdlpackageimportplugin" FILE "vhdlpackageimportplugin.json")

    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportPlugin)
   // Q_INTERFACES(IncludeImportPlugin)

public:

    //! The constructor.
    VHDLPackageImport();

    //! The destructor.
    ~VHDLPackageImport();

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

    //! \brief Returns the external program requirements of the plugin.
     virtual QList<ExternalProgramRequirement> getProgramRequirements();

    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import plugin supports.
     */
    virtual QStringList getSupportedFileTypes() const;

    /*!
     *  Gets any compatibility warnings for the plugin.
     *
     *      @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const;

    /*!
     *  Parses the given input and creates parameters from Verilog defines.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Sets the given highlighter to be used by the plugin.
     *
     *      @param [in] highlighter   The highlighter to use.
     */
    virtual void setHighlighter(Highlighter* highlighter);

private:

    // Disable copying.
    VHDLPackageImport(VHDLPackageImport const& rhs);
    VHDLPackageImport& operator=(VHDLPackageImport const& rhs);

    /*!
     *  Checks if the given input has a valid package declaration.
     *
     *      @param [in] input   The input to search for package declaration.
     *
     *      @return True, if a valid module was found, otherwise false.
     */
    bool hasPackageDeclaration(QString const& input);

    /*!
     *  Finds constant declarations from the inspected string with provided rule.
     *
     *      @param [in] declarationRule   The rule for finding a constant.
     *      @param [in] inspect           The input text containing the declarations.
     *
     *      @return   The resulting list of declarations.
     */
    QStringList findDeclarations(QRegularExpression const& declarationRule, QString const& inspect);

    /*!
     *  Highlights the given definition.
     *
     *      @param [in] definition   The defintion to highlight.
     */

    void highlightDefinition(QString const& definition);

    /*!
     *  Creates a parameter from the given definition and adds it to the component.
     *  Existing parameter with the same name will be updated and no new parameter is added.
     *
     *      @param [in] definition          The definition to create the parameter from.
     *      @param [in] targetComponent     The component to add the parameter to.
     */
    void createModelParameterFromDefinition(QString const& definition, QSharedPointer<Component> targetComponent);

    /*!
     *  Finds a parameter in the given component by the given name.
     *
     *      @param [in] targetComponent     The component the search in.
     *      @param [in] name                The name of the parameter to find.
     *
     *      @return The parameter with the given name or null if no parameter matches the name.
     */
    QSharedPointer<ModelParameter> findModelParameterByName(QSharedPointer<Component> targetComponent,
        QString const& name) const;

    //! The highlighter to use.
    Highlighter* highlighter_;
};



#endif // VHDLPACKAGEIMPORT_H
