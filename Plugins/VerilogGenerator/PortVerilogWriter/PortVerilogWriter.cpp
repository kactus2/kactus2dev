//-----------------------------------------------------------------------------
// File: PortVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#include "PortVerilogWriter.h"
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::PortVerilogWriter(QSharedPointer<MetaPort> port) : port_(port)
{

}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::~PortVerilogWriter()
{

}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::write()
//-----------------------------------------------------------------------------
void PortVerilogWriter::write( QTextStream& outputStream ) const
{
    if (!nothingToWrite())
    {
        outputStream << createDeclaration();
    }
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool PortVerilogWriter::nothingToWrite() const
{
    return port_.isNull() || 
        port_->port_->getDirection() == DirectionTypes::DIRECTION_PHANTOM ||
        port_->port_->getDirection() == DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::createDeclaration() const
{
    QString portDeclaration("<direction> <type> <vectorBounds> <array> <name>");

    portDeclaration.replace("<direction>", direction().leftJustified(6));
    portDeclaration.replace("<type>", port_->port_->getTypeName().leftJustified(7));
    portDeclaration.replace("<vectorBounds>", formatBounds(port_->vectorBounds_).leftJustified(14));
    portDeclaration.replace("<array>", formatBounds(port_->arrayBounds_).leftJustified(5));
    portDeclaration.replace("<name>", port_->port_->name());

    return portDeclaration;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::direction()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::direction() const
{
    QString directionString;

    if (port_->port_->getDirection() == DirectionTypes::IN)
    {
        directionString = "input";
    }
    else if (port_->port_->getDirection() == DirectionTypes::OUT)    
    {
        directionString = "output";
    }
    else if (port_->port_->getDirection() == DirectionTypes::INOUT)   
    {
        directionString = "inout";
    }

    return directionString;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::formatBounds()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::formatBounds(QPair<QString, QString> const& bounds) const
{
    QString vectorDefinition = "[" + bounds.first + ":" + bounds.second + "]";

    vectorDefinition.remove(" ");

    if (vectorDefinition == "[0:0]" || vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }
	
    return vectorDefinition;
}
