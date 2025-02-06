//-----------------------------------------------------------------------------
// File: OutputControl.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 15.11.2016
//
// Description:
// Container class for file output of generation.
//-----------------------------------------------------------------------------

#include "OutputControl.h"

#include <IPXACTmodels/Component/View.h>
#include <IPXACTmodels/Component/ComponentInstantiation.h>
#include <IPXACTmodels/Component/FileSet.h>

#include <QDir>

//-----------------------------------------------------------------------------
// Function: OutputControl::OutputControl()
//-----------------------------------------------------------------------------
OutputControl::OutputControl() : outputPath_(), outputs_(new QList<QSharedPointer<GenerationOutput> >)
{
}

//-----------------------------------------------------------------------------
// Function: OutputControl::~OutputControl()
//-----------------------------------------------------------------------------
OutputControl::~OutputControl()
{
}

//-----------------------------------------------------------------------------
// Function: OutputControl::validSelections()
//-----------------------------------------------------------------------------
bool OutputControl::validSelections(QString &warning)
{
    // Must have path for the files. 
    if (outputPath_.isEmpty() || !QDir(outputPath_).exists())
    {
        warning = QLatin1String("<b>The output directory must exist.</b>");
        return false;
    }

    // Must not have same file name more than once!
    for(int i = 0; i < outputs_->size(); ++i)
    {
        QSharedPointer<GenerationOutput> file = outputs_->at(i);

        for(int j = 0; j < outputs_->size(); ++j)
        {
            // Do not compare with itself
            if (i==j)
            {
                continue;
            }

            QString name2compare = outputs_->at(j)->fileName_;

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
// Function: OutputControl::setOutputPath()
//-----------------------------------------------------------------------------
void OutputControl::setOutputPath(QString const& path)
{
    outputPath_ = path;
}

//-----------------------------------------------------------------------------
// Function: OutputControl::getOutputPath()
//-----------------------------------------------------------------------------
QString OutputControl::getOutputPath() const
{
    return outputPath_;
}

//-----------------------------------------------------------------------------
// Function: OutputControl::getFileNames()
//-----------------------------------------------------------------------------
QSharedPointer<QList<QSharedPointer<GenerationOutput> > > OutputControl::getOutputs()
{
    return outputs_;
}

//-----------------------------------------------------------------------------
// Function: OutputControl::changeFileName()
//-----------------------------------------------------------------------------
QSharedPointer<GenerationOutput> OutputControl::changeFileName(int index, QString const& newName)
{
    if (index >= outputs_->size() || index < 0)
    {
        return QSharedPointer<GenerationOutput>();
    }

    QSharedPointer<GenerationOutput> selection = outputs_->at(index);

    if (selection->fileName_ == newName)
    {
        return QSharedPointer<GenerationOutput>();
    }

    selection->fileName_ = newName;
    selection->write(outputPath_);

    return selection;
}