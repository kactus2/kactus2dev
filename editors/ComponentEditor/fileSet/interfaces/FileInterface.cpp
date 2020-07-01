//-----------------------------------------------------------------------------
// File: FileInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 29.06.2020
//
// Description:
// Interface for editing files.
//-----------------------------------------------------------------------------

#include "FileInterface.h"

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>

namespace
{
//     QString const TRUE_STRING = QLatin1String("true");
//     QString const FALSE_STRING = QLatin1String("false");

    std::string const FILE_TYPE = "file";
};

//-----------------------------------------------------------------------------
// Function: FileInterface::FileInterface()
//-----------------------------------------------------------------------------
FileInterface::FileInterface(QSharedPointer<FileValidator> validator):
NameGroupInterface(),
files_(),
validator_(validator)
{

}

//-----------------------------------------------------------------------------
// Function: FileInterface::setFiles()
//-----------------------------------------------------------------------------
void FileInterface::setFiles(QSharedPointer<QList<QSharedPointer<File>>> newFiles)
{
    files_ = newFiles;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getItemIndex()
//-----------------------------------------------------------------------------
int FileInterface::getItemIndex(std::string const& itemName) const
{
    if (files_)
    {
        for (int i = 0; i < files_->size(); ++i)
        {
            if (files_->at(i)->name().toStdString() == itemName)
            {
                return i;
            }
        }
    }

    return -1;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getIndexedItemName()
//-----------------------------------------------------------------------------
std::string FileInterface::getIndexedItemName(int const& itemIndex) const
{
    std::string fileName = "";
    if (files_ && (itemIndex >= 0 && itemIndex < files_->size()))
    {
        fileName = files_->at(itemIndex)->name().toStdString();
    }

    return fileName;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::itemCount()
//-----------------------------------------------------------------------------
int FileInterface::itemCount() const
{
    if (files_)
    {
        return files_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getItemNames() const
{
    std::vector<std::string> names;
    if (files_)
    {
        for (auto file : *files_)
        {
            names.push_back(file->name().toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setName()
//-----------------------------------------------------------------------------
bool FileInterface::setName(std::string const& currentName, std::string const& newName)
{
    QSharedPointer<File> editedFile = getFile(currentName);
    if (!editedFile)
    {
        return false;
    }

    QString uniqueNewName = getUniqueName(newName, FILE_TYPE);

    editedFile->setName(uniqueNewName);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getDescription()
//-----------------------------------------------------------------------------
std::string FileInterface::getDescription(std::string const& itemName) const
{
    QSharedPointer<File> editedFile = getFile(itemName);
    if (editedFile)
    {
        return editedFile->getDescription().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setDescription()
//-----------------------------------------------------------------------------
bool FileInterface::setDescription(std::string const& itemName, std::string const& newDescription)
{
    QSharedPointer<File> editedFile = getFile(itemName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setDescription(QString::fromStdString(newDescription));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::validateItems()
//-----------------------------------------------------------------------------
bool FileInterface::validateItems() const
{
    for (auto field : *files_)
    {
        if (!validator_->validate(field))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::itemHasValidName()
//-----------------------------------------------------------------------------
bool FileInterface::itemHasValidName(std::string const& itemName) const
{
    return validator_->hasValidName(getFile(itemName)->name());
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addFile()
//-----------------------------------------------------------------------------
void FileInterface::addFile(int const& row, std::string const& newFileName)
{
    QString fileName = getUniqueName(newFileName, FILE_TYPE);

    QSharedPointer<File> newFile(new File());
    newFile->setName(fileName);

    files_->insert(row, newFile);
}

//-----------------------------------------------------------------------------
// Function: FileInterface::removeFile()
//-----------------------------------------------------------------------------
bool FileInterface::removeFile(std::string const& fileName)
{
    QSharedPointer<File> removedFile = getFile(fileName);
    if (!removedFile)
    {
        return false;
    }

    return files_->removeOne(removedFile);
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getFile()
//-----------------------------------------------------------------------------
QSharedPointer<File> FileInterface::getFile(std::string const& fileName) const
{
    if (files_)
    {
        for (auto file : *files_)
        {
            if (file->name().toStdString() == fileName)
            {
                return file;
            }
        }
    }

    return QSharedPointer<File>();
}
