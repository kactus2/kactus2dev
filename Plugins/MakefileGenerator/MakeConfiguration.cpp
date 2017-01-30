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
MakeConfiguration::MakeConfiguration(SWStackParser* parser) :
    fileOutput_(new FileOuput)
{
    // Must have the parser for the data.
    if (!parser)
    {
        return;
    }

    // Clear the old ones.
    fileOutput_->getFiles()->clear();

    // Append to the master make file to the list, no VLNV is directly associated with it.
    QSharedPointer<GenerationFile> masterFile(new GenerationFile);
    masterFile->fileName_ = *parser->masterName_;
    fileOutput_->getFiles()->append(masterFile);

    foreach (QSharedPointer<MakeFileData> mfd, *parser->getParsedData())
    {
        // Append reference to the each proposed name of a makefile.
        QSharedPointer<GenerationFile> file(new GenerationFile);
        file->fileName_ = mfd->makeName;
        fileOutput_->getFiles()->append(file);

        // Pick the VLNV from the first stack component if exists.
        if (mfd->parts.size() > 0)
        {
            file->vlnv_ = mfd->parts.first()->component->getVlnv().toString();
        }
        else
        {
            file->vlnv_ = ":::";
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