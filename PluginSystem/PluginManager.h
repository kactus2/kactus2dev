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
#include <QPluginLoader>

class IPlugin;

//-----------------------------------------------------------------------------
//! Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------
class PluginManager
{
public:
    /*!
     *  Constructor which loads the plugins from the given paths.
     *
     *      @param [in] pluginPaths The paths containing the plugins.
     */
    PluginManager(QStringList const& pluginPaths);

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
    QList<IPlugin*> const& getAllPlugins() const;

    /*!
     *  Loads the plugins from given pahts.
     *
     *      @param [in] pluginPaths   The paths containing the plugins.
     */
    void setPluginPaths(QStringList const& pluginPaths);
    
private:
    // Disable copying.
    PluginManager(PluginManager const& rhs);
    PluginManager& operator=(PluginManager const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of loaded plugins.
    QList<IPlugin*> plugins_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINMANAGER_H
