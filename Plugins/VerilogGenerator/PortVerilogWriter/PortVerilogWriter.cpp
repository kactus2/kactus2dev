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
PortVerilogWriter::PortVerilogWriter(QSharedPointer<GenerationPort> port) : port_(port)
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
        port_->port_->getDirection() == DirectionTypes::DIRECTION_PHANTOM || port_->port_->getDirection() == DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::createDeclaration() const
{
    QString portDeclaration("<direction> <type> <bounds> <name>");

    portDeclaration.replace("<direction>", direction().leftJustified(6));
    portDeclaration.replace("<type>", port_->port_->getTypeName().leftJustified(7));
    portDeclaration.replace("<bounds>", arrayAndVectorBounds().leftJustified(20));
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
// Function: PortVerilogWriter::arrayAndVectorBounds()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::arrayAndVectorBounds() const
{
    QString arrayDefinition = "[" + port_->arrayBounds_.first + ":" + port_->arrayBounds_.second + "]";

    arrayDefinition.remove(" ");

    if (arrayDefinition == "[:]")
    {
        arrayDefinition.clear();
    }

    QString vectorDefinition = "[" + port_->vectorBounds_.first + ":" + port_->vectorBounds_.second + "]";

    vectorDefinition.remove(" ");

    if (vectorDefinition == "[0:0]" || vectorDefinition == "[:]")
    {
        vectorDefinition.clear();
    }
	
    return arrayDefinition + vectorDefinition;
}
