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

//-----------------------------------------------------------------------------
//! Plugin manager which manages loaded plugins.
//-----------------------------------------------------------------------------
class PluginManager
{
public:
    /*!
     *  Constructor which loads the plugins from the given path.
     *
     *      @param [in] pluginPath The path containing the plugins.
     */
    PluginManager(QString const& pluginPath);

    /*!
     *  Destructor.
     */
    ~PluginManager();

    /*!
     *  Returns the list of loaded plugins.
     */
    QList<QObject*> const& getPlugins() const;
    
private:
    // Disable copying.
    PluginManager(PluginManager const& rhs);
    PluginManager& operator=(PluginManager const& rhs);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! The list of loaded plugins.
    QList<QObject*> plugins_;
};

//-----------------------------------------------------------------------------

#endif // PLUGINMANAGER_H
