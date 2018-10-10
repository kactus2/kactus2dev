//-----------------------------------------------------------------------------
// File: PluginManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
// Date: 27.6.2012
//
// Description:
// Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------

#include "PluginManager.h"

#include "IPlugin.h"

#include <IPXACTmodels/utilities/XmlUtils.h>

#include <QCoreApplication>
#include <QDir>
#include <QPluginLoader>

//-----------------------------------------------------------------------------
// Function: PluginManager::~PluginManager()
//-----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
}

//-----------------------------------------------------------------------------
// Function: PluginManager::addPlugin()
//-----------------------------------------------------------------------------
void PluginManager::addPlugin(IPlugin* plugin)
{
    plugins_.append(plugin);
}

//-----------------------------------------------------------------------------
// Function: PluginManager::getPlugins()
//-----------------------------------------------------------------------------
QList<IPlugin*> PluginManager::getAllPlugins() const
{
    return plugins_;
}

//-----------------------------------------------------------------------------
// Function: PluginManager::getActivePlugins()
//-----------------------------------------------------------------------------
QList<IPlugin*> PluginManager::getActivePlugins() const
{
    QSettings settings;
    QList<IPlugin*> activePlugins;

    foreach (IPlugin* plugin, plugins_)
    {
        if (settings.value(QStringLiteral("PluginSettings/") + XmlUtils::removeWhiteSpace(plugin->getName()) + 
            QStringLiteral("/Active"), true).toBool())
        {
            activePlugins.append(plugin);
        }
    }

    return activePlugins;
}

//-----------------------------------------------------------------------------
// Function: PluginManager::setPluginPaths()
//-----------------------------------------------------------------------------
void PluginManager::setPluginPaths(QStringList const& pluginPaths)
{
    plugins_.clear();
    plugins_ = findPluginsInPaths(pluginPaths);
}

//-----------------------------------------------------------------------------
// Function: PluginManager::findPluginsInPaths()
//-----------------------------------------------------------------------------
QList<IPlugin*> PluginManager::findPluginsInPaths(QStringList const& pluginPaths)
{
    QList<IPlugin*> plugins;

    QSettings settings;
    settings.beginGroup(QStringLiteral("PluginSettings"));

    foreach (QString const& dirName, pluginPaths)
    {
        QDir pluginDirectory(dirName);
        if (pluginDirectory.isRelative())
        {
            pluginDirectory.setPath(QCoreApplication::applicationDirPath() + QLatin1Char('/') + dirName);
        }        

        foreach (QFileInfo const& fileInfo, pluginDirectory.entryInfoList(QDir::Files))
        {
            QPluginLoader loader(fileInfo.absoluteFilePath());
            IPlugin* plugin = qobject_cast<IPlugin*>(loader.instance());

            if (plugin != 0 && isUnique(plugin, plugins))
            {
                plugins.append(plugin);

                settings.beginGroup(XmlUtils::removeWhiteSpace(plugin->getName()));
                if (plugin->getSettingsModel())
                {
                    plugin->getSettingsModel()->loadSettings(settings);
                }
                settings.endGroup();
            }
        }
    }

    settings.endGroup();

    return plugins;
}

//-----------------------------------------------------------------------------
// Function: PluginManager::isUnique()
//-----------------------------------------------------------------------------
bool PluginManager::isUnique(IPlugin* plugin, QList<IPlugin*> const& plugins)
{
    foreach(IPlugin* knownPlugin, plugins)
    {
        if (QString::compare(knownPlugin->getName(), plugin->getName()) == 0 && 
            QString::compare(knownPlugin->getVersion(), plugin->getVersion()) == 0)
        {
            return false;            
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: PluginManager::getInstance()
//-----------------------------------------------------------------------------
PluginManager& PluginManager::getInstance()
{
    static PluginManager instance;
    return instance;
}

//-----------------------------------------------------------------------------
// Function: PluginManager::PluginManager()
//-----------------------------------------------------------------------------
PluginManager::PluginManager(): plugins_()
{
    
}
