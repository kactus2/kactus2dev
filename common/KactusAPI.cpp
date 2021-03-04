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

#include <VersionHelper.h>
#include <common/ui/ConsoleMediator.h>

#include <IPXACTmodels/Component/Component.h>
#include <editors/ComponentEditor/common/IPXactSystemVerilogParser.h>

#include <wizards/ComponentWizard/ImportRunner.h>

#include <PluginS/PluginSystem/PluginManager.h>

LibraryInterface* KactusAPI::library_ = nullptr;
MessageMediator* KactusAPI::messageChannel_ = new ConsoleMediator();

//-----------------------------------------------------------------------------
// Function: KactusAPI::KactusAPI()
//-----------------------------------------------------------------------------
KactusAPI::KactusAPI(LibraryInterface* library, MessageMediator* messageChannel)
{
    if (library_ == nullptr)
    {
        library_ = library;        
    }

    if (messageChannel != nullptr)
    {
        delete messageChannel_;
        messageChannel_ = messageChannel;
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getLibrary()
//-----------------------------------------------------------------------------
LibraryInterface* KactusAPI::getLibrary()
{
    Q_ASSERT(library_ != nullptr);

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
    return "Kactus2 " + VersionHelper::versionFull();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::getLibraryPaths()
//-----------------------------------------------------------------------------
QStringList KactusAPI::getLibraryPaths()
{
    QSettings settings;

    return  settings.value("Library/ActiveLocations").toStringList();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
void KactusAPI::setLibraryPaths(QStringList const& activeLocations, QStringList const& allLocations)
{
    QSettings settings;

    QStringList oldLocations = settings.value("Library/Locations").toStringList();
    QStringList newLocations = allLocations;

    if (allLocations.empty())
    {
        newLocations = oldLocations;
    }

    QStringList oldActives = settings.value("Library/ActiveLocations").toStringList();
    QStringList newActives = activeLocations;

    for (auto location : newActives)
    {
        if (newLocations.contains(location) == false)
        {
            newLocations.append(location);
        }
    }

    if (newLocations != oldLocations || newActives != oldActives)
    {
        settings.setValue("Library/Locations", newLocations);
        settings.setValue("Library/ActiveLocations", newActives);
        library_->searchForIPXactFiles();
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
QString KactusAPI::getDefaultLibraryPath()
{
    QSettings settings;
    return settings.value("Library/DefaultLocation").toString();
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::setLibraryPaths()
//-----------------------------------------------------------------------------
void KactusAPI::setDefaultLibraryPath(QString const& path)
{
    QSettings settings;

    QStringList locations = settings.value("Library/Locations").toStringList();
    if (locations.contains(path))
    {
        settings.setValue("Library/DefaultLocation", path);
    }
}

//-----------------------------------------------------------------------------
// Function: KactusAPI::importFile()
//-----------------------------------------------------------------------------
void KactusAPI::importFile(QString const& filePath, VLNV const& targetVLNV, bool overwrite)
{
    bool existing = library_->contains(targetVLNV);

    if (existing && overwrite == false)
    {
        return;
    }

    QSharedPointer<Component const> existingComponent(nullptr);
    QString xmlPath = getDefaultLibraryPath() + "/" + targetVLNV.toString("/") + "/" + targetVLNV.getName() + ".xml";

    if (existing)
    {
        existingComponent = library_->getModel<Component>(targetVLNV);
        xmlPath = library_->getPath(targetVLNV);
    }
    else
    {
        existingComponent = QSharedPointer<Component const>(new Component(targetVLNV));
    }

    QSharedPointer<ComponentParameterFinder> parameterFinder(new ComponentParameterFinder(existingComponent));
    QSharedPointer<ExpressionParser> expressionParser(new IPXactSystemVerilogParser(parameterFinder));

    ImportRunner runner(parameterFinder, nullptr, nullptr);
    runner.setExpressionParser(expressionParser);
    runner.loadPlugins(PluginManager::getInstance());

    QString xmlPath = getDefaultLibraryPath() + "/" + targetVLNV.toString("/") + "/" + targetVLNV.getName() + ".xml";

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
    }
}
