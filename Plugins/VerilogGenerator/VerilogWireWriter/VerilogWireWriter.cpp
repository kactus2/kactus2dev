//-----------------------------------------------------------------------------
// File: VerilogWireWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 21.08.2014
//
// Description:
// Class for writing a Verilog wire declaration.
//-----------------------------------------------------------------------------

#include "VerilogWireWriter.h"

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::VerilogWireWriter()
//-----------------------------------------------------------------------------
VerilogWireWriter::VerilogWireWriter(QString name, QPair<QString, QString> bounds)
    : name_(name), bounds_(bounds)
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
    declaration.replace("<name>", name_);

    return declaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::formattedSize()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::formattedSize() const
{
	QString sizeString;

	if (bounds_.first != bounds_.second)
	{
		sizeString = "[" + bounds_.first + ":" + bounds_.second + "]";
	}

    return sizeString;
}
