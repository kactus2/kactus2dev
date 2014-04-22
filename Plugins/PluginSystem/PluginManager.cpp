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

#include <models/generaldeclarations.h>

#include <QApplication>
#include <QDir>

//-----------------------------------------------------------------------------
// Function: PluginManager::PluginManager()
//-----------------------------------------------------------------------------
PluginManager::PluginManager(QStringList const& pluginPaths)
{
    setPluginPaths(pluginPaths);
}

//-----------------------------------------------------------------------------
// Function: PluginManager::~PluginManager()
//-----------------------------------------------------------------------------
PluginManager::~PluginManager()
{
}

//-----------------------------------------------------------------------------
// Function: PluginManager::getPlugins()
//-----------------------------------------------------------------------------
QList<IPlugin*> const& PluginManager::getAllPlugins() const
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
        if (settings.value("PluginSettings/" + General::removeWhiteSpace(plugin->getName()) + "/Active", true).toBool())
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

    foreach(QString dirName, pluginPaths)
    {
        if (QFileInfo(dirName).isRelative())
        {
            dirName = QApplication::applicationDirPath() + "/" + dirName;
        }        

        QDir dir(dirName);

        foreach (QString const& filename, dir.entryList(QDir::Files))
        {
            QPluginLoader loader(dir.absoluteFilePath(filename));
            IPlugin* plugin = qobject_cast<IPlugin*>(loader.instance());

            if (plugin != 0)
            {
                // Check for duplicate plugins.
                bool uniquePlugin = true;
                foreach(IPlugin* knownPlugin, plugins_)
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
                    plugins_.append(plugin);
                }
            }
        }
    }
}