//-----------------------------------------------------------------------------
// File: PluginManager.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Joni-Matti M‰‰tt‰
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

class IPlugin;

//-----------------------------------------------------------------------------
//! Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------
class PluginManager
{
public:

    /*!
     *  Destructor.
     */
    ~PluginManager();

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
     *      @param [in] pluginPaths   The paths containing the plugins.
     */
    void setPluginPaths(QStringList const& pluginPaths);
    
    /*!
     *  Find all plugins in the given paths.
     *
     *      @param [in] pluginPaths   Paths to directories to search for plugins.
     *
     *      @return All plugins found in the given directories.
     */
    static QList<IPlugin*> findPluginsInPaths(QStringList const& pluginPaths);

    /*!
     *  Get the singleton instance of the PluginManager.
     *
     *      @return The PluginManager instance.
     */
    static PluginManager& getInstance();

private:
    // Disable copying.
    PluginManager(PluginManager const& rhs);
    PluginManager& operator=(PluginManager const& rhs);
     
    //! The constructor. Private to enforce the use of getInstance() to get access.
    PluginManager();

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------
    
    //! The list of loaded plugins.
    QList<IPlugin*> plugins_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINMANAGER_H
