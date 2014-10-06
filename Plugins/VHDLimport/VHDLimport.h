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

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterSource.h>

#include <QList>
#include <QMap>
#include <QtPlugin>

class Highlighter;
class ModelParameter;
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
     *   Returns the external program requirements of the plugin.
     */
    virtual QList<ExternalProgramRequirement> getProgramRequirements();
    
    /*!
     *  Returns the supported import file types.
     *
     *      @return The file types the import parser supports.
     */
    virtual QStringList acceptedFileTypes() const;

    /*!
     *   Parses a vhdl input, sets up an rtl view and creates generics and ports.
     *
     *      @param [in] input               The input text to parse.
     *      @param [in] targetComponent     The component to apply all imported changes to.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Removes any previously parsed ports and model parameters.
     */
    void clear();

    /*!
     *  Sets the given highlighter to be used by the import.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    virtual void setHighlighter(Highlighter* highlighter);
        
    /*!
     *  Sets the given visualizer to be used by the import.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    virtual void setModelParameterVisualizer(ModelParameterVisualizer* visualizer);

public slots:

    //! Called when a model parameter has changed.
    virtual void onModelParameterChanged(QSharedPointer<ModelParameter> changedParameter) const;

    //! Called when a port has been parsed.
    void onPortParsed(QSharedPointer<Port> parsedPort, QString const& declaration);   

    //! Called when a text should be highlighted.
    void highlight(QString const& text, QColor const& highlightColor) const;

private:

    //! No copying.
    VHDLimport(const VHDLimport&);

    //! No assignment.
    VHDLimport& operator=(const VHDLimport&);

	/*!
     *  Signals remove for all ports and empties lists to ports.
     */
    void removePreviousPortDeclarations();

	/*!
     *  Signals remove for all model parameters and empties lists to model parameters.
     */
    void removePreviouslyDependentGenerics();

    /*!
     *   Checks if given vhdl file has a valid entity declaration.
     *
     *      @param [in] fileString The vhdl file as string.
	 *
	 *      @return True if the file has a valid entity, otherwise false.
     */
    bool hasValidEntity(QString const& fileContent) const;

    /*!
     *  Grays the whole input.
     *
     *      @param [in] fileContent   The input file content to gray out.
     */
    void grayOutFileContent(QString const& fileContent) const;

    /*!
     *  Highlights the entity section in the input.
     *
     *      @param [in] fileContent   The input file content to highlight entity in.
     */
    void highlightEntity(QString const& fileContent) const;
    
    /*!
     *  Parses the model name from the input and sets it in the rtl view.
     *
     *      @param [in] input   The input text to parse the model name from.
     */
    void parseModelName(QString const& input) const;

    /*!
     *  Checks if the given input contains architecture definition for entity.
     *
     *      @param [in] architectureExp     Regular expression for capturing architecture.
     *      @param [in] input               The input text to parse the architecture from.
     *
     *      @return True, if an architecture is defined, otherwise false.
     */
    bool hasArchitecture(QRegExp const& architectureExp, QString const& input) const;

    /*!
     *  Creates model name from architecture definition.
     *
     *      @param [in] architectureExp   Regular expression for capturing architecture.
     *
     *      @return Model name based on the architecture.
     */
    QString createModelNameFromArchitecture(QRegExp const& architectureExp) const;

    /*!
     *  Highlights the architecture from which the model name was created.
     *
     *      @param [in] architectureExp   Regular expression for capturing architecture.
     */
    void highlightArchitecture(QRegExp const& architectureExp) const;
    
    /*!
     *  Checks if the given input contains configuration definition for entity.
     *
     *      @param [in] configurationExp    Regular expression for capturing configuration.
     *      @param [in] input               The input text to parse the configuration from.
     *
     *      @return True, if an configuration is defined, otherwise false.
     */
    bool hasConfiguration(QRegExp const& configurationExp, QString const& input) const;
    
    /*!
     *  Creates model name from configuration definition.
     *
     *      @param [in] configurationExp   Regular expression for capturing configuration.
     *
     *      @return Model name based on the configuration.
     */
    QString createModelNameFromConfiguration(QRegExp const& configurationExp) const;

    /*!
     *  Highlights the configuration from which the model name was created.
     *
     *      @param [in] configurationExp   Regular expression for capturing configuration.
     */
    void highlightConfiguration(QRegExp const& configurationExp) const;

    /*!
     *  Sets the language and environmental identifiers in the rtl view.
     */
    void setLanguageAndEnvironmentalIdentifiers() const;

    /*!
     *  Finds a flat (rtl) view from the target component or creates one, if none are found.
     *
     *      @return Flat view to set up with model name and environmental identifiers.
     */
    View* findOrCreateFlatView() const;

    /*!
     *  Adds a port dependency to a model parameter.
     *
     *      @param [in] modelParameter   The model parameter depended on.
     *      @param [in] parsedPort       The port depending on the model parameter.
     */
    void addDependencyOfGenericToPort(QSharedPointer<ModelParameter> modelParameter, 
        QSharedPointer<Port> parsedPort);
    
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
    QMap< QSharedPointer<ModelParameter>, QList< QSharedPointer<Port> > > dependedGenerics_;

    //! All parsed ports and their declarations in the source file.
    QMap<QSharedPointer<Port>, QString> parsedPortDeclarations_;
};

#endif // VHDLIMPORT_H
