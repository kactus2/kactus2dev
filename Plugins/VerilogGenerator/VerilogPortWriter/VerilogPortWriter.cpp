//-----------------------------------------------------------------------------
// File: VerilogPortWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 28.7.2014
//
// Description:
// Class for writing port declarations in Verilog.
//-----------------------------------------------------------------------------

#include "VerilogPortWriter.h"
#include <IPXACTmodels/Component/Port.h>

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::VerilogPortWriter()
//-----------------------------------------------------------------------------
VerilogPortWriter::VerilogPortWriter(QSharedPointer<MetaPort> port) : port_(port)
{

}

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::write()
//-----------------------------------------------------------------------------
void VerilogPortWriter::write( QTextStream& outputStream ) const
{
    if (!nothingToWrite())
    {
        outputStream << createDeclaration();
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool VerilogPortWriter::nothingToWrite() const
{
    return port_.isNull() || 
        port_->port_->getDirection() == DirectionTypes::DIRECTION_PHANTOM ||
        port_->port_->getDirection() == DirectionTypes::DIRECTION_INVALID;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::createDeclaration()
//-----------------------------------------------------------------------------
QString VerilogPortWriter::createDeclaration() const
{
    QString portDeclaration("<direction> <type> <array> <vectorBounds> <name>");

    portDeclaration.replace("<direction>", direction().leftJustified(6));
    portDeclaration.replace("<type>", port_->port_->getTypeName().leftJustified(7));
    portDeclaration.replace("<vectorBounds>", formatBounds(port_->vectorBounds_).leftJustified(14));
    portDeclaration.replace("<array>", formatBounds(port_->arrayBounds_).leftJustified(5));
    portDeclaration.replace("<name>", port_->port_->name());

    return portDeclaration;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::direction()
//-----------------------------------------------------------------------------
QString VerilogPortWriter::direction() const
{
    QString directionString;

    if (port_->port_->getDirection() == DirectionTypes::IN)
    {
        directionString = QStringLiteral("input");
    }
    else if (port_->port_->getDirection() == DirectionTypes::OUT)    
    {
        directionString = QStringLiteral("output");
    }
    else if (port_->port_->getDirection() == DirectionTypes::INOUT)   
    {
        directionString = QStringLiteral("inout");
    }

    return directionString;
}

//-----------------------------------------------------------------------------
// Function: VerilogPortWriter::formatBounds()
//-----------------------------------------------------------------------------
QString VerilogPortWriter::formatBounds(QPair<QString, QString> const& bounds) const
{
    QString vectorDefinition = "[" + bounds.first + ":" + bounds.second + "]";
    vectorDefinition.remove(QLatin1Char(' '));

    if (vectorDefinition.compare(QLatin1String("[0:0]")) == 0 || 
        vectorDefinition.compare(QLatin1String("[:]")) == 0)
    {
        vectorDefinition.clear();
    }
	
    return vectorDefinition;
}
