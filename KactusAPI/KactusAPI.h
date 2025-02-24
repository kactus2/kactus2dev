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

#ifndef KACTUSAPI_H
#define KACTUSAPI_H

#include "KactusAPIGlobal.h"

#include <KactusAPI/include/LibraryHandler.h>

#include <KactusAPI/include/ConsoleMediator.h>


class IPlugin;
class IGeneratorPlugin;

class MessageMediator;


//-----------------------------------------------------------------------------
//! Defines the operations available for accessing Kactus2 core.
//-----------------------------------------------------------------------------
class KACTUS2_API KactusAPI
{
public:

    //! The constructor.
    KactusAPI(MessageMediator* messageChannel = nullptr);
    
    //! The destructor.
    ~KactusAPI() = default;    

    //! No copying.
    KactusAPI(const KactusAPI &other) = delete;

    //! No assignment.
    KactusAPI &operator=(const KactusAPI &other) = delete;

    /*!
     * Get the active IP-XACT library manager in the core.
     *
     *    @return The active IP-XACT library manager.
     */
    static LibraryInterface* getLibrary();

    /*!
     * Get the channel for output and error messages.
     *
     *    @return The channel for messages.
     */
    static MessageMediator* getMessageChannel();

    /*!
     * Get the Kactus2 version.
     *
     *    @return The current Kactus2 version.
     */
    static QString getVersion();

    /*!
     *  Get the version number.
     *
     *    @return The current version number.
     */
    static QString getVersionFileString();

    /*!
     * Gets the active paths where IP-XACT files are stored.
     *
     *    @return The active paths where IP-XACT files are stored.
     */
    static QStringList getActiveLibraryPaths();

    /*!
     * Gets all paths where IP-XACT files are stored.
     *
     *    @return All of the paths where IP-XACT files are stored.
     */
    static QStringList getAllLibraryPaths();

    /*!
     * Set the paths where IP-XACT files are stored. The first active path is set as default, if there are changes.
     *
     *    @param [in] activeLocations  The locations that are currently available to store the files.
     *    @param [in] allLocations     All the locations available, but not all need to be active.
     */
    static void setLibraryPaths(QStringList const& activeLocations,
        QStringList const& allLocations = QStringList() );

    /*!
     *	Set a given library path as active or inactive. If set as active, also add the path if not found.
     *  
     *    @param [in] path         The path to set active.
     *    @param [in] isActive     Indicates if the path should be set active or inactive.
     */
    static void setLibraryPathActive(QString const& path, bool isActive);

    /*!
     *	Add a new library path and set it active or inactive.
     *  
     *    @param [in] path         The path to add.
     *    @param [in] isActive     Flag to indicate if path should be set active or inactive.
     */
    static void addLibraryPath(QString const& path, bool isActive);

    /*!
     *	Removes selected library path, if selected path is not the default path. If path to remove is not found
     *  in settings, nothing is removed.
     *  
     *    @param [in] path     The path to remove
     *	    
     *    @return True, if path removal was successful (or nothing was done). False, if the path to remove is the default path.
     */
    static bool removeLibraryPath(QString const& path);

    /*!
     * Get the default library path for IP-XACT files.
     *
     *    @return The default path for IP-XACT files.
     */
    static QString getDefaultLibraryPath();

    /*!
     * Set the default library path for IP-XACT files.
     *
     *    @param [in] path  The default path to set for IP-XACT files.
     */
    static void setDefaultLibraryPath(QString const& path);

    /*!
     * Import a source file (RTL) into the library as a component.
     *
     *    @param [in] filePath     The path to the file to import.
     *    @param [in] targetVLNV   The VLNV to use for the imported component.
     *    @param [in] overwrite    Overwrite the component in the library, if it already exists.
     *
     *    @return The number of imported IP-XACT documents.
     */
    static int importFile(QString const& filePath, VLNV const& targetVLNV, bool overwrite);

    /*!
     * Run a generation plugin for a component.
     *
     *    @param [in] plugin              The plugin to run.
     *    @param [in] componentVLNV       The component to run the generation for.
     *    @param [in] viewName            The component view to run the generation for.
     *    @param [in] outputDirectory     The target directory for generated output files.
     *    @param [in] implementation      The implementation type for which the generator is run.
     *    @param [in] parentWidget        The parent widget for dialogs if GUI is used.
     */
    static void runGenerator(IGeneratorPlugin* plugin, VLNV const& componentVLNV, QString const& viewName,
        QString const& outputDirectory, KactusAttribute::Implementation implementation, QWidget* parentWidget = nullptr);

    /*!
     * Get the list of available plugins.     
     *
     *    @return The list of available plugins.
     */
     static QList<IPlugin*> getPlugins();

     /*!
      *  Get the file path of the selected VLNV document.
      *
      *    @param [in] vlnv   VLNV of the selected document.
      *
      *    @return File path of the selected VLNV document.
      */
     static QString getDocumentFilePath(VLNV const& vlnv);

private:

    // All members are defined static to enforce coherent state of the API regardless of instances.

    //! The active library manager in the core.
    inline static LibraryInterface* library_{ &LibraryHandler::getInstance() };

    //! The active message channel for output and errors.
    inline static ConsoleMediator defaultChannel_{ };

    //! The active message channel for output and errors.
    inline static MessageMediator* messageChannel_{ &defaultChannel_ };

};


#endif // KACTUSAPI_H
