//-----------------------------------------------------------------------------
// File: VHDLimport.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VHDLimport reads a given input, creates ports and generics accordingly
// and highlights the created ports and generics. 
//-----------------------------------------------------------------------------
#ifndef VHDLIMPORT_H
#define VHDLIMPORT_H

#include "vhdlimport_global.h"

#include <KactusAPI/include/IPlugin.h>
#include <KactusAPI/include/ImportPlugin.h>
#include <KactusAPI/include/HighlightSource.h>
#include <KactusAPI/include/ModelParameterSource.h>

#include "IPXACTmodels/Component/ComponentInstantiation.h"

#include <QList>
#include <QMap>
#include <QtPlugin>
#include <QRegularExpressionMatch>

class Highlighter;
class ModuleParameter;
class ModelParameterVisualizer;
class Port;
class PortVisualizer;
class VHDLPortParser;
class VHDLGenericParser;
class View;

//-----------------------------------------------------------------------------
//! Class VHDLimport.
//-----------------------------------------------------------------------------
class VHDLIMPORT_EXPORT VHDLimport : public QObject, public ImportPlugin, public HighlightSource, 
    public ModelParameterSource
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.vhdlimportplugin" FILE "vhdlimportplugin.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportPlugin)

public:

    /*!
     * The constructor.
     */
    VHDLimport();

    /*!
     *  Destructor.
     */
    ~VHDLimport();
    
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

    /*! 
     *   Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();
    
    /*!
     *  Returns the supported import file types.
     *
     *    @return The file types the import parser supports.
     */
    virtual QStringList getSupportedFileTypes() const;

    /*!
     *  Gets any compatibility warnings for the plugin.
     *
     *    @return The warning text.
     */
    virtual QString getCompatibilityWarnings() const;

    /*!
     *   Parses a vhdl input, sets up an rtl view and creates generics and ports.
     *
     *    @param [in] input                   The input text to parse.
     *    @param [in] componentDeclaration    Declaration of the selected entity.
     *    @param [in] targetComponent         The component to apply all imported changes to.
     */
    virtual void import(QString const& input, QString const& componentDeclaration,
        QSharedPointer<Component> targetComponent) override final;

    /*!
     *  Sets the given highlighter to be used by the import.
     *
     *    @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);
        
    /*!
     *  Sets the given visualizer to be used by the import.
     *
     *    @param [in] visualizer   The visualizer to use.          
     */
    virtual void setModelParameterVisualizer(ModelParameterVisualizer* visualizer);

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

private:

    //! No copying.
    VHDLimport(const VHDLimport&);

    //! No assignment.
    VHDLimport& operator=(const VHDLimport&);


    /*!
     *   Checks if given vhdl file has a valid entity declaration.
     *
     *    @param [in] fileString The vhdl file as string.
	 *
	 *    @return True if the file has a valid entity, otherwise false.
     */
    bool hasValidEntity(QString const& fileContent) const;

    /*!
     *  Highlights the entity section in the input.
     *
     *    @param [in] fileContent   The input file content to highlight entity in.
     */
    void highlightEntity(QString const& fileContent) const;
    
    /*!
     *  Parses the model name and architecture name from the input and sets it in the rtl view.
     *
     *    @param [in] input                       The input text to parse the model name from.
     *    @param [in/out] targetInstantiation     The instantiation to set the names in.
     */
    void parseModelNameAndArchitecture(QString const& input, QString const& componentDeclaration,
        QSharedPointer<ComponentInstantiation> targetInstantiation) const;

    /*!
     *  Highlights the architecture from which the model name was created.
     *
     *    @param [in] architectureExp   Regular expression for capturing architecture.
     */
    void highlightArchitecture(QRegularExpressionMatch const& architectureExp) const;

    /*!
     *  Highlights the configuration from which the model name was created.
     *
     *    @param [in] configurationExp   Regular expression for capturing configuration.
     */
    void highlightConfiguration(QRegularExpressionMatch const& configurationExp) const;

    /*!
     *  Setup the component instantiation and sets the language and environmental identifiers in the rtl view.
     */
    QSharedPointer<ComponentInstantiation> setupComponentInstantiation() const;

    /*!
     *  Finds a flat (rtl) view from the target component or creates one, if none are found.
     *
     *    @return Flat view to set up with model name and environmental identifiers.
     */
    QSharedPointer<View> findOrCreateFlatView() const;



    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Parser for ports.
    VHDLPortParser* portParser_;

    //! Parser for generics.
    VHDLGenericParser* genericParser_;

    //! The component to which add all parsed elements.
    QSharedPointer<Component> targetComponent_;

    //! The highlighter to use.
    Highlighter* highlighter_;

    //! Maps a generic to ports depending on it.
    QMap< QSharedPointer<ModuleParameter>, QList< QSharedPointer<Port> > > dependedGenerics_;

    //! All parsed ports and their declarations in the source file.
    QMap<QSharedPointer<Port>, QString> parsedPortDeclarations_;
};

#endif // VHDLIMPORT_H
