//-----------------------------------------------------------------------------
// File: FileSetInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 01.09.2020
//
// Description:
// Interface for editing file sets.
//-----------------------------------------------------------------------------

#include "FileSetInterface.h"

#include <IPXACTmodels/Component/FileSet.h>
#include <IPXACTmodels/Component/validators/FileSetValidator.h>

#include <FileBuilderInterface.h>
#include <FileInterface.h>

namespace
{
    std::string const FILESET_TYPE = "fileSet";
};

//-----------------------------------------------------------------------------
// Function: FileSetInterface::FileSetInterface()
//-----------------------------------------------------------------------------
FileSetInterface::FileSetInterface(QSharedPointer<FileSetValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter,
    FileInterface* fileInterface, FileBuilderInterface* fileBuilderInterface):
ParameterizableInterface(expressionParser, expressionFormatter),
NameGroupInterface(),
fileSets_(),
validator_(validator),
fileInterface_(fileInterface),
fileBuilderInterface_(fileBuilderInterface)
{

}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::setFileSets()
//-----------------------------------------------------------------------------
void FileSetInterface::setFileSets(QSharedPointer<QList<QSharedPointer<FileSet>>> newFileSets)
{
    fileSets_ = newFileSets;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getItemIndex()
//-----------------------------------------------------------------------------
int FileSetInterface::getItemIndex(std::string const& itemName) const
{
    if (fileSets_)
    {
        for (int i = 0; i < fileSets_->size(); ++i)
        {
            if (fileSets_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string FileSetInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string fileSetName = "";
    if (fileSets_ && (itemIndex >= 0 && itemIndex < fileSets_->size()))
    {
        fileSetName = fileSets_->at(itemIndex)->name().toStdString();
    }

    return fileSetName;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::itemCount()
//-----------------------------------------------------------------------------
int FileSetInterface::itemCount() const
{
    if (fileSets_)
    {
        return fileSets_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> FileSetInterface::getItemNames() const
{
    std::vector<std::string> names;
    if (fileSets_)
    {
        for (auto fileSet : *fileSets_)
        {
            names.push_back(fileSet->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::setName()
//-----------------------------------------------------------------------------
bool FileSetInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<FileSet> editedFileSet = getFileSet(currentName);
    if (editedFileSet && nameHasChanged(newName, currentName))
    {
        auto uniqueNewName = getUniqueName(newName, FILESET_TYPE);

        editedFileSet->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getDescription()
//-----------------------------------------------------------------------------
std::string FileSetInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<FileSet> editedFileSet = getFileSet(itemName);
    if (editedFileSet)
    {
        return editedFileSet->description().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::setDescription()
//-----------------------------------------------------------------------------
bool FileSetInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<FileSet> editedFileSet = getFileSet(itemName);
    if (!editedFileSet)
    {
        return false;
    }

    editedFileSet->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::validateItems()
//-----------------------------------------------------------------------------
bool FileSetInterface::validateItems() const
{
    for (auto fileSet : *fileSets_)
    {
        if (!validator_->validate(fileSet))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool FileSetInterface::itemHasValidName(std::string const& itemName) const
{
    QSharedPointer<FileSet> fileSet = getFileSet(itemName);
    if (fileSet)
    {
        return validator_->hasValidName(fileSet->name());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getGroups()
//-----------------------------------------------------------------------------
std::vector<std::string> FileSetInterface::getGroups(std::string const& fileSetName) const
{
    std::vector<std::string> groups;

    QSharedPointer<FileSet> selectedFileSet = getFileSet(fileSetName);
    if (selectedFileSet)
    {
        for (auto fileSetGroup : *selectedFileSet->getGroups())
        {
            groups.push_back(fileSetGroup.toStdString());
        }
    }

    return groups;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::setGroups()
//-----------------------------------------------------------------------------
bool FileSetInterface::setGroups(std::string const& fileSetName, std::vector<std::string> const& newGroups)
{
    QSharedPointer<FileSet> editedFileSet = getFileSet(fileSetName);
    if (!editedFileSet)
    {
        return false;
    }

    editedFileSet->getGroups()->clear();

    QStringList newFileSetGroups;
    for (auto singleGroup : newGroups)
    {
        newFileSetGroups.append(QString::fromStdString(singleGroup));
    }

    editedFileSet->getGroups()->append(newFileSetGroups);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getDependencies()
//-----------------------------------------------------------------------------
std::vector<std::string> FileSetInterface::getDependencies(std::string const& fileSetName) const
{
    std::vector<std::string> dependencies;

    QSharedPointer<FileSet> selectedFileSet = getFileSet(fileSetName);
    if (selectedFileSet)
    {
        for (auto fileSetDependency : *selectedFileSet->getDependencies())
        {
            dependencies.push_back(fileSetDependency.toStdString());
        }
    }

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::setDependencies()
//-----------------------------------------------------------------------------
bool FileSetInterface::setDependencies(std::string const& fileSetName,
    std::vector<std::string> const& newDependencies)
{
    QSharedPointer<FileSet> editedFileSet = getFileSet(fileSetName);
    if (!editedFileSet)
    {
        return false;
    }

    editedFileSet->getDependencies()->clear();

    QStringList newFileSetDependencies;
    for (auto singleDependency : newDependencies)
    {
        newFileSetDependencies.append(QString::fromStdString(singleDependency));
    }

    editedFileSet->getDependencies()->append(newFileSetDependencies);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::addFileSet()
//-----------------------------------------------------------------------------
void FileSetInterface::addFileSet(int const& row, std::string const& newFileSetName)
{
    auto fileSetName = getUniqueName(newFileSetName, FILESET_TYPE);

    QSharedPointer<FileSet> newFileSet(new FileSet());
    newFileSet->setName(fileSetName);

    fileSets_->insert(row, newFileSet);
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::removeFileSet()
//-----------------------------------------------------------------------------
bool FileSetInterface::removeFileSet(std::string const& fileName)
{
    QSharedPointer<FileSet> removedFileSet = getFileSet(fileName);
    if (!removedFileSet)
    {
        return false;
    }

    return fileSets_->removeOne(removedFileSet);
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getFileSet()
//-----------------------------------------------------------------------------
QSharedPointer<FileSet> FileSetInterface::getFileSet(std::string const& fileSetName) const
{
    if (fileSets_)
    {
        for (auto fileSet : *fileSets_)
        {
            if (fileSet->name().toStdString() == fileSetName)
            {
                return fileSet;
            }
        }
    }

    return QSharedPointer<FileSet>();
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getFileInterface()
//-----------------------------------------------------------------------------
FileInterface* FileSetInterface::getFileInterface() const
{
    return fileInterface_;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getFileBuilderInterface()
//-----------------------------------------------------------------------------
FileBuilderInterface* FileSetInterface::getFileBuilderInterface() const
{
    return fileBuilderInterface_;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int FileSetInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<FileSet> selectedFileSet = getFileSet(itemName);
    if (selectedFileSet)
    {
        fileBuilderInterface_->setFileBuilders(selectedFileSet->getDefaultFileBuilders());
        for (auto builder : *selectedFileSet->getDefaultFileBuilders())
        {
            totalReferencesToParameter +=
                fileBuilderInterface_->getAllReferencesToIdInItem(builder->getFileType().toStdString(), valueID);
        }
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::getExpressionsInSelectedFileSets()
//-----------------------------------------------------------------------------
std::vector<std::string> FileSetInterface::getExpressionsInSelectedFileSets(std::vector<std::string> fileSetNames)
const
{
    std::vector<std::string> expressionList;

    for (auto name : fileSetNames)
    {
        QSharedPointer<FileSet> currentFileSet = getFileSet(name);

        if (currentFileSet)
        {
            fileBuilderInterface_->setFileBuilders(currentFileSet->getDefaultFileBuilders());
            std::vector<std::string> fileBuilderExpressions =
                fileBuilderInterface_->getExpressionsInSelectedFileBuilders(fileBuilderInterface_->getItemNames());

            for (auto expression : fileBuilderExpressions)
            {
                expressionList.push_back(expression);
            }

            fileInterface_->setFiles(currentFileSet->getFiles());
            std::vector<std::string> fileExpressions =
                fileInterface_->getExpressionsInSelectedFiles(fileInterface_->getItemNames());

            for (auto expression : fileExpressions)
            {
                expressionList.push_back(expression);
            }
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: FileSetInterface::fileSetExists()
//-----------------------------------------------------------------------------
bool FileSetInterface::fileSetExists(std::string const& fileSetName) const
{
    QString fileSetNameQ = QString::fromStdString(fileSetName);
    foreach(QSharedPointer<FileSet> fileSet, *fileSets_)
    {
        if (fileSet->name() == fileSetNameQ)
        {
            return true;
        }
    }
    return false;
}
