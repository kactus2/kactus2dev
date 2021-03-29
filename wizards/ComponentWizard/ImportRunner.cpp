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
#include <Plugins/PluginSystem/ImportPlugin/ExpressionSupport.h>

#include <editors/common/FileHandler/FileHandler.h>
#include <editors/ComponentEditor/common/NullParser.h>

#include <wizards/ComponentWizard/ImportEditor/ImportHighlighter.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/File.h>

#include <QApplication>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextStream>
#include <QTextBlock>
#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: ImportRunner::ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::ImportRunner(QSharedPointer<ComponentParameterFinder> parameterFinder, QTabWidget* displayTabs,
    QObject* parent):
QObject(parent),
expressionParser_(new NullParser()),
ImportPlugins_(),
analyzerPlugins_(),
parameterFinder_(parameterFinder),
displayTabs_(displayTabs),
fileSuffixTable_(FileHandler::constructFileSuffixTable()),
componentsInFile_()
{
    
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::gatherComponentsFromFile()
//-----------------------------------------------------------------------------
QStringList ImportRunner::constructComponentDataFromFile(QString const& filePath, QString const& componentXMLPath,
    QSharedPointer<const Component> targetComponent)
{
    componentsInFile_.clear();

    QStringList filetypes = filetypesOf(filePath, *targetComponent->getFileSets().data());
    QString const& fileContent = readInputFile(filePath, componentXMLPath);

    QStringList availableComponentNames;

    for (ImportPlugin const* parser : importPluginsForFileTypes(filetypes))
    {
        QStringList possibleComponents = parser->getFileComponents(fileContent);
        for (auto component : possibleComponents)
        {
            QString componentName = parser->getComponentName(component);
            availableComponentNames.append(componentName);

            ImportRunner::AvailableComponent newComponent{ componentName, component };
            componentsInFile_.append(newComponent);
        }
    }

    return availableComponentNames;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::run()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ImportRunner::run(QString const& componentName, QString const& filePath,
    QString const& componentXmlPath, QSharedPointer<const Component> targetComponent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);

    if (displayTabs_ != nullptr)
    {
        displayTabs_->clear();
    }

    QSharedPointer<Component> importComponent(new Component(*targetComponent.data()));
    parameterFinder_->setComponent(importComponent);

    importIncludes(filePath, componentXmlPath, importComponent);

    QStringList filetypes = filetypesOf(filePath, *importComponent->getFileSets().data());
   
    importFile(componentName, filePath, componentXmlPath, importPluginsForFileTypes(filetypes), importComponent);

    QApplication::restoreOverrideCursor();

    return importComponent;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importFileTypes()
//-----------------------------------------------------------------------------
QStringList ImportRunner::importFileTypes() const
{
    QStringList fileTypes;

    for (ImportPlugin const* parser : ImportPlugins_)
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
    for (IPlugin* plugin : pluginManager.getActivePlugins())
    {
        ImportPlugin* importPlugin = dynamic_cast<ImportPlugin*>(plugin);
        ISourceAnalyzerPlugin* analyzerPlugin = dynamic_cast<ISourceAnalyzerPlugin*>(plugin);
        if (importPlugin)
        {            
            ImportPlugins_.append(importPlugin);
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
void ImportRunner::setExpressionParser(QSharedPointer<ExpressionParser> parser)
{
    expressionParser_ = parser;
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
        // Loop until first formatted block is found.
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
    QStringList filetypes = filetypesOf(filePath, *importComponent->getFileSets().data());

    for (ISourceAnalyzerPlugin* analyzer : analyzerPluginsForFileTypes(filetypes))
    {
        analyzer->beginAnalysis(importComponent.data(), componentPath);
        dependencies.append(analyzer->getFileDependencies(importComponent.data(), componentPath, 
            General::getAbsolutePath(componentXmlPath, filePath)));
        analyzer->endAnalysis(importComponent.data(), componentPath);
    }

    QList<ImportPlugin*> importPlugins = includeImportPluginsForFileTypes(filetypes);
    QString basePath = QFileInfo(General::getAbsolutePath(componentXmlPath, filePath)).absolutePath() + "/";

    for (FileDependencyDesc const& dependency : dependencies)
    {        
        importFile(QString(""), dependency.filename, basePath, importPlugins, importComponent);
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::filetypesOf()
//-----------------------------------------------------------------------------
QStringList ImportRunner::filetypesOf(QString const& fileName, QList<QSharedPointer<FileSet> > const& fileSets)
    const
{
    QStringList fileTypes;

    QRegularExpression filePattern("(?:^|[\\\\/])" + fileName + "$");

    for (auto const& fileSet : fileSets)
    {
        for (auto const& file : *fileSet->getFiles())
        {
            if (filePattern.match(file->name()).hasMatch())
            {
                fileTypes.append(*file->getFileTypes());
            }
        }
    }

    fileTypes.removeDuplicates();

    if (fileTypes.isEmpty() && !fileName.isEmpty())
    {
        QString fileSuffix = QFileInfo(fileName).suffix();
        fileTypes.append(FileHandler::getFileTypeForSuffix(fileSuffixTable_, fileSuffix));
    }

    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::analyzerPluginsForFileTypes()
//-----------------------------------------------------------------------------
QList<ISourceAnalyzerPlugin*> ImportRunner::analyzerPluginsForFileTypes(QStringList filetypes) const
{
    QList<ISourceAnalyzerPlugin*> analysersForFiletype;
    for (ISourceAnalyzerPlugin* analyzer : analyzerPlugins_)
    {
        QStringList supportedTypes = analyzer->getSupportedFileTypes();
        for (QString const& filetype : filetypes)
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
void ImportRunner::importFile(QString const& componentName, QString const& filePath,
    QString const& absoluteBasePath, QList<ImportPlugin *> importPluginsForFile,
    QSharedPointer<Component> importComponent)
{

    QString componentDeclaration = getComponentFromFile(componentName);
    if (filePath.isEmpty() || (componentDeclaration.isEmpty() && !componentName.isEmpty()))
    {
        return;
    }

    QString const& fileContent = readInputFile(filePath, absoluteBasePath);

    QPlainTextEdit* sourceDisplayer = nullptr;
    Highlighter* highlighter = nullptr;

    if (displayTabs_ != nullptr)
    {
        sourceDisplayer = createSourceDisplayForFile(filePath);
        sourceDisplayer->setPlainText(fileContent);

        highlighter = new ImportHighlighter(sourceDisplayer, this);
        highlighter->applyFontColor(fileContent, Qt::gray);
    }

    QStringList compatibilityWarnings;
    for (ImportPlugin* parser : importPluginsForFile)
    {
        compatibilityWarnings.append(parser->getCompatibilityWarnings());
        
        HighlightSource* highlightSource = dynamic_cast<HighlightSource*>(parser);
        if (highlightSource)
        {
            highlightSource->setHighlighter(highlighter);
            parser->import(fileContent, componentDeclaration, importComponent);
            highlightSource->setHighlighter(nullptr);
        }
        else
        {
            parser->import(fileContent, componentDeclaration, importComponent);
        }
    }
    compatibilityWarnings.removeAll("");
    emit noticeMessage(compatibilityWarnings.join("\n"));


    if (displayTabs_ != nullptr)
    {
        displayTabs_->setCurrentIndex(displayTabs_->indexOf(sourceDisplayer));
        scrollSourceDisplayToFirstHighlight(sourceDisplayer);

        delete highlighter;
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::getComponentFromFile()
//-----------------------------------------------------------------------------
QString ImportRunner::getComponentFromFile(QString const& componentName) const
{
    if (!componentName.isEmpty())
    {
        for (auto component : componentsInFile_)
        {
            if (component.componentName_.compare(componentName) == 0)
            {
                return component.componentFile_;
            }
        }
    }

    return QString();
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
    QFile importedFile(absoluteFilePath);
    if (QFileInfo(absoluteFilePath).exists() && importedFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&importedFile);
        fileContent = stream.readAll();
        fileContent.replace("\r\n", "\n");

        importedFile.close();
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
    for (ImportPlugin* importer : ImportPlugins_)
    {
        if (dynamic_cast<IncludeImportPlugin*>(importer) == 0)
        {
            QStringList parserAcceptedFiletypes = importer->getSupportedFileTypes();
            for (QString const& filetype : filetypes)
            {
                if (parserAcceptedFiletypes.contains(filetype) && !compatiblePlugins.contains(importer))
                {
                    compatiblePlugins.prepend(importer);
                }
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
    QList<ImportPlugin*> compatiblePlugins;
    for (ImportPlugin* importer : ImportPlugins_)
    {
        if (dynamic_cast<IncludeImportPlugin*>(importer) != 0)
        {
            QStringList parserAcceptedFiletypes = importer->getSupportedFileTypes();
            for (QString const& filetype : filetypes)
            {
                if (parserAcceptedFiletypes.contains(filetype) && !compatiblePlugins.contains(importer))
                {
                    compatiblePlugins.prepend(importer);
                }
            }
        }
    }

    return compatiblePlugins;
}
