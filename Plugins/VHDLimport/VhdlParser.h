//-----------------------------------------------------------------------------
// File: VhdlParser.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 10.06.2013
//
// Description:
// VhdlParser reads a given input, creates ports and generics accordingly
// and highlights the created ports and generics. 
//-----------------------------------------------------------------------------
#ifndef VhdlParser_H
#define VhdlParser_H

#include "vhdlimport_global.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/Importparser.h>
#include <Plugins/PluginSystem/HighlightSource.h>
#include <Plugins/PluginSystem/PortSource.h>
#include <Plugins/PluginSystem/ModelParameterSource.h>

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

//-----------------------------------------------------------------------------
//! Class VhdlParser.
//-----------------------------------------------------------------------------
class VHDLIMPORT_EXPORT VhdlParser : public QObject, public ImportParser, public HighlightSource, 
    public PortSource, public ModelParameterSource
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "kactus2.plugins.vhdlimportplugin" FILE "vhdlimportplugin.json")
    Q_INTERFACES(IPlugin)
    Q_INTERFACES(ImportParser)

public:

    /*!
     * The constructor.
     */
    VhdlParser();

    /*!
     *  Destructor.
     */
    ~VhdlParser();
    
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
     *   Reads a vhdl file and creates generics and ports.
     *
     *      @param [in] absolutePath The absolute path of the file.
	 *
	 *      @return False, if file could not be read, otherwise true.
     */
    virtual void runParser(QString const& input, QSharedPointer<Component> targetComponent);

    /*!
     *  Removes any previously parsed ports and model parameters.
     */
    void clear();

    virtual void setHighlighter(Highlighter* highlighter);
    
    virtual void setPortVisualizer(PortVisualizer* visualizer);

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
    VhdlParser(const VhdlParser&);

    //! No assignment.
    VhdlParser& operator=(const VhdlParser&);

	/*!
     *  Signals remove for all ports and empties lists to ports.
     */
    void removePreviousPorts();

	/*!
     *  Signals remove for all model parameters and empties lists to model parameters.
     */
    void removePreviousGenerics();

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

#endif // VhdlParser_H
