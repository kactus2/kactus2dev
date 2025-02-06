//-----------------------------------------------------------------------------
// File: VerilogDocument.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 26.01.2017
//
// Description:
// Storage for Verilog writers.
//-----------------------------------------------------------------------------

#include "VerilogDocument.h"

#include <QDateTime>
#include <QTextStream>

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: VerilogDocument::VerilogDocument()
//-----------------------------------------------------------------------------
VerilogDocument::VerilogDocument()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::write()
//-----------------------------------------------------------------------------
void VerilogDocument::write(QString const& /*outputDirectory*/)
{
    fileContent_.clear();
    QTextStream outputStream(&fileContent_);
    headerWriter_->write(outputStream, fileName_, QDateTime::currentDateTime());
    topWriter_->write(outputStream);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::getImplementation()
//-----------------------------------------------------------------------------
void VerilogDocument::getBodyHighlight(int& begin, int& end) const
{
    QString error;
    VerilogSyntax::findImplementation(fileContent_, begin, end, error);
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::selectImplementation()
//-----------------------------------------------------------------------------
bool VerilogDocument::selectImplementation(QString const& code, QString& implementation,
    QString& postModule, QString& error)
{
    // The Verilog syntax defines how it should be done.
    return VerilogSyntax::selectImplementation(code, implementation, postModule, error);
}
