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

class IPlugin;
class IGeneratorPlugin;

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

    /*!
     * Gets the paths where IP-XACT files are stored.
     *
     *     @return The paths where IP-XACT files are stored.
     */
     static QStringList getLibraryPaths();

    /*!
     * Set the paths where IP-XACT files are stored.
     *
     *     @param [in] activeLocations  The locations that are currently available to store the files.
     *     @param [in] allLocations     All the locations available, but not all need to be active.
     */
     static void setLibraryPaths(QStringList const&activeLocations,
        QStringList const& allLocations = QStringList() );

    /*!
     * Get the default library path for IP-XACT files.
     *
     *     @return The default path for IP-XACT files.
     */
    static QString getDefaultLibraryPath();

    /*!
     * Set the default library path for IP-XACT files.
     *
     *     @param [in] path  The default path to set for IP-XACT files.
     */
    static void setDefaultLibraryPath(QString const& path);

    /*!
     * Import a source file (RTL) into the library as a component.
     *
     *     @param [in] filePath     The path to the file to import.
     *     @param [in] targetVLNV   The VLNV to use for the imported component.
     *     @param [in] overwrite    Overwrite the component in the library, if it already exists.
     *
     *     @return The number of imported IP-XACT documents.
     */
    static int importFile(QString const& filePath, VLNV const& targetVLNV, bool overwrite);

    /*!
     * Run a generation plugin for a component.
     *
     *     @param [in] plugin           The plugin to run.
     *     @param [in] componentVLNV    The component to run the generation for.
     *     @param [in] viewName         The component view to run the generation for.
     *     @param [in] outputDirectory  The target directory for generated output files.
     *     @param [in] parentWidget     The parent widget for dialogs if GUI is used.
     */
     static void runGenerator(IGeneratorPlugin* plugin, VLNV const& componentVLNV, QString const& viewName,
        QString const& outputDirectory, QWidget* parentWidget = nullptr);

    /*!
     * Get the list of available plugins.     
     *
     *     @return The list of available plugins.
     */
     static QList<IPlugin*> getPlugins();

private:

    // All members are defined static to enforce coherent state of the API regardless of instances.

    //! The active library manager in the core.
    static LibraryInterface* library_;

    //! The active message channel for output and errors.
    static MessageMediator* messageChannel_;

};


#endif // KACTUSAPI_H
