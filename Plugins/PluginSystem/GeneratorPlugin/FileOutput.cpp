//-----------------------------------------------------------------------------
// File: FileOutput.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Container class for file output of generation.
//-----------------------------------------------------------------------------

#include "FileOutput.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

//-----------------------------------------------------------------------------
// Function: FileOuput::FileOuput()
//-----------------------------------------------------------------------------
FileOuput::FileOuput() : outputPath_(), fileNames_(new QList<QString*>)
{
}

//-----------------------------------------------------------------------------
// Function: FileOuput::~FileOuput()
//-----------------------------------------------------------------------------
FileOuput::~FileOuput()
{
}

//-----------------------------------------------------------------------------
// Function: FileOuput::getFileNames()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QString*> > FileOuput::getFileNames()
{
    return fileNames_;
}

//-----------------------------------------------------------------------------
// Function: FileOuput::setOutputPath()
//-----------------------------------------------------------------------------
void FileOuput::setOutputPath(QString const& path)
{
    outputPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: FileOuput::getOutputPath()
//-----------------------------------------------------------------------------
QString FileOuput::getOutputPath() const
{
    return outputPath_;
}

//-----------------------------------------------------------------------------
// Function: FileOuput::setOutputFileName()
//-----------------------------------------------------------------------------
void FileOuput::setOutputFileName(QString newName, int index)
{
    if (index < 0 || index >= fileNames_->size())
    {
        return;
    }

    QString* modpath = fileNames_->at(index);
    *modpath = newName;
}