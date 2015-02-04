//-----------------------------------------------------------------------------
// File: ImportRunner.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Runs import plugins for a given file and component.
//-----------------------------------------------------------------------------

#ifndef IMPORTRUNNER_H
#define IMPORTRUNNER_H

#include <Plugins/PluginSystem/PluginManager.h>

#include <QObject>
#include <QSharedPointer>

class Component;
class ExpressionParser;
class Highlighter;
class ImportPlugin;

class ModelParameterVisualizer;

//-----------------------------------------------------------------------------
//! Runs import plugins for a given file and component.
//-----------------------------------------------------------------------------
class ImportRunner : public QObject
{
    Q_OBJECT
public:

	//! The constructor.
	ImportRunner(QObject* parent);

	//! The destructor.
	~ImportRunner();

    /*!
     *  Sets the given highlighter to be used by all highlight sources.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    void setHighlighter(Highlighter* highlighter);
  
    /*!
     *  Sets the given highlighter to be used by all highlight sources.
     *
     *      @param [in] highlighter   The highlighter to use.          
     */
    void setVerilogExpressionParser(QSharedPointer<ExpressionParser> parser);

    /*!
     *  Sets the given visualizer to be used by all model parameter sources.
     *
     *      @param [in] visualizer   The visualizer to use.          
     */
    void setModelParameterVisualizer(ModelParameterVisualizer* visualizer);

    /*!
     *  Loads all available import plugins.
     *
     *      @param [in] pluginManager   The plugin manager owning all the plugins.
     */
    void loadImportPlugins(PluginManager const &pluginManager);

    /*!
     *  Runs all import plugins with matching file types for a given file and component.
     *
     *      @param [in] importFile          The relative path to the input file.
     *      @param [in] componentXmlPath    The path to the target component XML file.
     *      @param [in] targetComponent     The target component to which import all parsed elements.     
     *
     *      @return The component with all the imported elements.     
     */
    QSharedPointer<Component> parse(QString const& importFile, QString const& componentXmlPath, 
        QSharedPointer<const Component> targetComponent);

    /*!
     *  Gets all file types for which a import parser(s) can be run.
     *
     *      @return List of possible file types.
     */
    QStringList importFileTypes() const;

signals:
    
    //! Emitted to notice user.
    void noticeMessage(QString const& message);

private:

	// Disable copying.
	ImportRunner(ImportRunner const& rhs);
    ImportRunner& operator=(ImportRunner const& rhs);

    /*!
     *  Finds possible import plugins for given file types.
     *
     *      @param [in] filetypes   The file types to search for.
     *
     *      @return Import plugins capable of reading the given file types.
     */
    QList<ImportPlugin*> parsersForFileTypes(QStringList const& filetypes) const;

    /*!
     *  Finds the file types of a given file.
     *
     *      @param [in] importFile          The relative path to the file.
     *      @param [in] targetComponent     The component containing the file set containing the given file.
     *
     *      @return File types of the given file.
     */
    QStringList filetypesOfImportFile(QString const& importFile, QSharedPointer<Component> targetComponent) const;

    /*!
     *  Reads the content of a given file.
     *
     *      @param [in] relativePath    Relative path to the file from basePath.
     *      @param [in] basePath        The from which to begin the relative path.
     *
     *      @return The file content.
     */
    QString readInputFile(QString const& relativePath, QString const& basePath) const;
   
    /*!
     *  Adds model parameter visualization to a given import plugin if possible.
     *
     *      @param [in] parser   The plugin to add visualization to.     
     */
    void addModelParameterVisualizationIfPossible(ImportPlugin* parser);
    
    /*!
     *  Adds highlighting to a given import plugin if possible.
     *
     *      @param [in] parser   The plugin to add highlighting to.     
     */
    void addHighlightIfPossible(ImportPlugin* parser);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The highlighter used by all import plugins.
    Highlighter* highlighter_;

    //! The model parameter visualizer used by all import plugins.
    ModelParameterVisualizer* modelParameterVisualizer_;

    //! The expression parser to use for verilog expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! All loaded import plugins.
    QList<ImportPlugin*> ImportPlugins_;
};

#endif // IMPORTRUNNER_H
