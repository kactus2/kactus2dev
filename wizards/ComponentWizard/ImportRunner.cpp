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
#include <Plugins/PluginSystem/ImportPlugin/ImportPlugin.h>
#include <Plugins/PluginSystem/ImportPlugin/HighlightSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ExpressionSupport.h>

#include <editors/ComponentEditor/common/NullParser.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>

#include <QApplication>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: ImportRunner::ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::ImportRunner(QSharedPointer<ComponentParameterFinder> parameterFinder, QObject* parent)
    : QObject(parent), ImportPlugins_(), highlighter_(0), expressionParser_(new NullParser()),
    modelParameterVisualizer_(0),
    parameterFinder_(parameterFinder)
{
    
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::~ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::~ImportRunner()
{

}

//-----------------------------------------------------------------------------
// Function: ImportRunner::parse()
//-----------------------------------------------------------------------------
QSharedPointer<Component> ImportRunner::import(QString const& importFile, QString const& componentXmlPath,
    QSharedPointer<const Component> targetComponent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);    

    QString const& fileContent = readInputFile(importFile, componentXmlPath);
    if (highlighter_)
    {
        highlighter_->applyFontColor(fileContent, Qt::gray);
    }

    QSharedPointer<Component> importComponent(new Component(*targetComponent.data()));

    parameterFinder_->setComponent(importComponent);

    QStringList filetypes = filetypesOfImportFile(importFile, importComponent);
    QStringList parserCompatibilityWarnings;

    foreach(ImportPlugin* parser, parsersForFileTypes(filetypes))
    {
        parserCompatibilityWarnings.append(parser->getCompatibilityWarnings());
        parser->import(fileContent, importComponent);
    }
    parserCompatibilityWarnings.removeAll("");
    emit noticeMessage(parserCompatibilityWarnings.join("\n"));

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
// Function: ImportRunner::filetypesOfImportFile()
//-----------------------------------------------------------------------------
QStringList ImportRunner::filetypesOfImportFile(QString const& importFile, 
    QSharedPointer<Component> targetComponent) const
{
    QStringList fileTypes;

    QList<File*> files;
    targetComponent->getFiles(importFile, files);

    if (files.size() == 1)
    {
        fileTypes.append(files.first()->getAllFileTypes());
    }
      
    fileTypes.removeDuplicates();

    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::parsersForFileTypes()
//-----------------------------------------------------------------------------
QList<ImportPlugin*> ImportRunner::parsersForFileTypes(QStringList const& filetypes) const
{
    QList<ImportPlugin*> parsersForFiletype;
    foreach(ImportPlugin* parser, ImportPlugins_)
    {
        QStringList parserAcceptedFiletypes = parser->getSupportedFileTypes();
        foreach(QString filetype, filetypes)
        {
            if (parserAcceptedFiletypes.contains(filetype) && !parsersForFiletype.contains(parser))
            {
                parsersForFiletype.prepend(parser);
            }
        }
    }

    return parsersForFiletype;
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::readInputFile()
//-----------------------------------------------------------------------------
QString ImportRunner::readInputFile(QString const& relativePath, QString const& basePath) const
{
    QString fileContent;

    QString absoluteFilePath = General::getAbsolutePath(basePath, relativePath);

    QFile importFile(absoluteFilePath);
    if (QFileInfo(absoluteFilePath).exists() && importFile.open(QIODevice::ReadOnly))
    {
        QTextStream stream(&importFile);
        fileContent = stream.readAll();
        importFile.close();
    }
      
    return fileContent.replace("\r\n", "\n");
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::loadImportPlugins()
//-----------------------------------------------------------------------------
void ImportRunner::loadImportPlugins(PluginManager const& pluginManager)
{
    foreach(IPlugin* plugin, pluginManager.getActivePlugins())
    {
        ImportPlugin* importPlugin = dynamic_cast<ImportPlugin*>(plugin);
        if (importPlugin)
        {            
            ImportPlugins_.append(importPlugin);

            addHighlightIfPossible(importPlugin);
            addModelParameterVisualizationIfPossible(importPlugin);
            addExpressionParserIfPossible(importPlugin);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::setHighlighter()
//-----------------------------------------------------------------------------
void ImportRunner::setHighlighter(Highlighter* highlighter)
{
    highlighter_ =  highlighter;
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
// Function: ImportRunner::addHighlightIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addHighlightIfPossible(ImportPlugin* parser)
{
    if (highlighter_)
    {
        HighlightSource* highlightSource = dynamic_cast<HighlightSource*>(parser);
        if (highlightSource)
        {
            highlightSource->setHighlighter(highlighter_);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::addModelParameterVisualizationIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addModelParameterVisualizationIfPossible(ImportPlugin* parser)
{
    if (modelParameterVisualizer_)
    {
        ModelParameterSource* modelParameterSource = dynamic_cast<ModelParameterSource*>(parser);
        if (modelParameterSource)
        {
            modelParameterSource->setModelParameterVisualizer(modelParameterVisualizer_);        
        }        
    }
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::addExpressionParserIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addExpressionParserIfPossible(ImportPlugin* importPlugin)
{
    ExpressionSupport* pluginRequiringParser = dynamic_cast<ExpressionSupport*>(importPlugin);
    if (pluginRequiringParser)
    {
        pluginRequiringParser->setExpressionParser(expressionParser_);
    }
}