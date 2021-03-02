//-----------------------------------------------------------------------------
// File: KactusAPI.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 25.02.2021
//
// Description:
// Defines the operations available for accessing Kactus2 core.
//-----------------------------------------------------------------------------

#include "Global.h"

#include <library/LibraryInterface.h>

#include <common/ui/MessageMediator.h>

#include <Plugins/PluginSystem/IPlugin.h>

#ifndef KACTUSAPI_H
#define KACTUSAPI_H

//-----------------------------------------------------------------------------
//! Defines the operations available for accessing Kactus2 core.
//-----------------------------------------------------------------------------
class KACTUS2_API KactusAPI
{
public:

    //! The constructor.
    KactusAPI(LibraryInterface* library = nullptr, MessageMediator* messageChannel = nullptr);
    
    //! The destructor.
    ~KactusAPI() = default;    

    //! No copying.
    KactusAPI(const KactusAPI &other) = delete;

    //! No assignment.
    KactusAPI &operator=(const KactusAPI &other) = delete;

    /*!
     * Get the active IP-XACT library manager in the core.
     *
     *     @return The active IP-XACT library manager.
     */
    static LibraryInterface* getLibrary();

    /*!
     * Get the channel for output and error messages.
     *
     *     @return The channel for messages.
     */
    static MessageMediator* getMessageChannel();

    /*!
     * Get the Kactus2 version.
     *
     *     @return The current Kactus2 version.
     */
    static QString getVersion();

    //static std::vector<std::string> listPlugins();

    static QStringList getLibraryPaths();

    static void setLibraryPaths(QStringList const&activeLocations,
        QStringList const& allLocations = QStringList() );

    static QString getDefaultLibraryPath();

    static void setDefaultLibraryPath(QString const& path);

private:

    // All members are defined static to enforce coherent state of the API regardless of instances.

    //! The active library manager in the core.
    static LibraryInterface* library_;

    //! The active message channel for output and errors.
    static MessageMediator* messageChannel_;
};


#endif // KACTUSAPI_H
