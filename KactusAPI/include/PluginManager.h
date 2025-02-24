//-----------------------------------------------------------------------------
// File: PluginManager.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti Maatta
// Date: 27.6.2012
//
// Description:
// Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------

#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QString>
#include <QList>
#include <QObject>

#include <KactusAPI/KactusAPIGlobal.h>

class IPlugin;

//-----------------------------------------------------------------------------
//! Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------
class KACTUS2_API PluginManager
{
public:

    //! The destructor.     
    ~PluginManager() = default;

    /*!
     *  Adds the given plugin to the available plugins.
     *
     *    @param [in] plugin   The plugin to add
     */
    void addPlugin(IPlugin* plugin);

    /*!
     *  returns the list of active plugins.
     */
    QList<IPlugin*> getActivePlugins() const;

    /*!
     *  Returns the list of all loaded plugins.
     */
    QList<IPlugin*> getAllPlugins() const;

    /*!
     *  Loads the plugins from given paths.
     *
     *    @param [in] pluginPaths   The paths containing the plugins.
     */
    void setPluginPaths(QStringList const& pluginPaths);
    
    /*!
     *  Find all plugins in the given paths.
     *
     *    @param [in] pluginPaths   Paths to directories to search for plugins.
     *
     *    @return All plugins found in the given directories.
     */
    static QList<IPlugin*> findPluginsInPaths(QStringList const& pluginPaths);
   
    /*!
     *  Get the singleton instance of the PluginManager.
     *
     *    @return The PluginManager instance.
     */
    static PluginManager& getInstance();

private:
    // Disable copying.
    PluginManager(PluginManager const& rhs);
    PluginManager& operator=(PluginManager const& rhs);
     
    //! The constructor. Private to enforce the use of getInstance() to get access.
    PluginManager();
    
    static bool isUnique(IPlugin* plugin, QList<IPlugin*> const& plugins);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The list of loaded plugins.
    QList<IPlugin*> plugins_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINMANAGER_H
