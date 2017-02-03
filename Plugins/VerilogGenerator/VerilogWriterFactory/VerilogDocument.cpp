//-----------------------------------------------------------------------------
// File: VerilogDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers, logic for calling them.
//-----------------------------------------------------------------------------

#include "VerilogDocument.h"

#include <QDateTime>
#include <QTextStream>

//-----------------------------------------------------------------------------
// Function: VerilogDocument::write()
//-----------------------------------------------------------------------------
void VerilogDocument::write()
{
    fileContent_.clear();
    QTextStream outputStream(&fileContent_);
    headerWriter_->write(outputStream, fileName_, QDateTime::currentDateTime());
    topWriter_->write(outputStream);
}