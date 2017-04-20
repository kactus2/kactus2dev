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
    implementationStart_ = -1;
    implementationEnd_ = -1;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::write()
//-----------------------------------------------------------------------------
void VerilogDocument::write(QString const& outputDirectory)
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
    begin = implementationStart_;
    end = implementationEnd_;
}

//-----------------------------------------------------------------------------
// Function: VerilogDocument::selectImplementation()
//-----------------------------------------------------------------------------
bool VerilogDocument::selectImplementation(QString const& code, QString& implementation,
    QString& postModule, QString& error)
{
    if (!VerilogSyntax::findImplementation(code, implementationStart_, implementationEnd_, error))
    {
        return false;
    }

    // Rip the detected implementation.
    int implementationLength = implementationEnd_ - implementationStart_;
    implementation = code.mid(implementationStart_,implementationLength);

    // Then take all the text that comes after the module, just in case.
    int postStart = implementationEnd_ + 9;
    postModule = code.mid(postStart);

    // Also trim away extra white space.
    postModule = postModule.trimmed();

    // The destructor shall close the file. All done here.
    return true;
}