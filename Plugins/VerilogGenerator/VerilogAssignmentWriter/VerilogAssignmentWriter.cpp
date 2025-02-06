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
    portAssignment_(portAssignment),
    mPort_(mPort),
    isHierarchicalPort_(isHierPort)
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
        output << QStringLiteral("    assign ") << outputLine << QStringLiteral(";") << Qt::endl;
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::assignmentForPort()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::assignmentForPort() const
{
    DirectionTypes::Direction direction = mPort_->port_->getDirection();
    bool isOutPort = (direction == DirectionTypes::OUT || direction == DirectionTypes::INOUT);
    bool isInPort = (direction == DirectionTypes::IN || direction == DirectionTypes::INOUT);

    bool assignToPort = (!isHierarchicalPort_ && isInPort) || (isHierarchicalPort_ && isOutPort);
    bool assignToWire = (isHierarchicalPort_ && isInPort) || (!isHierarchicalPort_ && isOutPort);
    
    QString logicalAssign = createLogicalAssignment(assignToPort);
    QString physicalAssign = createPhysicalAssignment();

    if (logicalAssign.isEmpty() || physicalAssign.isEmpty())
    {
        return QString();
    }

    QString invert = QString();
    if (portAssignment_->invert_ && !isHierarchicalPort_)
    {
        invert = QStringLiteral("~");
    }

    if (assignToPort)
    {
        return physicalAssign +  QStringLiteral(" = ") + invert + logicalAssign;
    }
    else if (assignToWire)
    {
        return logicalAssign + QStringLiteral(" = ") + invert + physicalAssign;
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::createLogicalAssignment()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::createLogicalAssignment(bool assignToPort) const
{
    if (portAssignment_->wire_)
    {
        QString logicalAssign = QStringLiteral("<logicalWireName>[<logicalLeft>:<logicalRight>]");

        // The bounds of the selected part wire that will be assigned to selected bounds of the component port.
        QPair<QString, QString> logicalBounds = portAssignment_->logicalBounds_;

        // The bounds of the wire that will be assigned to selected bounds of the component port.
        QPair<QString, QString> wireBounds = portAssignment_->wire_->bounds_;
        if (logicalBounds == wireBounds)
        {
            logicalAssign.remove(QStringLiteral("[<logicalLeft>:<logicalRight>]"));
        }
        else if (logicalBounds.first == logicalBounds.second)
        {
            if (wireBounds.first == wireBounds.second)
            {
                logicalAssign.remove(QStringLiteral("[<logicalLeft>:<logicalRight>]"));
            }
            else  // If the chosen port bounds differ, must select the bit.
            {
                logicalAssign.replace(QStringLiteral("<logicalLeft>:<logicalRight>"), logicalBounds.first);
            }
        }
        else
        {
            logicalAssign.replace(QStringLiteral("<logicalLeft>"), logicalBounds.first);
            logicalAssign.replace(QStringLiteral("<logicalRight>"), logicalBounds.second);
        }

        QString portName = VerilogSyntax::legalizeName(portAssignment_->wire_->name_);
        logicalAssign.replace(QStringLiteral("<logicalWireName>"), portName);
        return logicalAssign;
    }
    else if (assignToPort)
    {    
        // If a default value is assigned to a physical port, it shall be used.
        QString defaultValue = portAssignment_->defaultValue_;
        if (defaultValue.isEmpty())
        {
            defaultValue = mPort_->defaultValue_;
        }

        return defaultValue;
    }
    else
    {
        return QString();
    }
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::createPhysicalAssignment()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::createPhysicalAssignment() const
{
    QString portAssign = portWireName_ + QStringLiteral("[<physicalLeft>:<physicalRight>]");

    QPair<QString, QString> portBounds = portAssignment_->physicalBounds_;
    QPair<QString, QString> logicalBounds = portAssignment_->logicalBounds_;
    if (logicalBounds == portBounds && logicalBounds == mPort_->vectorBounds_)
    {
        portAssign.remove(QStringLiteral("[<physicalLeft>:<physicalRight>]"));
    }
    // Use bounds only if they are not the same.
    else if (portBounds.first == portBounds.second)
    {
        if (mPort_->vectorBounds_.first == mPort_->vectorBounds_.second)
        {
            portAssign.remove(QStringLiteral("[<physicalLeft>:<physicalRight>]"));
        }
        else // If the chosen wire bounds differ, must select the bit.
        {
            portAssign.replace(QStringLiteral("<physicalLeft>:<physicalRight>"), portBounds.first);
        }
    }
    else
    {
        portAssign.replace(QStringLiteral("<physicalLeft>"), portBounds.first);
        portAssign.replace(QStringLiteral("<physicalRight>"), portBounds.second);
    }

    return portAssign;
}
