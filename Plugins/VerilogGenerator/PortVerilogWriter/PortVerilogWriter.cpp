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
void PortVerilogWriter::write(QTextStream& outputStream)
{
    if (shouldNotWrite())
    {
        return;
    }

    writeIndent(outputStream);

    writeDirection(outputStream);

    writeType(outputStream);

    writeName(outputStream);

    writeDescription(outputStream);
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::shouldNotWrite()
//-----------------------------------------------------------------------------
bool PortVerilogWriter::shouldNotWrite()
{
    return port_->getDirection() == General::DIRECTION_PHANTOM || port_->getDirection() == General::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::writeIndent()
//-----------------------------------------------------------------------------
void PortVerilogWriter::writeIndent(QTextStream& outputStream)
{
    outputStream << "    ";
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::writeDirection()
//-----------------------------------------------------------------------------
void PortVerilogWriter::writeDirection(QTextStream& outputStream)
{
    if (port_->getDirection() == General::IN)
    {
        outputStream << "input ";   
    }
    else if (port_->getDirection() == General::OUT)    
    {
        outputStream << "output ";
    }
    else
    {
        outputStream << "inout ";
    }
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::writeType()
//-----------------------------------------------------------------------------
void PortVerilogWriter::writeType(QTextStream& outputStream)
{
    if (!port_->getTypeName().isEmpty())
    {
        outputStream << port_->getTypeName() << " ";
    }

    if (port_->getPortSize() > 1)
    {
        outputStream << "[" + QString::number(port_->getLeftBound()) << ":" <<
            QString::number(port_->getRightBound()) << "] ";
    }
}

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::writeName()
//-----------------------------------------------------------------------------
void PortVerilogWriter::writeName(QTextStream& outputStream)
{
    outputStream << port_->getName() << ";";
}


//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::writeDescription()
//-----------------------------------------------------------------------------
void PortVerilogWriter::writeDescription(QTextStream& outputStream)
{
    if (!port_->getDescription().isEmpty())
    {
        outputStream << " ";
        HDLUtils::writeDescription(port_->getDescription(), outputStream, "//");
    }
    else
    {
        outputStream << "\n";
    }
}
