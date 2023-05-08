//-----------------------------------------------------------------------------
// File: ConfigurationManager.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring editor configurations.
//-----------------------------------------------------------------------------

#ifndef CONFIGURATIONMANAGER_H
#define CONFIGURATIONMANAGER_H

class Component;
class LibraryInterface;
class FileSet;
class File;

#include <QFile>
#include <QJsonObject>
#include <QJsonDocument>
#include <QSharedPointer>

//-----------------------------------------------------------------------------
//! Manager for storing and restoring editor configurations.
//-----------------------------------------------------------------------------
class ConfigurationManager
{

public:
    
    /*!
     *  The constructor.
     * 
     *      @param [in] library     Interface for accessing the library.
     */
    ConfigurationManager(LibraryInterface* library);
    
    /*!
     * The destructor.
     */
    ~ConfigurationManager() = default;

    /*!
     *  Get the configuration document.
     *
     *      @param [in] component       The containing component.
     *      @param [in] fileTypeName    Type of the configuration file.
     *
     *      @return The configuration document.
     */
    QJsonDocument getConfigurationDocument(QSharedPointer<Component> component, QString const& fileTypeName);

protected:

    /*!
     *  Get the path to the selected configuration file.
     *
     *      @param [in] component       The top component.
     *      @param [in] fileExtension   File extension for the selected file.
     *
     *      @return The path to the selected configuration file.
     */
    QString getPathToConfigurationFile(QSharedPointer<Component> component, QString const& fileExtension);

    /*!
     *  Get an existing configuration file, or create a new one if one is not found.
     *
     *      @param [in] containingComponent     Component containing the selected configuration file.
     *      @param [in] fileTypeName            Type name for the configuration file.
     *
     *      @return The selected configuration file.
     */
    QSharedPointer<File> getOrCreateConfigurationFile(QSharedPointer<Component> containingComponent, QString const& fileTypeName);

    /*!
     *  Get the interface for accessing the library.
     *
     *      @return Interface for accessing the library.
     */
    LibraryInterface* getLibrary() const;

    /*!
     *  Write the selected component to the file system.
     *
     *      @param [in] component    The selected component.
     */
    void writeModelToFile(QSharedPointer<Component> component);

private:

    /*!
     *  Print the selected error.
     *
     *      @param [in] errorMessage    The selected error message.
     */
    virtual void printError(QString const& errorMessage) const = 0;

    /*!
     *  Get an existing configuration file set, or create a new one if one is not found.
     *
     *      @param [in] containingComponent     Component containing the configuration file set.
     *
     *      @return The configuration file set.
     */
    QSharedPointer<FileSet> getOrCreateConfigurationFileSet(QSharedPointer<Component> containingComponent);

    /*!
     *  Get the file set for configurations.
     *
     *      @param [in] topComponent    Top level component.
     *
     *      @return File set for configurations.
     */
    QSharedPointer<FileSet> getConfigurationFileSet(QSharedPointer<Component> targetComponent);

    /*!
     *  Get the configuration file for the selected file type..
     *
     *      @param [in] configurationFileSet    File set for configurations.
     *      @param [in] fileType                The selected file type.
     *
     *      @return Configuration file for the selected file type.
     */
    QSharedPointer<File> getConfigurationFile(QSharedPointer<FileSet> configurationFileSet, QString const& fileType);

    //-----------------------------------------------------------------------------
    // Data.
    //-----------------------------------------------------------------------------

    //! Interface for accessing the library.
    LibraryInterface* library_;
};

#endif // CONFIGURATIONMANAGER_H
