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
// Function: VerilogHeaderWriter::VerilogHeaderWriter()
//-----------------------------------------------------------------------------
VerilogHeaderWriter::VerilogHeaderWriter(VLNV const& vlnv, QString const& xmlPath, QString const& author,
    QString const& description, QString const& kactusVersion, QString const& generatorVersion) : 
    vlnv_(vlnv), 
    xmlPath_(xmlPath),
    author_(author),
    description_(description),
    generatorVersion_(generatorVersion),
    kactusVersion_(kactusVersion)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogHeaderWriter::write()
//-----------------------------------------------------------------------------
void VerilogHeaderWriter::write(QTextStream& outputStream, QString const& fileName, QDateTime const& generationTime) const
{
    QStringList descriptionLines = description_.split("\n");

    outputStream << "//-----------------------------------------------------------------------------" << Qt::endl;
    outputStream << "// File          : " << fileName << Qt::endl;
    outputStream << "// Creation date : " << generationTime.date().toString("dd.MM.yyyy") << Qt::endl;
	outputStream << "// Creation time : " << generationTime.time().toString("hh:mm:ss") << Qt::endl;
    outputStream << "// Description   : " << descriptionLines.first() << Qt::endl;

    for (int i = 1; i < descriptionLines.count(); i++)
    {
        outputStream << "//                 " << descriptionLines.at(i) << Qt::endl;
    }

	outputStream << "// Created by    : " << author_ << Qt::endl;
	outputStream << "// Tool : Kactus2 " << kactusVersion_ << Qt::endl;
	outputStream << "// Plugin : Verilog generator " << generatorVersion_ << Qt::endl;
	outputStream << "// This file was generated based on IP-XACT component " << vlnv_.toString() << Qt::endl;
    outputStream << "// whose XML file is " << xmlPath_ << Qt::endl;
    outputStream << "//-----------------------------------------------------------------------------" << Qt::endl;
    outputStream << Qt::endl;
}
