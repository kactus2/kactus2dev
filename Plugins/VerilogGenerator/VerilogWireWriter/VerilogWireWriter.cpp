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
// Function: VerilogWireWriter::InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogWireWriter::VerilogWireWriter(QSharedPointer<GenerationWire> wire) : wire_(wire)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::~InterconnectionVerilogWriter()
//-----------------------------------------------------------------------------
VerilogWireWriter::~VerilogWireWriter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::write()
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
    declaration.replace("<size>", formattedSize().leftJustified(6));
    declaration.replace("<name>", wire_->name);

    return declaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::formattedSize()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::formattedSize() const
{
	QString sizeString;

	if (wire_->bounds.first != wire_->bounds.second)
	{
		sizeString = "[" + wire_->bounds.first + ":" + wire_->bounds.second + "]";
	}

    return sizeString;
}
