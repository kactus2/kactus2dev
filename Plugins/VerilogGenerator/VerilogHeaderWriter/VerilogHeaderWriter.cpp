//-----------------------------------------------------------------------------
// File: VerilogHeaderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------

#include "VerilogHeaderWriter.h"

#include <IPXACTmodels/vlnv.h>

//-----------------------------------------------------------------------------
// Function: VerilogHeaderWriter::()
//-----------------------------------------------------------------------------
VerilogHeaderWriter::VerilogHeaderWriter(VLNV const& vlnv, QString const& xmlPath , QString const& author) : 
vlnv_(vlnv), xmlPath_(xmlPath), author_(author)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogHeaderWriter::()
//-----------------------------------------------------------------------------
VerilogHeaderWriter::~VerilogHeaderWriter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogHeaderWriter::write()
//-----------------------------------------------------------------------------
void VerilogHeaderWriter::write(QTextStream& outputStream, QString const& fileName, QString const& description, 
    QDateTime const& generationTime) const
{
    QStringList descriptionLines = description.split("\n");

    outputStream << "//-----------------------------------------------------------------------------" << endl;
    outputStream << "// File          : " << fileName << endl;
    outputStream << "// Creation date : " << generationTime.date().toString(Qt::LocalDate) << endl;
    outputStream << "// Creation time : " << generationTime.time().toString(Qt::LocalDate) << endl;
    outputStream << "// Description   : " << descriptionLines.first() << endl;

    for (int i = 1; i < descriptionLines.count(); i++)
    {
        outputStream << "//                 " << descriptionLines.at(i) << endl;
    }

    outputStream << "// Created by    : " << author_ << endl;
    outputStream << "// This file was generated with Kactus2 verilog generator" << endl;
    outputStream << "// based on IP-XACT component " << vlnv_.toString() << endl;
    outputStream << "// whose XML file is " << xmlPath_ << endl;
    outputStream << "//-----------------------------------------------------------------------------" << endl;
    outputStream << endl;
}
