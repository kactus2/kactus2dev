//-----------------------------------------------------------------------------
// File: ImportRunner.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 18.09.2014
//
// Description:
// Runs import plugins for a given file and component.
//-----------------------------------------------------------------------------

#include "ImportRunner.h"

#include <Plugins/PluginSystem/IPlugin.h>
#include <Plugins/PluginSystem/ISourceAnalyzerPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/ImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/IncludeImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ExpressionSupport.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

#include <editors/ComponentEditor/common/NullParser.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/fileset.h>
#include <IPXACTmodels/file.h>

#include <QApplication>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextStream>
#include <QTextBlock>

//-----------------------------------------------------------------------------
// Function: ImportRunner::ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::ImportRunner(QSharedPointer<ComponentParameterFinder> parameterFinder, QTabWidget* displayTabs,
    QObject* parent)
    : QObject(parent), 
    expressionParser_(new NullParser()),
    modelParameterVisualizer_(0),
    ImportPlugins_(),
    analyzerPlugins_(),
    parameterFinder_(parameterFinder),
    displayTabs_(displayTabs)
{
    
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::~ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::~ImportRunner()
{

}

//-----------------------------------------------------------------------------
// Function: ImportRunner::run()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ImportRunner::run(QString const& filePath, QString const& componentXmlPath,
    QSharedPointer<const Component> targetComponent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);    
    displayTabs_->clear();

    QSharedPointer<Component> importComponent(new Component(*targetComponent.data()));
    parameterFinder_->setComponent(importComponent);

    importIncludes(filePath, componentXmlPath, importComponent);

    QStringList filetypes = filetypesOf(filePath, importComponent->getFileSets());
    importFile(filePath, componentXmlPath, importPluginsForFileTypes(filetypes), importComponent);

    QApplication::restoreOverrideCursor();

    return importComponent;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importFileTypes()
//-----------------------------------------------------------------------------
QStringList ImportRunner::importFileTypes() const
{
    QStringList fileTypes;

    foreach(ImportPlugin* parser, ImportPlugins_)
    {
        fileTypes.append(parser->getSupportedFileTypes());
    }
    fileTypes.removeDuplicates();

    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::loadPlugins()
//-----------------------------------------------------------------------------
void ImportRunner::loadPlugins(PluginManager const& pluginManager)
{
    foreach(IPlugin* plugin, pluginManager.getActivePlugins())
    {
        ImportPlugin* importPlugin = dynamic_cast<ImportPlugin*>(plugin);
        ISourceAnalyzerPlugin* analyzerPlugin = dynamic_cast<ISourceAnalyzerPlugin*>(plugin);
        if (importPlugin)
        {            
            ImportPlugins_.append(importPlugin);

            addModelParameterVisualizationIfPossible(importPlugin);
            addExpressionParserIfPossible(importPlugin);
        }

        if (analyzerPlugin)
        {
            analyzerPlugins_.append(analyzerPlugin);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::setVerilogExpressionParser()
//-----------------------------------------------------------------------------
void ImportRunner::setVerilogExpressionParser(QSharedPointer<ExpressionParser> parser)
{
    expressionParser_ = parser;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::setPortVisualizer()
//-----------------------------------------------------------------------------
void ImportRunner::setModelParameterVisualizer(ModelParameterVisualizer* visualizer)
{
    modelParameterVisualizer_ = visualizer;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::addModelParameterVisualizationIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addModelParameterVisualizationIfPossible(ImportPlugin* importPlugin) const
{
    if (modelParameterVisualizer_)
    {
        ModelParameterSource* modelParameterSource = dynamic_cast<ModelParameterSource*>(importPlugin);
        if (modelParameterSource)
        {
            modelParameterSource->setModelParameterVisualizer(modelParameterVisualizer_);        
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::addExpressionParserIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addExpressionParserIfPossible(ImportPlugin* importPlugin) const
{
    ExpressionSupport* pluginRequiringParser = dynamic_cast<ExpressionSupport*>(importPlugin);
    if (pluginRequiringParser)
    {
        pluginRequiringParser->setExpressionParser(expressionParser_);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::scrollToFirstHighlight()
//-----------------------------------------------------------------------------
void ImportRunner::scrollSourceDisplayToFirstHighlight(QPlainTextEdit* sourceDisplay) const
{
    QTextCursor cursor = sourceDisplay->textCursor();
    cursor.movePosition(QTextCursor::Start);
    QTextCharFormat initialFormat = cursor.charFormat();

    while (cursor.movePosition(QTextCursor::NextBlock) && cursor.charFormat() == initialFormat)
    {
    }

    if (!cursor.atEnd())
    {
        int row = cursor.block().firstLineNumber();
        sourceDisplay->verticalScrollBar()->setValue(row);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importIncludes()
//-----------------------------------------------------------------------------
void ImportRunner::importIncludes(QString const& filePath, QString const& componentXmlPath, 
    QSharedPointer<Component> importComponent)
{
    if (filePath.isEmpty())
    {
        return;
    }

    QList<FileDependencyDesc> dependencies;
    QString componentPath = QFileInfo(componentXmlPath).absolutePath() + "/";
    QStringList filetypes = filetypesOf(filePath, importComponent->getFileSets());

    foreach(ISourceAnalyzerPlugin* analyzer, analyzerPluginsForFileTypes(filetypes))
    {
        analyzer->beginAnalysis(importComponent.data(), componentPath);
        dependencies.append(analyzer->getFileDependencies(importComponent.data(), componentPath, 
            General::getAbsolutePath(componentXmlPath, filePath)));
        analyzer->endAnalysis(importComponent.data(), componentPath);
    }

    QList<ImportPlugin*> importPlugins = includeImportPluginsForFileTypes(filetypes);
    QString basePath = QFileInfo(General::getAbsolutePath(componentXmlPath, filePath)).absolutePath() + "/";
    foreach(FileDependencyDesc dependency, dependencies)
    {
        importFile(dependency.filename, basePath, importPlugins, importComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::filetypesOf()
//-----------------------------------------------------------------------------
QStringList ImportRunner::filetypesOf(QString const& fileName, 
    QList<QSharedPointer<FileSet> > const& fileSets) const
{
    QStringList fileTypes;

    QRegularExpression filePattern("(?:^|[\\\\/])" + fileName + "$");

    foreach (QSharedPointer<FileSet> fileSet, fileSets)
    {
        foreach(QSharedPointer<File> file, fileSet->getFiles())
        {
            if (filePattern.match(file->getName()).hasMatch())
            {
                fileTypes.append(file->getAllFileTypes());
            }
        }
    }

    fileTypes.removeDuplicates();

    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::analyzerPluginsForFileTypes()
//-----------------------------------------------------------------------------
QList<ISourceAnalyzerPlugin*> ImportRunner::analyzerPluginsForFileTypes(QStringList filetypes) const
{
    QList<ISourceAnalyzerPlugin*> analysersForFiletype;
    foreach(ISourceAnalyzerPlugin* analyzer, analyzerPlugins_)
    {
        QStringList supportedTypes = analyzer->getSupportedFileTypes();
        foreach(QString filetype, filetypes)
        {
            if (supportedTypes.contains(filetype) && !analysersForFiletype.contains(analyzer))
            {
                analysersForFiletype.append(analyzer);
            }
        }
    }

    return analysersForFiletype;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importFile()
//-----------------------------------------------------------------------------
void ImportRunner::importFile(QString const& filePath, QString const& absoluteBasePath,
    QList<ImportPlugin*> importPluginsForFile, QSharedPointer<Component> importComponent)
{
    if (filePath.isEmpty())
    {
        return;
    }

    QPlainTextEdit* sourceDisplayer = createSourceDisplayForFile(filePath);
    QString const& fileContent = readInputFile(filePath, absoluteBasePath);
    sourceDisplayer->setPlainText(fileContent);

    Highlighter* highlighter = new ImportHighlighter(sourceDisplayer, this);
    highlighter->applyFontColor(fileContent, Qt::gray);

    QStringList compatibilityWarnings;
    foreach(ImportPlugin* parser, importPluginsForFile)
    {
        compatibilityWarnings.append(parser->getCompatibilityWarnings());

        addHighlightIfPossible(parser, highlighter);
        parser->import(fileContent, importComponent);
        addHighlightIfPossible(parser, 0);
    }
    compatibilityWarnings.removeAll("");
    emit noticeMessage(compatibilityWarnings.join("\n"));

    displayTabs_->setCurrentIndex(displayTabs_->indexOf(sourceDisplayer));
    scrollSourceDisplayToFirstHighlight(sourceDisplayer);

    delete highlighter;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::createSourceDisplayForFile()
//-----------------------------------------------------------------------------
QPlainTextEdit* ImportRunner::createSourceDisplayForFile(QString const& filePath) const
{
    QPlainTextEdit* sourceDisplay = new QPlainTextEdit(displayTabs_);

    QFont font("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setFixedPitch(true);
    font.setPointSize(9);

    sourceDisplay->setFont(font);
    sourceDisplay->setTabStopWidth(4 * sourceDisplay->fontMetrics().width(' '));
    sourceDisplay->setReadOnly(true);
    sourceDisplay->setCursorWidth(0);

    displayTabs_->insertTab(0, sourceDisplay, QFileInfo(filePath).fileName());

    return sourceDisplay;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::readInputFile()
//-----------------------------------------------------------------------------
QString ImportRunner::readInputFile(QString const& relativePath, QString const& basePath) const
{
    QString fileContent = "";

    QString absoluteFilePath = General::getAbsolutePath(basePath, relativePath);

    QFile importFile(absoluteFilePath);
    if (QFileInfo(absoluteFilePath).exists() && importFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&importFile);
        fileContent = stream.readAll();
        fileContent.replace("\r\n", "\n");
        importFile.close();
    }
    else
    {
        fileContent = tr("<File %1 not found in the file sets.>").arg(relativePath);
    }

    return fileContent;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importPluginsForFileTypes()
//-----------------------------------------------------------------------------
QList<ImportPlugin*> ImportRunner::importPluginsForFileTypes(QStringList const& filetypes) const
{
    QList<ImportPlugin*> compatiblePlugins;
    foreach(ImportPlugin* importer, ImportPlugins_)
    {
        QStringList parserAcceptedFiletypes = importer->getSupportedFileTypes();
        foreach(QString filetype, filetypes)
        {
            if (parserAcceptedFiletypes.contains(filetype) && !compatiblePlugins.contains(importer))
            {
                compatiblePlugins.prepend(importer);
            }
        }
    }

    return compatiblePlugins;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::includeImportPluginsForFileTypes()
//-----------------------------------------------------------------------------
QList<ImportPlugin*> ImportRunner::includeImportPluginsForFileTypes(QStringList const& filetypes) const
{
    QList<ImportPlugin*> compatiblePlugins = importPluginsForFileTypes(filetypes);
    foreach(ImportPlugin* importer, compatiblePlugins)
    {
        if (dynamic_cast<IncludeImportPlugin*>(importer) == 0)
        {
            compatiblePlugins.removeAll(importer);
        }

    }

    return compatiblePlugins;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::addHighlightIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addHighlightIfPossible(ImportPlugin* parser, Highlighter* highlighter) const
{
    HighlightSource* highlightSource = dynamic_cast<HighlightSource*>(parser);
    if (highlightSource)
    {
        highlightSource->setHighlighter(highlighter);
    }
}
