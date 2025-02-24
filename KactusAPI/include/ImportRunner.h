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

#include <KactusAPI/KactusAPIGlobal.h>

#include <PluginManager.h>

#include <ComponentParameterFinder.h>

#include <QObject>
#include <QSharedPointer>
#include <QTabWidget>
#include <QPlainTextEdit>
#include <QMap>

class Component;
class ExpressionParser;
class FileSet;
class Highlighter;
class ImportPlugin;
class ISourceAnalyzerPlugin;

//-----------------------------------------------------------------------------
//! Runs import plugins for a given file and component.
//-----------------------------------------------------------------------------
class KACTUS2_API ImportRunner : public QObject
{
    Q_OBJECT
public:

	/*!
	 *  The constructor.
	 *
	 *    @param [in] parameterFinder     Pointer to the parameter finder.
     *    @param [in] displayTabs         The container widget for read and displayed source files.
	 *    @param [in] parent              Pointer to the owner of this runner.
	 */
	ImportRunner(QSharedPointer<ComponentParameterFinder> parameterFinder, QTabWidget* displayTabs, 
        QObject* parent);

	//! The destructor.
	virtual ~ImportRunner() = default;
  
    /*!
     *  Sets the expression parser to be used by all importers.
     *
     *    @param [in] parser   The parser to use.          
     */
    void setExpressionParser(QSharedPointer<ExpressionParser> parser);

    /*!
     *  Loads all available import plugins.
     *
     *    @param [in] pluginManager   The plugin manager owning all the plugins.
     */
    void loadPlugins(PluginManager const& pluginManager);

    /*!
     *  Locates component declarations from the selected file.
     *
     *    @param [in] filePath            Path to the selected file.
     *    @param [in] componentXMLPath    Path to the target component XML file.
     *    @param [in] targetComponent     Target component to which import all parsed elements.
     *
     *    @return List of component declarations found in the selected file.
     */
    QStringList constructComponentDataFromFile(QString const& filePath, QString const& componentXMLPath,
        QSharedPointer<const Component> targetComponent);

    /*!
     *  Runs all import plugins with matching file types for a given file and component.
     *
     *    @param [in] componentName       Name of thes selected component.
     *    @param [in] filePath            The relative path to the input file.
     *    @param [in] componentXmlPath    The path to the target component XML file.
     *    @param [in] targetComponent     The target component to which import all parsed elements.     
     *
     *    @return The component with all the imported elements.     
     */
    QSharedPointer<Component> run(QString const& componentName, QString const& filePath,
        QString const& componentXmlPath, QSharedPointer<const Component> targetComponent);

    /*!
     *  Gets all file types for which a import parser(s) can be run.
     *
     *    @return List of possible file types.
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
     *  Adds expression parser to a given import plugin if possible.
     *
     *    @param [in] parser   The plugin to add expression parser to.     
     */
    void addExpressionParserIfPossible(ImportPlugin* importPlugin) const;

    /*!
     *  Scrolls the given display widget to the first highlighted row.
     *
     *    @param [in] sourceDisplay   The display to scroll.
     */
    void scrollSourceDisplayToFirstHighlight(QPlainTextEdit* sourceDisplay) const;
    
    /*!
     *  Imports all includes from the dependency files of a given file.
     *
     *    @param [in] filePath            The file whose dependency files to import.
     *    @param [in] componentXmlPath    The path to the target component XML file.
     *    @param [in] importComponent     The component to import to.
     */
    void importIncludes(QString const& filePath, QString const& componentXmlPath, 
        QSharedPointer<Component> importComponent);
    
    /*!
     *  Finds the include import plugins for the given file types.
     *
     *    @param [in] filetypes   The file types to find plugins for.
     *
     *    @return The found include import plugins.
     */
    QList<ImportPlugin*> includeImportPluginsForFileTypes(QStringList const& filetypes) const;

    /*!
     *  Finds the file types of a given file.
     *
     *    @param [in] importFile          The relative path to the file.
     *    @param [in] fileSets            The file sets to find the file and its file types from.
     *
     *    @return File types of the given file.
     */
    QStringList filetypesOf(QString const& fileName, QList<QSharedPointer<FileSet> > const& fileSets) const;

    /*!
     *  Finds possible source analyzer plugins for given file types.
     *
     *    @param [in] filetypes   The file types to search for.
     *
     *    @return Import source analyzers capable of analyzing the given file types.
     */
    QList<ISourceAnalyzerPlugin*> analyzerPluginsForFileTypes(QStringList filetypes) const;

    /*!
     *  Imports a file to the given component.
     *
     *    @param [in] componentName           Name of the selected component.
     *    @param [in] filePath                The path to the file to import.
     *    @param [in] absoluteBasePath        The absolute base path for relative filePaths.
     *    @param [in] importPluginsForFile    The import plugins to use.
     *    @param [in] importComponent         The component to import to.
     */
    void importFile(QString const& componentName, QString const& filePath, QString const& absoluteBasePath,
        QList<ImportPlugin*> importPluginsForFile, QSharedPointer<Component> importComponent);

    /*!
     *  Reads the content of a given file.
     *
     *    @param [in] relativePath    Relative path to the file from basePath.
     *    @param [in] basePath        The from which to begin the relative path.
     *
     *    @return The file content.
     */
    QString readInputFile(QString const& relativePath, QString const& basePath) const;

    /*!
     *  Creates a display widget for the given file.
     *
     *    @param [in] filePath    Path to the file.
     *
     *    @return A display widget for the given file.
     */
    QPlainTextEdit* createSourceDisplayForFile(QString const& filePath) const;
      
    /*!
     *  Finds possible import plugins for given file types.
     *
     *    @param [in] filetypes   The file types to search for.
     *
     *    @return Import plugins capable of reading the given file types.
     */
    QList<ImportPlugin*> importPluginsForFileTypes(QStringList const& filetypes) const;
    
    /*!
     *  Get the selected component declaration.
     *
     *    @param [in] componentName   Name of the selected component declaration.
     *
     *    @return The selected component declaration.
     */
    QString getComponentFromFile(QString const& componentName) const;


    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Container for component declarations.
    struct AvailableComponent
    {
        //! Name of the component.
        QString componentName_;

        //! The component declaration.
        QString componentFile_;
    };

    //! The expression parser to use for verilog expressions.
    QSharedPointer<ExpressionParser> expressionParser_;

    //! All loaded import plugins.
    QList<ImportPlugin*> ImportPlugins_;

    //! All loaded analyzer plugins.
    QList<ISourceAnalyzerPlugin*> analyzerPlugins_;

    //! The parameter finder.
    QSharedPointer <ComponentParameterFinder> parameterFinder_;

    //! The container widget for source display widgets.
    QTabWidget* displayTabs_;

    //! Table containing file extensions as key combined with file types as value.
    QMultiMap<QString, QString> fileSuffixTable_;

    //! List of currently available component declarations.
    QVector<AvailableComponent> componentsInFile_;
};

#endif // IMPORTRUNNER_H
