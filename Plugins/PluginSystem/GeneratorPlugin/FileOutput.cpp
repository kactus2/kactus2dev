//-----------------------------------------------------------------------------
// File: FileOutput.cpp
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

#include <QDir>

//-----------------------------------------------------------------------------
// Function: FileOuput::FileOuput()
//-----------------------------------------------------------------------------
FileOuput::FileOuput() : outputPath_(), files_(new QList<QSharedPointer<GenerationFile> >)
{
}

//-----------------------------------------------------------------------------
// Function: FileOuput::~FileOuput()
//-----------------------------------------------------------------------------
FileOuput::~FileOuput()
{
}

//-----------------------------------------------------------------------------
// Function: FileOuput::validSelections()
//-----------------------------------------------------------------------------
bool FileOuput::validSelections(QString &warning)
{
    // Must have path for the files. 
    if (outputPath_.isEmpty() || !QDir(outputPath_).exists())
    {
        warning = QLatin1String("<b>The output directory must exist.</b>");
        return false;
    }

    // Must not have same file name more than once!
    for(int i = 0; i < files_->size(); ++i)
    {
        QSharedPointer<GenerationFile> file = files_->at(i);

        for(int j = 0; j < files_->size(); ++j)
        {
            // Do not compare with itself
            if (i==j)
            {
                continue;
            }

            QString name2compare = files_->at(j)->fileName_;

            // Is the same -> fail.
            if (file->fileName_ == name2compare)
            {
                warning = QLatin1String("<b>File name</b> ") + file->fileName_ + QLatin1String(" <b>is listed more than once.</b>");
                return false;
            }
        }
    }

    return true;
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
// Function: FileOuput::getFileNames()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<GenerationFile> > > FileOuput::getFiles()
{
    return files_;
}