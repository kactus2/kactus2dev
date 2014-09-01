//-----------------------------------------------------------------------------
// File: VerilogWireWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 21.08.2014
//
// Description:
// Class for writing a Verilog wire declaration.
//-----------------------------------------------------------------------------

#include "VerilogWireWriter.h"

//-----------------------------------------------------------------------------
// Function: InterconnectionVerilogWriter::InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogWireWriter::VerilogWireWriter(QString connectionName, int size) :
name_(connectionName), size_(size)
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionVerilogWriter::~InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogWireWriter::~VerilogWireWriter()
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionVerilogWriter::write()
//-----------------------------------------------------------------------------
void VerilogWireWriter::write(QTextStream& output) const
{
    output << "    " << createDeclaration() << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::createDeclaration() const
{
    QString declaration("wire <size> <name>;");    
    declaration.replace("<size>", formattedSize());
    declaration.replace("<name>", name_);

    return declaration.simplified();;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::formattedSize()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::formattedSize() const
{
    QString sizeString = "";
    if (size_ > 1)
    {
        sizeString = "[" + QString::number(size_ - 1) + ":0]";
    }

    return sizeString;
}
