//-----------------------------------------------------------------------------
// File: ModelSimWriter.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 20.04.2017
//
// Description:
// Executes writing a ModelSim do file.
//-----------------------------------------------------------------------------

#ifndef MODELSIMWRITER_H
#define MODELSIMWRITER_H

#include <QList>
#include <QString>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Executes writing a ModelSim do file.
//-----------------------------------------------------------------------------
struct ModelSimWriter
{
    //! The constructor.
    ModelSimWriter();

    /*!
     *  Writes the content to the given output.
     *
     *      @param [in] output   The output to write to.
     */
    void write(QTextStream& output, QString const& outputDirectory) const;
    
    /*!
     *  Adds an absolute path to the list of paths that needs to be referred.
     */
    void addPath(QString const& path);
    
private:
    QList<QString> paths_;
};

#endif // MODELSIMWRITER_H
