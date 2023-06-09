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

#include <FileInterface.h>

#include <IPXACTmodels/Component/File.h>
#include <IPXACTmodels/Component/validators/FileValidator.h>

namespace
{
    std::string const FILE_TYPE = "file";
};

//-----------------------------------------------------------------------------
// Function: FileInterface::FileInterface()
//-----------------------------------------------------------------------------
FileInterface::FileInterface(QSharedPointer<FileValidator> validator,
    QSharedPointer<ExpressionParser> expressionParser, QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
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
    if (editedFile && nameHasChanged(newName, currentName))
    {
        QString uniqueNewName = getUniqueName(newName, FILE_TYPE);

        editedFile->setName(uniqueNewName);
        return true;
    }
    else
    {
        return false;
    }
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
    for (auto file : *files_)
    {
        if (!validator_->validate(file))
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
    QSharedPointer<File> file = getFile(itemName);
    if (file)
    {
        return validator_->hasValidName(file->name());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int FileInterface::getAllReferencesToIdInItem(const std::string& fileName, std::string const& valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<BuildCommand> selectedBuildCommand = getBuildCommand(fileName);
    if (selectedBuildCommand)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInBuildCommandReplaceDefaultFlags =
            selectedBuildCommand->getReplaceDefaultFlags().count(idString);

        totalReferencesToParameter = referencesInBuildCommandReplaceDefaultFlags;
    }


    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getExpressionsInSelectedFiles()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getExpressionsInSelectedFiles(std::vector<std::string> fileNames) const
{
    std::vector<std::string> expressionList;

    for (auto name : fileNames)
    {
        QSharedPointer<BuildCommand> currentBuild = getBuildCommand(name);
        if (currentBuild)
        {
            expressionList.push_back(currentBuild->getReplaceDefaultFlags().toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addFile()
//-----------------------------------------------------------------------------
void FileInterface::addFile(int const& row, std::string const& newFileName)
{
    QString fileName = getUniqueName(newFileName, FILE_TYPE);

    QSharedPointer<File> newFile(new File());

    QSettings settings;
    newFile->setFileTypes(settings);
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
// Function: FileInterface::swapFiles()
//-----------------------------------------------------------------------------
void FileInterface::swapFiles(int const& firstIndex, int const& secondIndex)
{
    files_->swapItemsAt(firstIndex, secondIndex);
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

//-----------------------------------------------------------------------------
// Function: FileInterface::getLogicalName()
//-----------------------------------------------------------------------------
std::string FileInterface::getLogicalName(std::string const& fileName) const
{
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        return selectedFile->getLogicalName().toStdString();
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setLogicalName()
//-----------------------------------------------------------------------------
bool FileInterface::setLogicalName(std::string const& fileName, std::string newLogicalName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setLogicalName(QString::fromStdString(newLogicalName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::isLogicalNameDefault()
//-----------------------------------------------------------------------------
bool FileInterface::isLogicalNameDefault(std::string const& fileName) const
{
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        return selectedFile->isLogicalNameDefault();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setLogicalNameDefault()
//-----------------------------------------------------------------------------
bool FileInterface::setLogicalNameDefault(std::string const& fileName, bool newLogicalDefault)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setLogicalNameDefault(newLogicalDefault);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::isStructural()
//-----------------------------------------------------------------------------
bool FileInterface::isStructural(std::string const& fileName) const
{
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        return selectedFile->isStructural();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setLogicalNameDefault()
//-----------------------------------------------------------------------------
bool FileInterface::setStructural(std::string const& fileName, bool structuralStatus)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setStructural(structuralStatus);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::isIncludeFile()
//-----------------------------------------------------------------------------
bool FileInterface::isIncludeFile(std::string const& fileName) const
{
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        return selectedFile->isIncludeFile();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setIncludeFile()
//-----------------------------------------------------------------------------
bool FileInterface::setIncludeFile(std::string const& fileName, bool includeFileStatus)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setIncludeFile(includeFileStatus);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::hasExternalDeclarations()
//-----------------------------------------------------------------------------
bool FileInterface::hasExternalDeclarations(std::string const& fileName) const
{
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        return selectedFile->hasExternalDeclarations();
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setExternalDeclarations()
//-----------------------------------------------------------------------------
bool FileInterface::setExternalDeclarations(std::string const& fileName, bool externalDeclarationsStatus)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->setExternalDeclarations(externalDeclarationsStatus);
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::clearFileTypes()
//-----------------------------------------------------------------------------
void FileInterface::clearFileTypes(std::string const& fileName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        editedFile->clearFileTypes();
    }
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setFileTypes()
//-----------------------------------------------------------------------------
bool FileInterface::setFileTypes(std::string const& fileName, std::vector<std::string> const newFileTypes)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    clearFileTypes(fileName);
    addMultipleFileTypes(fileName, newFileTypes);

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addFileType()
//-----------------------------------------------------------------------------
bool FileInterface::addFileType(std::string const& fileName, std::string const newFileType)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->getFileTypes()->append(FileType(QString::fromStdString(newFileType)));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addMultipleFileTypes()
//-----------------------------------------------------------------------------
bool FileInterface::addMultipleFileTypes(std::string const& fileName, std::vector<std::string> const newFileTypes)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    for (auto const& fileType : newFileTypes)
    {
        editedFile->getFileTypes()->append(FileType(QString::fromStdString(fileType)));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getDependencies()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getDependencies(std::string const& fileName) const
{
    std::vector<std::string> dependencies;
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        for (auto singleDependency : *selectedFile->getDependencies())
        {
            dependencies.push_back(singleDependency.toStdString());
        }
    }

    return dependencies;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::clearDependencies()
//-----------------------------------------------------------------------------
void FileInterface::clearDependencies(std::string const& fileName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        editedFile->getDependencies()->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addDependency()
//-----------------------------------------------------------------------------
bool FileInterface::addDependency(std::string const& fileName, std::string const newDependency)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->getDependencies()->append(QString::fromStdString(newDependency));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addMultipleDependencies()
//-----------------------------------------------------------------------------
bool FileInterface::addMultipleDependencies(std::string const& fileName,
    std::vector<std::string> const newDependencies)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    for (auto dependency : newDependencies)
    {
        editedFile->getDependencies()->append(QString::fromStdString(dependency));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getExportedNames()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getExportedNames(std::string const& fileName) const
{
    std::vector<std::string> exportedNames;
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        for (auto singleName : *selectedFile->getExportedNames())
        {
            exportedNames.push_back(singleName.toStdString());
        }
    }

    return exportedNames;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::clearExportedNames()
//-----------------------------------------------------------------------------
void FileInterface::clearExportedNames(std::string const& fileName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        editedFile->getExportedNames()->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addExportedName()
//-----------------------------------------------------------------------------
bool FileInterface::addExportedName(std::string const& fileName, std::string const newExportedName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->getExportedNames()->append(QString::fromStdString(newExportedName));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addMultipleExportedNames()
//-----------------------------------------------------------------------------
bool FileInterface::addMultipleExportedNames(std::string const& fileName,
    std::vector<std::string> const newExportedNames)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    for (auto name : newExportedNames)
    {
        editedFile->getExportedNames()->append(QString::fromStdString(name));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getImageTypes()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getImageTypes(std::string const& fileName) const
{
    std::vector<std::string> imageTypes;
    QSharedPointer<File> selectedFile = getFile(fileName);
    if (selectedFile)
    {
        for (auto singleType : *selectedFile->getImageTypes())
        {
            imageTypes.push_back(singleType.toStdString());
        }
    }

    return imageTypes;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::clearImageTypes()
//-----------------------------------------------------------------------------
void FileInterface::clearImageTypes(std::string const& fileName)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        editedFile->getImageTypes()->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addImageType()
//-----------------------------------------------------------------------------
bool FileInterface::addImageType(std::string const& fileName, std::string const newImageType)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    editedFile->getImageTypes()->append(QString::fromStdString(newImageType));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::addMultipleImageTypes()
//-----------------------------------------------------------------------------
bool FileInterface::addMultipleImageTypes(std::string const& fileName,
    std::vector<std::string> const newImageTypes)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    for (auto imageType : newImageTypes)
    {
        editedFile->getImageTypes()->append(QString::fromStdString(imageType));
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getFileTypes()
//-----------------------------------------------------------------------------
std::vector<std::string> FileInterface::getFileTypes(std::string const& fileName) const
{
    std::vector<std::string> fileTypes;
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        for (auto const& type : *editedFile->getFileTypes())
        {
            fileTypes.push_back(type.type_.toStdString());
        }
    }

    return fileTypes;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandText()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandText(std::string const& fileName) const
{
    std::string buildCommandText = "";

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        buildCommandText = fileBuild->getCommand().toStdString();
    }

    return buildCommandText;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommand()
//-----------------------------------------------------------------------------
QSharedPointer<BuildCommand> FileInterface::getBuildCommand(std::string const& fileName) const
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (editedFile)
    {
        QSharedPointer<BuildCommand> fileBuild = editedFile->getBuildCommand();
        if (fileBuild)
        {
            return fileBuild;
        }
    }

    return QSharedPointer<BuildCommand>();
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setBuildCommand()
//-----------------------------------------------------------------------------
bool FileInterface::setBuildCommand(std::string const& fileName, std::string const& newBuildCommand)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        fileBuild->setCommand(QString::fromStdString(newBuildCommand));
    }
    else if (!newBuildCommand.empty())
    {
        QSharedPointer<BuildCommand> newFileBuild(new BuildCommand());
        newFileBuild->setCommand(QString::fromStdString(newBuildCommand));

        editedFile->setBuildcommand(newFileBuild);
    }

    if (newBuildCommand.empty())
    {
        removeEmptyBuildCommand(editedFile);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::removeEmptyBuildCommand()
//-----------------------------------------------------------------------------
void FileInterface::removeEmptyBuildCommand(QSharedPointer<File> containingFile) const
{
    QSharedPointer<BuildCommand> buildCommand = containingFile->getBuildCommand();
    if (buildCommand && buildCommand->getCommand().isEmpty() && buildCommand->getFlags().isEmpty() &&
        buildCommand->getReplaceDefaultFlags().isEmpty() && buildCommand->getTargetName().isEmpty())
    {
        containingFile->setBuildcommand(QSharedPointer<BuildCommand>());
    }
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setBuildCommandFlags()
//-----------------------------------------------------------------------------
bool FileInterface::setBuildCommandFlags(std::string const& fileName, std::string const& newFlags)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        fileBuild->setFlags(QString::fromStdString(newFlags));
    }
    else if (!newFlags.empty())
    {
        QSharedPointer<BuildCommand> newFileBuild(new BuildCommand());
        newFileBuild->setFlags(QString::fromStdString(newFlags));

        editedFile->setBuildcommand(newFileBuild);
    }

    if (newFlags.empty())
    {
        removeEmptyBuildCommand(editedFile);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandFlags()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandFlags(std::string const& fileName) const
{
    std::string buildCommandFlags = "";

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        buildCommandFlags = fileBuild->getFlags().toStdString();
    }

    return buildCommandFlags;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandReplaceDefaultFlagsValue()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandReplaceDefaultFlagsValue(std::string const& fileName,
    int const& baseNumber) const
{
    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        return parseExpressionToBaseNumber(fileBuild->getReplaceDefaultFlags(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandReplaceDefaultFlagsFormattedExpression()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandReplaceDefaultFlagsFormattedExpression(std::string const& fileName) const
{
    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        return formattedValueFor(fileBuild->getReplaceDefaultFlags()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandReplaceDefaultFlagsExpression()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandReplaceDefaultFlagsExpression(std::string const& fileName) const
{
    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        return fileBuild->getReplaceDefaultFlags().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setbuildCommandReplaceDefaultFlags()
//-----------------------------------------------------------------------------
bool FileInterface::setbuildCommandReplaceDefaultFlags(std::string const& fileName,
    std::string const& newReplaceDefaultFlags)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        fileBuild->setReplaceDefaultFlags(QString::fromStdString(newReplaceDefaultFlags));
    }
    else if (!newReplaceDefaultFlags.empty())
    {
        QSharedPointer<BuildCommand> newFileBuild(new BuildCommand());
        newFileBuild->setReplaceDefaultFlags(QString::fromStdString(newReplaceDefaultFlags));

        editedFile->setBuildcommand(newFileBuild);
    }

    if (newReplaceDefaultFlags.empty())
    {
        removeEmptyBuildCommand(editedFile);
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::getBuildCommandTarget()
//-----------------------------------------------------------------------------
std::string FileInterface::getBuildCommandTarget(std::string const& fileName) const
{
    std::string buildCommandTarget = "";

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        buildCommandTarget = fileBuild->getTargetName().toStdString();
    }

    return buildCommandTarget;
}

//-----------------------------------------------------------------------------
// Function: FileInterface::setBuildCommandTarget()
//-----------------------------------------------------------------------------
bool FileInterface::setBuildCommandTarget(std::string const& fileName, std::string const& newTarget)
{
    QSharedPointer<File> editedFile = getFile(fileName);
    if (!editedFile)
    {
        return false;
    }

    QSharedPointer<BuildCommand> fileBuild = getBuildCommand(fileName);
    if (fileBuild)
    {
        fileBuild->setTargetName(QString::fromStdString(newTarget));
    }
    else if (!newTarget.empty())
    {
        QSharedPointer<BuildCommand> newFileBuild(new BuildCommand());
        newFileBuild->setTargetName(QString::fromStdString(newTarget));

        editedFile->setBuildcommand(newFileBuild);
    }

    if (newTarget.empty())
    {
        removeEmptyBuildCommand(editedFile);
    }

    return true;
}
