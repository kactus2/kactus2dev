//-----------------------------------------------------------------------------
// File: ConfigurationManager.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 21.04.2023
//
// Description:
// Manager for storing and restoring editor configurations.
//-----------------------------------------------------------------------------

#include "ConfigurationManager.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/FileSet.h>

namespace
{
    //! Identifier for configuration file set name.
    const QString CONFIGURATIONFILESETNAME = "configurations";

    //! File type for JSON files.
    const QString JSONFILETYPE = "json";

};

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::ConfigurationManager()
//-----------------------------------------------------------------------------
ConfigurationManager::ConfigurationManager(LibraryInterface* library) :
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getConfigurationDocument()
//-----------------------------------------------------------------------------
QJsonDocument ConfigurationManager::getConfigurationDocument(QSharedPointer<Component> component, QString const& fileTypeName)
{
    QSharedPointer<FileSet> configurationFileSet = getConfigurationFileSet(component);
    if (configurationFileSet)
    {
        QSharedPointer<File> configurationFileItem = getConfigurationFile(configurationFileSet, fileTypeName);
        if (configurationFileItem)
        {
            QString componentPath = library_->getDirectoryPath(component->getVlnv()) + "/";
            QString configurationFilePath = General::getAbsolutePath(componentPath, configurationFileItem->name());

            QFile configurationFile(configurationFilePath);

            if (!configurationFile.open(QFile::ReadOnly))
            {
                printError("Could not open configuration file " + configurationFilePath);
            }
            else
            {
                QJsonDocument configurationDocument = QJsonDocument::fromJson(configurationFile.readAll());
                return configurationDocument;
            }
        }
    }

    return QJsonDocument();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getPathToConfigurationFile()
//-----------------------------------------------------------------------------
QString ConfigurationManager::getPathToConfigurationFile(QSharedPointer<Component> component, QString const& fileExtension)
{
    QSharedPointer<File> configurationFileItem = getOrCreateConfigurationFile(component, fileExtension);
    QString componentPath = library_->getDirectoryPath(component->getVlnv()) + "/";
    QString configurationFilePath = General::getAbsolutePath(componentPath, configurationFileItem->name());

    return configurationFilePath;
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getOrCreateConfigurationFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> ConfigurationManager::getOrCreateConfigurationFile(QSharedPointer<Component> containingComponent, QString const& fileTypeName)
{
    QSharedPointer<FileSet> configurationFileSet = getOrCreateConfigurationFileSet(containingComponent);
    QSharedPointer<File> configurationFileItem = getConfigurationFile(configurationFileSet, fileTypeName);
    if (!configurationFileItem)
    {
        QString configurationFileName = containingComponent->getVlnv().getName() + "." + containingComponent->getVlnv().getVersion() + "_" + fileTypeName + "." + JSONFILETYPE;

        configurationFileItem = QSharedPointer<File>(new File(configurationFileName));
        configurationFileItem->getFileTypes()->append(FileType(JSONFILETYPE));

        configurationFileSet->getFiles()->append(configurationFileItem);
    }

    return configurationFileItem;
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getOrCreateConfigurationFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> ConfigurationManager::getOrCreateConfigurationFileSet(QSharedPointer<Component> containingComponent)
{
    QSharedPointer<FileSet> configurationFileSet = getConfigurationFileSet(containingComponent);
    if (!configurationFileSet)
    {
        configurationFileSet = QSharedPointer<FileSet>(new FileSet(CONFIGURATIONFILESETNAME));
        containingComponent->getFileSets()->append(configurationFileSet);
    }

    return configurationFileSet;
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getConfigurationFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> ConfigurationManager::getConfigurationFileSet(QSharedPointer<Component> targetComponent)
{
    for (auto fileSet : *targetComponent->getFileSets())
    {
        if (fileSet->name() == CONFIGURATIONFILESETNAME)
        {
            return fileSet;
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getConfigurationFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> ConfigurationManager::getConfigurationFile(QSharedPointer<FileSet> configurationFileSet, QString const& fileType)
{
    if (configurationFileSet)
    {
        for (auto containedFile : *configurationFileSet->getFiles())
        {
            if (containedFile->name().contains(fileType))
            {
                return containedFile;
            }
        }
    }

    return QSharedPointer<File>();
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::getLibrary()
//-----------------------------------------------------------------------------
LibraryInterface* ConfigurationManager::getLibrary() const
{
    return library_;
}

//-----------------------------------------------------------------------------
// Function: ConfigurationManager::writeModelToFile()
//-----------------------------------------------------------------------------
void ConfigurationManager::writeModelToFile(QSharedPointer<Component> component)
{
    library_->writeModelToFile(component);
}
