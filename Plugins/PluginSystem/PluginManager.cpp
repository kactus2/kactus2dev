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

#include <IPXACTmodels/XmlUtils.h>

#include <QApplication>
#include <QDir>


//-----------------------------------------------------------------------------
// Function: PluginManager::~PluginManager()
//-----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
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
        if (settings.value("PluginSettings/" + XmlUtils::removeWhiteSpace(plugin->getName()) + "/Active", true).toBool())
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
// Function: PluginManager::getPluginsInPaths()
//-----------------------------------------------------------------------------
QList<IPlugin*> PluginManager::findPluginsInPaths(QStringList const& pluginPaths)
{
    QList<IPlugin*> plugins;

    QSettings settings;
    settings.beginGroup("PluginSettings");

    foreach(QString dirName, pluginPaths)
    {
        if (QFileInfo(dirName).isRelative())
        {
            dirName = QApplication::applicationDirPath() + "/" + dirName;
        }        

        foreach (QFileInfo const& fileInfo, QDir(dirName).entryInfoList(QDir::Files))
        {
            QPluginLoader loader(fileInfo.absoluteFilePath());
            IPlugin* plugin = qobject_cast<IPlugin*>(loader.instance());

            if (plugin != 0)
            {
                // Check for duplicate plugins.
                bool uniquePlugin = true;
                foreach(IPlugin* knownPlugin, plugins)
                {
                    if (QString::compare(knownPlugin->getName(), plugin->getName()) == 0 && 
                        QString::compare(knownPlugin->getVersion(), plugin->getVersion()) == 0)
                    {
                        uniquePlugin = false;
                        break;
                    }
                }

                if (uniquePlugin)
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
    }

    settings.endGroup();

    return plugins;
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
