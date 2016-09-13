//-----------------------------------------------------------------------------
// File: VerilogHeaderWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for generating a header for a Verilog file.
//-----------------------------------------------------------------------------

#include "VerilogHeaderWriter.h"

#include <IPXACTmodels/common/VLNV.h>

//-----------------------------------------------------------------------------
// Function: VerilogHeaderWriter::()
//-----------------------------------------------------------------------------
VerilogHeaderWriter::VerilogHeaderWriter(VLNV const& vlnv, QString const& xmlPath, QString const& author, QString const& description) : 
vlnv_(vlnv), xmlPath_(xmlPath), author_(author), description_(description)
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
void VerilogHeaderWriter::write(QTextStream& outputStream, QString const& fileName, QString const& generatorVersion,
	QString const& kactusVersion, QDateTime const& generationTime) const
{
    QStringList descriptionLines = description_.split("\n");

    outputStream << "//-----------------------------------------------------------------------------" << endl;
    outputStream << "// File          : " << fileName << endl;
    outputStream << "// Creation date : " << generationTime.date().toString("dd.MM.yyyy") << endl;
	outputStream << "// Creation time : " << generationTime.time().toString("hh:mm:ss") << endl;
    outputStream << "// Description   : " << descriptionLines.first() << endl;

    for (int i = 1; i < descriptionLines.count(); i++)
    {
        outputStream << "//                 " << descriptionLines.at(i) << endl;
    }

	outputStream << "// Created by    : " << author_ << endl;
	outputStream << "// Tool : Kactus2 " << kactusVersion << endl;
	outputStream << "// Plugin : Verilog generator " << generatorVersion << endl;
	outputStream << "// This file was generated based on IP-XACT component " << vlnv_.toString() << endl;
    outputStream << "// whose XML file is " << xmlPath_ << endl;
    outputStream << "//-----------------------------------------------------------------------------" << endl;
    outputStream << endl;
}
