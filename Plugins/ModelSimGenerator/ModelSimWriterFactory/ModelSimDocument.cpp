//-----------------------------------------------------------------------------
// File: ModelSimDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Storage for ModelSim writers.
//-----------------------------------------------------------------------------

#include "ModelSimDocument.h"

#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: ModelSimDocument::ModelSimDocument()
//-----------------------------------------------------------------------------
ModelSimDocument::ModelSimDocument()
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimDocument::write()
//-----------------------------------------------------------------------------
void ModelSimDocument::write(QString const& outputDirectory)
{
    fileContent_.clear();
    QTextStream outputStream(&fileContent_);

    writer_->write(outputStream, outputDirectory);
}

//-----------------------------------------------------------------------------
// Function: ModelSimDocument::getImplementation()
//-----------------------------------------------------------------------------
void ModelSimDocument::getBodyHighlight(int& begin, int& end) const
{
    begin = -1;
    end = -1;
}