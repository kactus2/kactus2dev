//-----------------------------------------------------------------------------
// File: ModelSimWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Executes writing a ModelSim do file.
//-----------------------------------------------------------------------------

#include "ModelSimWriter.h"

#include <IPXACTmodels/generaldeclarations.h>

//-----------------------------------------------------------------------------
// Function: ModelSimWriter::ModelSimWriter()
//-----------------------------------------------------------------------------
ModelSimWriter::ModelSimWriter()
{
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriter::write()
//-----------------------------------------------------------------------------
void ModelSimWriter::write(QTextStream& output, QString const& outputDirectory) const
{
    // Inform user when executing the script.
    output << "echo " << QObject::tr("\"Adding files to the project\"") << endl;

    output << endl;

    // Each path gets a line: Create a relative path from output directory to the location.
    foreach (QString absolutePath, paths_)
    {
        QString relativePath = General::getRelativePath(outputDirectory, absolutePath);

        output << "project addfile " << relativePath << endl;
    }

    output << endl;

    // Latsly, compile the project.
    output << "project compileall" << endl;

    output << endl;

    // Inform user that script has been successfully executed.
    output << "echo " << QObject::tr("\"Script has been executed\"") << endl;
}

//-----------------------------------------------------------------------------
// Function: ModelSimWriter::addPath()
//-----------------------------------------------------------------------------
void ModelSimWriter::addPath(QString const& path)
{
    if (!paths_.contains(path))
    {
        paths_.append(path);
    }
}