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
VerilogWireWriter::VerilogWireWriter(QString name, QPair<QString, QString> bounds, QPair<QString, QString> array)
    : name_(name), bounds_(bounds), array_(array)
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
    output << "    " << createDeclaration() << Qt::endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::createDeclaration() const
{
    QString declaration("wire <array><size> <name>;");    
    declaration.replace("<array>", formattedSize(array_)); 
    declaration.replace("<size>", formattedSize(bounds_).leftJustified(5));
    declaration.replace("<name>", name_);

    return declaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogWireWriter::formattedSize()
//-----------------------------------------------------------------------------
QString VerilogWireWriter::formattedSize(QPair<QString, QString> const& bounds) const
{
	QString sizeString;

	if (bounds.first != bounds.second)
	{
		sizeString = "[" + bounds.first + ":" + bounds.second + "]";
	}

    return sizeString;
}
