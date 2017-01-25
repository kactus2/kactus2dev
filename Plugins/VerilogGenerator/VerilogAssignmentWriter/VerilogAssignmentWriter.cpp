//-----------------------------------------------------------------------------
// File: VerilogAssignmentWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Janne Virtanen
// Date: 21.01.2017
//
// Description:
// Class for writing a Verilog assignments.
//-----------------------------------------------------------------------------

#include "VerilogAssignmentWriter.h"

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::VerilogAssignmentWriter()
//-----------------------------------------------------------------------------
VerilogAssignmentWriter::VerilogAssignmentWriter(QString portWireName,
    QSharedPointer<MetaPortAssignment> portAssignment,
    DirectionTypes::Direction direction,
    bool isHierPort) :
    portWireName_(portWireName),
    mpa_(portAssignment),
    direction_(direction),
    isInHierPort_(isHierPort)
{
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::~VerilogAssignmentWriter()
//-----------------------------------------------------------------------------
VerilogAssignmentWriter::~VerilogAssignmentWriter()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::write()
//-----------------------------------------------------------------------------
void VerilogAssignmentWriter::write(QTextStream& output) const
{
    QString outputLine = assignmentForPort();

    if (!outputLine.isEmpty())
    {
        output << "    assign " << outputLine << ";" <<endl;
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::assignmentForPort()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::assignmentForPort() const
{
    bool isOutPort = (direction_ == DirectionTypes::OUT || direction_ == DirectionTypes::INOUT);
    bool isInPort = (direction_ == DirectionTypes::IN || direction_ == DirectionTypes::INOUT);

    QString assignmentString;

    if ((!isInHierPort_ && isInPort) || (isInHierPort_ && isOutPort))
    {
        assignmentString = "<portWireName>[<physicalLeft>:<physicalRight>] = <logicalWireName>[<logicalLeft>:<logicalRight>]";
    }
    else if ((!isInHierPort_ && isOutPort) || (isInHierPort_ && isInPort))
    {
        assignmentString = "<logicalWireName>[<logicalLeft>:<logicalRight>] = <portWireName>[<physicalLeft>:<physicalRight>]";
    }
    else
    {
        return "";
    }

    if (mpa_->wire_)
    {
        QPair<QString,QString> wireBounds = mpa_->logicalBounds_;

        // Use bounds only if they are not the same.
        if (wireBounds.first == wireBounds.second)
        {
            assignmentString.remove("[<logicalLeft>:<logicalRight>]");
        }
        else
        {
            assignmentString.replace("<logicalLeft>", wireBounds.first);
            assignmentString.replace("<logicalRight>", wireBounds.second);
        }

        assignmentString.replace("<logicalWireName>", mpa_->wire_->name_);
    }
    else if ((!isInHierPort_ && isInPort) || (isInHierPort_ && isOutPort))
    {
        // If a default value is assigned to a physical port, it shall be used.
        assignmentString.remove("[<logicalLeft>:<logicalRight>]");
        assignmentString.replace("<logicalWireName>", mpa_->defaultValue_);
    }
    else
    {
        return "";
    }

    QPair<QString,QString> portBounds = mpa_->physicalBounds_;

    // Use bounds only if they are not the same.
    if (portBounds.first == portBounds.second)
    {
        assignmentString.remove("[<physicalLeft>:<physicalRight>]");
    }
    else
    {
        assignmentString.replace("<physicalLeft>", portBounds.first);
        assignmentString.replace("<physicalRight>", portBounds.second);
    }

    assignmentString.replace("<portWireName>", portWireName_);

    return assignmentString;
}