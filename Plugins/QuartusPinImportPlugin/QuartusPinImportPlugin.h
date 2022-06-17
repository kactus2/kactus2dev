//-----------------------------------------------------------------------------
// File: QuartusPinImportPlugin.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 19.11.2013
//
// Description:
// Kactus2 plugin for Quartus II pin import.
//-----------------------------------------------------------------------------

#ifndef QUARTUSPINIMPORTPLUGIN_H
#define QUARTUSPINIMPORTPLUGIN_H

#include "QuartusPinImportPlugin_global.h"

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/ImportPlugin.h>
#include <KactusAPI/include/HighlightSource.h>

#include <IPXACTmodels/common/DirectionTypes.h>

class Component;
class Highlighter;
//-----------------------------------------------------------------------------
// Plugin for importing component ports from Quartus II pin file.
//-----------------------------------------------------------------------------
class QUARTUSPINIMPORTPLUGIN_EXPORT QuartusPinImportPlugin : public QObject, 
    public ImportPlugin, public HighlightSource
{

    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.quartuspinimportplugin" FILE "quartuspinimportplugin.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportPlugin)

public:
    //! The constructor.
    QuartusPinImportPlugin();

    //! The destructor.
    ~QuartusPinImportPlugin();

    //! Returns the name of the plugin.
    virtual QString getName() const;

	//! Returns the version of the plugin.
    virtual QString getVersion() const;

	//! Returns the description of the plugin.
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
     *  Returns the holder of the license of the plugin.
     */
    virtual QString getLicenseHolder() const;

	//! Returns the icon for the generator.
    /*!
     *  Returns the settings widget.
     */
    virtual QWidget* getSettingsWidget();

    /*!
     *  This is used to access the settings modified by function getSettingsWidget().
     */
    virtual PluginSettingsModel* getSettingsModel(){return NULL;}

    //! Returns the external program requirements of the plugin.
    virtual QList<IPlugin::ExternalProgramRequirement> getProgramRequirements();
        
    /*!
     *  Sets the given highlighter to be used by the plugin.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);

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
     *  Runs the import by parsing the given input and applying the parsed elements to the given component.
     *
     *      @param [in] input                   The input text to parse.
     *      @param [in] componentDeclaration    Declaration of the selected component.
     *      @param [in] targetComponent         The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QString const& componentDeclaration,
        QSharedPointer<Component> targetComponent);

private:

    //! No copying
    QuartusPinImportPlugin(const QuartusPinImportPlugin& other);

    //! No assignment
    QuartusPinImportPlugin& operator=(const QuartusPinImportPlugin& other);

    /*!
     *  Creates a port in the component from the given line in the pin file.
     *
     *      @param [in] line                The line to parse as port.
     *      @param [in] targetComponent     The component to add the port into.
     */
    void createPort(QString const& line, QSharedPointer<Component> targetComponent);

    /*!
     *  Parses pin direction to IP-XACT port direction.
     *
     *      @param [in] direction   The direction to parse.
     *
     *      @return The port direction.
     */
    DirectionTypes::Direction parseDirection(QString const& direction);

    //! The highlighter to use.
    Highlighter* highlighter_;
};

#endif // QUARTUSPINIMPORTPLUGIN_H
