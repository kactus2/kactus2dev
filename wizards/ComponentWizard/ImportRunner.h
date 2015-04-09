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

#include <editors/ComponentEditor/common/ComponentParameterFinder.h>

#include <QObject>
#include <QSharedPointer>
#include <QTabWidget>
#include <QPlainTextEdit>

class Component;
class ExpressionParser;
class FileSet;
class Highlighter;
class ImportPlugin;
class ISourceAnalyzerPlugin;
class ModelParameterVisualizer;

//-----------------------------------------------------------------------------
//! Runs import plugins for a given file and component.
//-----------------------------------------------------------------------------
class ImportRunner : public QObject
{
    Q_OBJECT
public:

	/*!
	 *  The constructor.
	 *
	 *      @param [in] parameterFinder     Pointer to the parameter finder.
     *      @param [in] displayTabs         The container widget for read and displayed source files.
	 *      @param [in] parent              Pointer to the owner of this runner.
	 */
	ImportRunner(QSharedPointer<ComponentParameterFinder> parameterFinder, QTabWidget* displayTabs, 
        QObject* parent);

	//! The destructor.
	~ImportRunner();
  
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
    void loadPlugins(PluginManager const& pluginManager);

    /*!
     *  Runs all import plugins with matching file types for a given file and component.
     *
     *      @param [in] filePath            The relative path to the input file.
     *      @param [in] componentXmlPath    The path to the target component XML file.
     *      @param [in] targetComponent     The target component to which import all parsed elements.     
     *
     *      @return The component with all the imported elements.     
     */
    QSharedPointer<Component> run(QString const& filePath, QString const& componentXmlPath, 
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
     *  Adds model parameter visualization to a given import plugin if possible.
     *
     *      @param [in] parser   The plugin to add visualization to.     
     */
    void addModelParameterVisualizationIfPossible(ImportPlugin* importPlugin) const;
       
    /*!
     *  Adds expression parser to a given import plugin if possible.
     *
     *      @param [in] parser   The plugin to add expression parser to.     
     */
    void addExpressionParserIfPossible(ImportPlugin* importPlugin) const;

    /*!
     *  Scrolls the given display widget to the first highlighted row.
     *
     *      @param [in] sourceDisplay   The display to scroll.
     */
    void scrollSourceDisplayToFirstHighlight(QPlainTextEdit* sourceDisplay) const;
    
    /*!
     *  Imports all includes from the dependency files of a given file.
     *
     *      @param [in] filePath            The file whose dependency files to import.
     *      @param [in] componentXmlPath    The path to the target component XML file.
     *      @param [in] importComponent     The component to import to.
     */
    void importIncludes(QString const& filePath, QString const& componentXmlPath, 
        QSharedPointer<Component> importComponent);
    
     /*!
      *  Finds the include import plugins for the given file types.
      *
      *      @param [in] filetypes   The file types to find plugins for.
      *
      *      @return The found include import plugins.
      */
     QList<ImportPlugin*> includeImportPluginsForFileTypes(QStringList const& filetypes) const;

    /*!
     *  Finds the file types of a given file.
     *
     *      @param [in] importFile          The relative path to the file.
     *      @param [in] fileSets            The file sets to find the file and its file types from.
     *
     *      @return File types of the given file.
     */
    QStringList filetypesOf(QString const& fileName, QList<QSharedPointer<FileSet> > const& fileSets) const;

    /*!
     *  Finds possible source analyzer plugins for given file types.
     *
     *      @param [in] filetypes   The file types to search for.
     *
     *      @return Import source analyzers capable of analyzing the given file types.
     */
    QList<ISourceAnalyzerPlugin*> analyzerPluginsForFileTypes(QStringList filetypes) const;

    /*!
     *  Imports a file to the given component.
     *
     *      @param [in] filePath                The path to the file to import.
     *      @param [in] absoluteBasePath        The absolute base path for relative filePaths.
     *      @param [in] importPluginsForFile    The import plugins to use.
     *      @param [in] importComponent         The component to import to.
     */
    void importFile(QString const& filePath, QString const& absoluteBasePath, 
        QList<ImportPlugin*> importPluginsForFile, QSharedPointer<Component> importComponent);
        
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
     *  Creates a display widget for the given file.
     *
     *      @param [in] filePath    Path to the file.
     *
     *      @return A display widget for the given file.
     */
    QPlainTextEdit* createSourceDisplayForFile(QString const& filePath) const;
      
    /*!
     *  Finds possible import plugins for given file types.
     *
     *      @param [in] filetypes   The file types to search for.
     *
     *      @return Import plugins capable of reading the given file types.
     */
    QList<ImportPlugin*> importPluginsForFileTypes(QStringList const& filetypes) const;
    
    /*!
     *  Adds highlighting to a given import plugin if possible.
     *
     *      @param [in] parser          The plugin to add highlighting to.   
     *      @param [in] highlighter     The highligter to add to the plugin.   
     */
    void addHighlightIfPossible(ImportPlugin* parser, Highlighter* highlighter) const;
   
    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The expression parser to use for verilog expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! The model parameter visualizer used by all import plugins.
    ModelParameterVisualizer* modelParameterVisualizer_;

    //! All loaded import plugins.
    QList<ImportPlugin*> ImportPlugins_;

    //! All loaded analyzer plugins.
    QList<ISourceAnalyzerPlugin*> analyzerPlugins_;

    //! The parameter finder.
    QSharedPointer <ComponentParameterFinder> parameterFinder_;

    //! The container widget for source display widgets.
    QTabWidget* displayTabs_;
};

#endif // IMPORTRUNNER_H
