//-----------------------------------------------------------------------------
// File: MakeConfiguration.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 17.11.2016
//
// Description:
// Container class for make configuration.
//-----------------------------------------------------------------------------

#include "MakeConfiguration.h"

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::MakeConfiguration()
//-----------------------------------------------------------------------------
MakeConfiguration::MakeConfiguration(SWStackParser* parser) : parser_(parser),
    fileOutput_(new FileOuput), addLauncher_(false)
{
    if (!parser_)
    {
        return;
    }

    fileOutput_->getFileNames()->clear();

    QStringList vlnvs;

    foreach (QSharedPointer<MakeFileData> mfd, *parser_->getParsedData())
    {
        QString* fileName = &mfd->targetPath;
        fileOutput_->getFileNames()->append(fileName);

        if (mfd->parts.size() > 0)
        {
            vlnvs.append(mfd->parts.first()->component->getVlnv().toString());
        }
        else
        {
            vlnvs.append(":::");
        }
    }
}

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::~MakeConfiguration()
//-----------------------------------------------------------------------------
MakeConfiguration::~MakeConfiguration()
{
}

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::validSelections()
//-----------------------------------------------------------------------------
bool MakeConfiguration::validSelections(QString &warning)
{
    // Must have valid file output.
    if (!fileOutput_->validSelections(warning))
    {
        return false;
    }

   return true;
}

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::getFileOuput()
//-----------------------------------------------------------------------------
QSharedPointer<FileOuput> MakeConfiguration::getFileOuput() const
{
    return fileOutput_;
}

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::setAddLauncher()
//-----------------------------------------------------------------------------
void MakeConfiguration::setAddLauncher(bool value)
{
    addLauncher_ = value;
}

//-----------------------------------------------------------------------------
// Function: MakeConfiguration::getAddLauncher()
//-----------------------------------------------------------------------------
bool MakeConfiguration::getAddLauncher() const
{
    return addLauncher_;
}