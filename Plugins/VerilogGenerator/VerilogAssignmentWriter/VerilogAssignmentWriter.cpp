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

#include <Plugins/VerilogImport/VerilogSyntax.h>

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::VerilogAssignmentWriter()
//-----------------------------------------------------------------------------
VerilogAssignmentWriter::VerilogAssignmentWriter(QString portWireName,
    QSharedPointer<MetaPortAssignment> portAssignment,
    QSharedPointer<MetaPort> mPort,
    bool isHierPort) :
    portWireName_(portWireName),
    mpa_(portAssignment),
    mPort_(mPort),
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
    DirectionTypes::Direction dir = mPort_->port_->getDirection();
    bool isOutPort = (dir == DirectionTypes::OUT || dir == DirectionTypes::INOUT);
    bool isInPort = (dir == DirectionTypes::IN || dir == DirectionTypes::INOUT);

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

    // The selected bounds of the component port that will be assigned.
    QPair<QString,QString> portBounds = mpa_->physicalBounds_;
    // The bounds of the wire that will be assigned to selected bounds of the component port.
    QPair<QString,QString> wireBounds;
    // The bounds of the selected part wire that will be assigned to selected bounds of the component port.
    QPair<QString,QString> logicalBounds;

    if (mpa_->wire_)
    {
        logicalBounds = mpa_->logicalBounds_;
        wireBounds = mpa_->wire_->bounds_;

        // Use bounds only if they are not the same.
        if (logicalBounds.first == logicalBounds.second)
        {
            // If the chosen port bounds differ, must select the bit.
            if (wireBounds.first == wireBounds.second)
            {
                assignmentString.remove("[<logicalLeft>:<logicalRight>]");
            }
            else
            {
                assignmentString.replace("<logicalLeft>:<logicalRight>", logicalBounds.first);
            }
        }
        else
        {
            assignmentString.replace("<logicalLeft>", logicalBounds.first);
            assignmentString.replace("<logicalRight>", logicalBounds.second);
        }

        assignmentString.replace("<logicalWireName>", VerilogSyntax::legalizeName(mpa_->wire_->name_));
    }
    else if ((!isInHierPort_ && isInPort) || (isInHierPort_ && isOutPort))
    {
        QString defaultValue = mpa_->defaultValue_;

        if (defaultValue.isEmpty())
        {
            defaultValue = mPort_->defaultValue_;
        }

        // If a default value is assigned to a physical port, it shall be used.
        assignmentString.remove("[<logicalLeft>:<logicalRight>]");
        assignmentString.replace("<logicalWireName>", defaultValue);
    }
    else
    {
        return "";
    }

    // Use bounds only if they are not the same.
    if (portBounds.first == portBounds.second)
    {
        // If the chosen wire bounds differ, must select the bit.
        if (mPort_->vectorBounds_.first == mPort_->vectorBounds_.second)
        {
            assignmentString.remove("[<physicalLeft>:<physicalRight>]");
        }
        else
        {
            assignmentString.replace("<physicalLeft>:<physicalRight>", portBounds.first);
        }
    }
    else
    {
        assignmentString.replace("<physicalLeft>", portBounds.first);
        assignmentString.replace("<physicalRight>", portBounds.second);
    }

    assignmentString.replace("<portWireName>", portWireName_);

    return assignmentString;
}