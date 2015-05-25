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

#include <editors/ComponentEditor/common/ExpressionFormatter.h>

//-----------------------------------------------------------------------------
// Function: PortVerilogWriter::PortVerilogWriter()
//-----------------------------------------------------------------------------
PortVerilogWriter::PortVerilogWriter(QSharedPointer<const Port> port, 
    QSharedPointer<ExpressionFormatter> expressionFormatter)
    : port_(port), formatter_(expressionFormatter)
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

    outputStream << createDeclaration();
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
    QString portDeclaration("<direction> <type> <bounds> <name>");

    portDeclaration.replace("<direction>", direction().leftJustified(6));
    portDeclaration.replace("<type>", port_->getTypeName().leftJustified(7));
    portDeclaration.replace("<bounds>", formatter_->formatReferringExpression(bounds()).leftJustified(6));
    portDeclaration.replace("<name>", port_->getName());

    return portDeclaration;
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
    QString boundsDefinition = "[" + port_->getLeftBoundExpression() + ":" + 
        port_->getRightBoundExpression() + "]";

    boundsDefinition.remove(" ");

    if (boundsDefinition == "[0:0]")
    {
        return QString();
    }

    return boundsDefinition;
}
