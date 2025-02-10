//-----------------------------------------------------------------------------
// File: KactusAPI.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.02.2021
//
// Description:
// Defines the operations available for accessing Kactus2 core.
//-----------------------------------------------------------------------------

#include "KactusAPI.h"

#include <KactusAPI/include/VersionHelper.h>
#include <KactusAPI/include/ConsoleMediator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/DesignConfiguration/DesignConfiguration.h>

#include <KactusAPI/include/IPXactSystemVerilogParser.h>

#include <KactusAPI/include/LibraryHandler.h>

#include <IPlugin.h>
#include <IGeneratorPlugin.h>
#include <PluginUtilityAdapter.h>
#include <CLIGenerator.h>

#include <KactusAPI/include/ImportRunner.h>

#include <QFileInfo>

//-----------------------------------------------------------------------------
// Function: KactusAPI::KactusAPI()
//-----------------------------------------------------------------------------
KactusAPI::KactusAPI(MessageMediator* messageChannel)
{
    if (messageChannel != nullptr)
    {
        messageChannel_ = messageChannel;
        LibraryHandler::getInstance().setOutputChannel(messageChannel_);
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getLibrary()
//-----------------------------------------------------------------------------
LibraryInterface* KactusAPI::getLibrary()
{
    return library_;
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getMessageChannel()
//-----------------------------------------------------------------------------
MessageMediator* KactusAPI::getMessageChannel()
{
    return messageChannel_;
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getVersion()
//-----------------------------------------------------------------------------
QString KactusAPI::getVersion()
{
    return QStringLiteral("Kactus2 ") + VersionHelper::versionFull();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getVersionFileString()
//-----------------------------------------------------------------------------
QString KactusAPI::getVersionFileString()
{
    return VersionHelper::versionFileStr();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getLibraryPaths()
//-----------------------------------------------------------------------------
QStringList KactusAPI::getLibraryPaths()
{
    QSettings settings;

    return  settings.value(QStringLiteral("Library/ActiveLocations")).toStringList();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
void KactusAPI::setLibraryPaths(QStringList const& activeLocations, QStringList const& allLocations)
{
    QSettings settings;

    QStringList oldLocations = settings.value(QStringLiteral("Library/Locations")).toStringList();
    QStringList newLocations = allLocations;

    if (allLocations.empty())
    {
        newLocations = oldLocations;
    }

    QStringList oldActives = settings.value(QStringLiteral("Library/ActiveLocations")).toStringList();
    QStringList newActives = activeLocations;

    for (auto const& location : newActives)
    {
        if (newLocations.contains(location) == false)
        {
            newLocations.append(location);
        }
    }

    if (newLocations != oldLocations || newActives != oldActives)
    {
        settings.setValue(QStringLiteral("Library/Locations"), newLocations);
        settings.setValue(QStringLiteral("Library/ActiveLocations"), newActives);
        library_->searchForIPXactFiles();
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
QString KactusAPI::getDefaultLibraryPath()
{
    QSettings settings;
    return settings.value(QStringLiteral("Library/DefaultLocation")).toString();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
void KactusAPI::setDefaultLibraryPath(QString const& path)
{
    QSettings settings;

    QStringList locations = settings.value(QStringLiteral("Library/Locations")).toStringList();
    if (locations.contains(path))
    {
        settings.setValue(QStringLiteral("Library/DefaultLocation"), path);
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::importFile()
//-----------------------------------------------------------------------------
int KactusAPI::importFile(QString const& filePath, VLNV const& targetVLNV, bool overwrite)
{
    bool existing = library_->contains(targetVLNV);

    if (existing && overwrite == false)
    {
        return 0;
    }

    QSharedPointer<Component const> existingComponent(nullptr);
    QString xmlPath = getDefaultLibraryPath() + QStringLiteral("/") + targetVLNV.toString(QStringLiteral("/")) +
        QStringLiteral("/") + targetVLNV.getName() + QStringLiteral(".xml");

    if (existing)
    {
        existingComponent = library_->getModel<Component>(targetVLNV);
        xmlPath = library_->getPath(targetVLNV);
    }
    else
    {
        existingComponent = QSharedPointer<Component const>(new Component(targetVLNV, Document::Revision::Std14));
    }

    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(existingComponent));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    ImportRunner runner(parameterFinder, nullptr, nullptr);
    runner.setExpressionParser(expressionParser);
    runner.loadPlugins(PluginManager::getInstance());

    QStringList names = runner.constructComponentDataFromFile(filePath, xmlPath, existingComponent);

    if (names.isEmpty() == false)
    {
        QSharedPointer<Component> importedComponent = 
            runner.run(names.first(), filePath, xmlPath, existingComponent);
  
        if (existing)
        {
            library_->writeModelToFile(importedComponent);
        }
        else
        {
            library_->writeModelToFile(xmlPath, importedComponent);
        }

        return 1;
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::runGenerator()
//-----------------------------------------------------------------------------
void KactusAPI::runGenerator(IGeneratorPlugin* plugin, VLNV const& componentVLNV, QString const& viewName,
    QString const& outputDirectory, KactusAttribute::Implementation implementation, QWidget* parentWidget)
{
    QSharedPointer<Component> component = library_->getModel<Component>(componentVLNV);
    if (component == nullptr)
    {
        return;
    }

    VLNV designVLNV; 
    QSharedPointer<Design> design;
    
    VLNV configVLNV;
    if (implementation == KactusAttribute::SYSTEM)
    {
        configVLNV = component->getHierSystemRef(viewName);
    }
    else
    {
        configVLNV = component->getHierRef(viewName);
    }

    QSharedPointer<DesignConfiguration> designConfiguration;
    if (configVLNV.getType() == VLNV::DESIGNCONFIGURATION)
    {        
        designConfiguration = library_->getModel<DesignConfiguration>(configVLNV);
        designVLNV = designConfiguration->getDesignRef();
    }
    else if (configVLNV.getType() == VLNV::DESIGN)
    {
        designVLNV = configVLNV;
        configVLNV.clear();
    }

    if (designVLNV.isEmpty() == false)
    {
        design = library_->getModel<Design>(designVLNV);
    }

    PluginUtilityAdapter adapter(library_, messageChannel_, VersionHelper::createVersionString(), parentWidget);

    CLIGenerator* cliRunnable = dynamic_cast<CLIGenerator*>(plugin);
    if (parentWidget == nullptr && cliRunnable)
    {
        cliRunnable->runGenerator(&adapter, component, design, designConfiguration, viewName, outputDirectory);
    }
    else if (plugin != nullptr)
    {
        plugin->runGenerator(&adapter, component, design, designConfiguration);
    }    
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getPlugins()
//-----------------------------------------------------------------------------
QList<IPlugin*> KactusAPI::getPlugins()
{    
    return PluginManager::getInstance().getAllPlugins();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getDocumentFilePath()
//-----------------------------------------------------------------------------
QString KactusAPI::getDocumentFilePath(VLNV const& vlnv)
{
    return QFileInfo(library_->getPath(vlnv)).absolutePath();
}
