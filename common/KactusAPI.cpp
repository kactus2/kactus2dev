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

/*
std::vector<std::string> KactusAPI::listPlugins()
{
    std::vector<std::string> names;

    for (auto plugin : PluginManager::getInstance().getActivePlugins())
    {
        names.push_back(plugin->getName().toStdString());
    }

    return names;
}

*/
