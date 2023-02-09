//-----------------------------------------------------------------------------
// File: FileBuilderInterface.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 03.09.2020
//
// Description:
// Interface for editing file builders.
//-----------------------------------------------------------------------------

#include "FileBuilderInterface.h"

#include <IPXACTmodels/common/FileBuilder.h>

namespace
{
    std::string const DEFAULT_TYPE = "unknown";
    QString const USERFILETYPE = "user";
};

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::FileBuilderInterface()
//-----------------------------------------------------------------------------
FileBuilderInterface::FileBuilderInterface(QSharedPointer<ExpressionParser> expressionParser,
    QSharedPointer<ExpressionFormatter> expressionFormatter):
ParameterizableInterface(expressionParser, expressionFormatter),
CommonInterface(),
fileBuilders_()
{

}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::setFileBuilders()
//-----------------------------------------------------------------------------
void FileBuilderInterface::setFileBuilders(QSharedPointer<QList<QSharedPointer<FileBuilder>>> newFileBuilders)
{
    fileBuilders_ = newFileBuilders;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::itemCount()
//-----------------------------------------------------------------------------
int FileBuilderInterface::itemCount() const
{
    if (fileBuilders_)
    {
        return fileBuilders_->count();
    }

    return 0;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::validateItems()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::validateItems() const
{
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getItemNames()
//-----------------------------------------------------------------------------
std::vector<std::string> FileBuilderInterface::getItemNames() const
{
    std::vector<std::string> names;
    if (fileBuilders_)
    {
        for (auto builder : *fileBuilders_)
        {
            QString builderID = builder->getFileType();

            if (builderID == USERFILETYPE)
            {
                builderID = builder->getUserFileType();
            }
            names.push_back(builderID.toStdString());
        }
    }

    return names;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getAllReferencesToIdInItem()
//-----------------------------------------------------------------------------
int FileBuilderInterface::getAllReferencesToIdInItem(const std::string& itemName, std::string const& valueID) const
{
    int totalReferencesToParameter = 0;

    QSharedPointer<FileBuilder> selectedFileBuilder = getFileBuilder(itemName);
    if (selectedFileBuilder)
    {
        QString idString = QString::fromStdString(valueID);

        int referencesInReplaceDefaultFlags(selectedFileBuilder->getReplaceDefaultFlags().count(idString));

        totalReferencesToParameter = referencesInReplaceDefaultFlags;
    }

    return totalReferencesToParameter;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getExpressionsInSelectedFileBuilders()
//-----------------------------------------------------------------------------
std::vector<std::string> FileBuilderInterface::getExpressionsInSelectedFileBuilders(
    std::vector<std::string> fileBuilderNames) const
{
    std::vector<std::string> expressionList;

    for (auto name : fileBuilderNames)
    {
        QSharedPointer<FileBuilder> currentFileBuilder = getFileBuilder(name);
        if (currentFileBuilder)
        {
            expressionList.push_back(currentFileBuilder->getReplaceDefaultFlags().toStdString());
        }
    }

    return expressionList;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::addFileBuilder()
//-----------------------------------------------------------------------------
void FileBuilderInterface::addFileBuilder(int const& row, std::string const& newFileBuilderName)
{
    QString uniqueFileBuilderName = getUniqueName(newFileBuilderName, DEFAULT_TYPE);

    QSharedPointer<FileBuilder> newFileBuilder(new FileBuilder());

    newFileBuilder->setFileType(uniqueFileBuilderName);

    fileBuilders_->insert(row, newFileBuilder);
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::removeFileBuilder()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::removeFileBuilder(std::string const& fileBuilderName)
{
    QSharedPointer<FileBuilder> removedFileBuilder = getFileBuilder(fileBuilderName);
    if (!removedFileBuilder)
    {
        return false;
    }

    return fileBuilders_->removeOne(removedFileBuilder);
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getFileBuilder()
//-----------------------------------------------------------------------------
QSharedPointer<FileBuilder> FileBuilderInterface::getFileBuilder(std::string const& fileBuilderName) const
{
    if (fileBuilders_)
    {
        for (auto builder : *fileBuilders_)
        {
            if ((builder->getFileType() == USERFILETYPE && builder->getUserFileType() ==
                QString::fromStdString(fileBuilderName)) ||
                builder->getFileType() == QString::fromStdString(fileBuilderName))
            {
                return builder;
            }
        }
    }

    return QSharedPointer<FileBuilder>();
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getIndexedFileType()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getIndexedFileType(int const& itemIndex)
{
    std::string fileType = "";

    if (fileBuilders_ && (itemIndex >= 0 && itemIndex < fileBuilders_->size()))
    {
        QSharedPointer<FileBuilder> builder = fileBuilders_->at(itemIndex);
        if (builder->getFileType() == USERFILETYPE)
        {
            fileType = builder->getUserFileType().toStdString();
        }
        else
        {
            fileType = builder->getFileType().toStdString();
        }
    }

    return fileType;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::setFileType()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::setFileType(std::string const& currentFileType, std::string const& newFileType)
{
    QSharedPointer<FileBuilder> editedFileBuilder = getFileBuilder(currentFileType);
    if (editedFileBuilder && nameHasChanged(newFileType, currentFileType))
    {
        QString uniqueNewFileType = getUniqueName(newFileType, DEFAULT_TYPE);

        editedFileBuilder->setFileType(uniqueNewFileType);
        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getCommand()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getCommand(std::string const& fileType) const
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(fileType);
    if (selectedBuilder)
    {
        return selectedBuilder->getCommand().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::setCommand()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::setCommand(std::string const& fileType, std::string const& newCommand)
{
    QSharedPointer<FileBuilder> editedFileBuilder = getFileBuilder(fileType);
    if (!editedFileBuilder)
    {
        return false;
    }

    editedFileBuilder->setCommand(QString::fromStdString(newCommand));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getFlags()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getFlags(std::string const& fileType) const
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(fileType);
    if (selectedBuilder)
    {
        return selectedBuilder->getFlags().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::setFlags()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::setFlags(std::string const& fileType, std::string const& newFlags)
{
    QSharedPointer<FileBuilder> editedFileBuilder = getFileBuilder(fileType);
    if (!editedFileBuilder)
    {
        return false;
    }

    editedFileBuilder->setFlags(QString::fromStdString(newFlags));
    return true;
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getReplaceDefaultFlagsValue()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getReplaceDefaultFlagsValue(std::string const& fileType, int const& baseNumber)
    const
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(fileType);
    if (selectedBuilder)
    {
        return parseExpressionToBaseNumber(selectedBuilder->getReplaceDefaultFlags(), baseNumber).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getReplaceDefaultFlagsFormattedExpression()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getReplaceDefaultFlagsFormattedExpression(std::string const& fileType) const
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(fileType);
    if (selectedBuilder)
    {
        return formattedValueFor(selectedBuilder->getReplaceDefaultFlags()).toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::getReplaceDefaultFlagsExpression()
//-----------------------------------------------------------------------------
std::string FileBuilderInterface::getReplaceDefaultFlagsExpression(std::string const& fileType) const
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(fileType);
    if (selectedBuilder)
    {
        return selectedBuilder->getReplaceDefaultFlags().toStdString();
    }

    return std::string("");
}

//-----------------------------------------------------------------------------
// Function: FileBuilderInterface::setReplaceDefaultFlags()
//-----------------------------------------------------------------------------
bool FileBuilderInterface::setReplaceDefaultFlags(std::string const& filetype,
    std::string const& newReplaceDefaultFlags)
{
    QSharedPointer<FileBuilder> selectedBuilder = getFileBuilder(filetype);
    if (!selectedBuilder)
    {
        return false;
    }

    selectedBuilder->setReplaceDefaultFlags(QString::fromStdString(newReplaceDefaultFlags));
    return true;
}
