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
#include <Plugins/PluginSystem/ImportPlugin/PortSource.h>
#include <Plugins/PluginSystem/ImportPlugin/ModelParameterSource.h>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/file.h>

#include <QApplication>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: ImportRunner::ImportRunner()
//-----------------------------------------------------------------------------
ImportRunner::ImportRunner(QObject* parent)
    : QObject(parent), ImportPlugins_(), portVisualizer_(), highlighter_()
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
void ImportRunner::parse(QString const& importFile, QString const& componentXmlPath, 
    QSharedPointer<Component> targetComponent)
{
    QApplication::setOverrideCursor(Qt::WaitCursor);    

    QStringList filetypes = filetypesOfImportFile(importFile, targetComponent);
    
    QString const& fileContent = readInputFile(importFile, componentXmlPath);

    foreach(ImportPlugin* parser, parsersForFileTypes(filetypes))
    {
        parser->runParser(fileContent, targetComponent);
    }

    QApplication::restoreOverrideCursor();
}

//-----------------------------------------------------------------------------
// Function: ImportRunner::importFileTypes()
//-----------------------------------------------------------------------------
QStringList ImportRunner::importFileTypes() const
{
    QStringList fileTypes;

    foreach(ImportPlugin* parser, ImportPlugins_)
    {
        fileTypes.append(parser->acceptedFileTypes());
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
        fileTypes.append(files.first()->getFileTypes());
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
        QStringList parserAcceptedFiletypes = parser->acceptedFileTypes();
        foreach(QString filetype, filetypes)
        {
            if (parserAcceptedFiletypes.contains(filetype) && !parsersForFiletype.contains(parser))
            {
                parsersForFiletype.append(parser);
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
    if (importFile.open(QIODevice::ReadOnly))
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
void ImportRunner::loadImportPlugins(PluginManager const &pluginManager)
{
    foreach(IPlugin* plugin, pluginManager.getActivePlugins())
    {
        ImportPlugin* parser = dynamic_cast<ImportPlugin*>(plugin);
        if (parser)
        {
            ImportPlugins_.append(parser);

            addHighlightIfPossible(parser);
            addPortVisualizationIfPossible(parser);
            addModelParameterVisualizationIfPossible(parser);
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
// Function: ImportRunner::setPortVisualizer()
//-----------------------------------------------------------------------------
void ImportRunner::setPortVisualizer(PortVisualizer* visualizer)
{
    portVisualizer_ = visualizer;
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
// Function: ImportRunner::addPortVisualizationIfPossible()
//-----------------------------------------------------------------------------
void ImportRunner::addPortVisualizationIfPossible(ImportPlugin* parser)
{
    if (portVisualizer_)
    {
        PortSource* portSource = dynamic_cast<PortSource*>(parser);
        if (portSource)
        {
            portSource->setPortVisualizer(portVisualizer_);
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
