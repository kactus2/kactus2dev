//-----------------------------------------------------------------------------
// File: PortVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#include "PortVerilogWriter.h"

#include <kactusGenerators/HDLGenerator/HDLUtils.h>

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::PortVerilogWriter(QSharedPointer<const Port> port)
    : port_(port)
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
    if (nothingToWrite())
    {
        return;
    }
    
    QString indentation = "    ";

    outputStream << indentation << createDeclaration();

    if (port_->getDescription().isEmpty())
    {
        outputStream << endl;
    }
    else 
    {
        outputStream << " ";
        HDLUtils::writeDescription(port_->getDescription(), outputStream, "//");
    }
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool PortVerilogWriter::nothingToWrite() const
{
    return port_.isNull() || 
        port_->getDirection() == General::DIRECTION_PHANTOM || port_->getDirection() == General::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::createDeclaration() const
{
    QString portDeclaration("<direction> <type><bounds> <name>;");

    portDeclaration.replace("<direction>", direction());
    portDeclaration.replace("<type>", port_->getTypeName());
    portDeclaration.replace("<bounds>", bounds());
    portDeclaration.replace("<name>", port_->getName());

    return portDeclaration.simplified();
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::direction()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::direction() const
{
    QString directionString;

    if (port_->getDirection() == General::IN)
    {
        directionString = "input";
    }
    else if (port_->getDirection() == General::OUT)    
    {
        directionString = "output";
    }
    else if (port_->getDirection() == General::INOUT)   
    {
        directionString = "inout";
    }

    return directionString;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::bounds()
//-----------------------------------------------------------------------------
QString PortVerilogWriter::bounds() const
{
    QString boundsDefinition;
    if (port_->getPortSize() > 1)
    {
        boundsDefinition = "[" + QString::number(port_->getLeftBound()) + ":" + 
                               QString::number(port_->getRightBound()) + "]";
    }

    return boundsDefinition;
}
