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
VerilogAssignmentWriter::VerilogAssignmentWriter() :
ports_()
{
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::~VerilogAssignmentWriter()
//-----------------------------------------------------------------------------
VerilogAssignmentWriter::~VerilogAssignmentWriter()
{

}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::addPort()
//-----------------------------------------------------------------------------
void VerilogAssignmentWriter::addPort(QSharedPointer<MetaPort> port)
{
    ports_.append(port);
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::write()
//-----------------------------------------------------------------------------
void VerilogAssignmentWriter::write(QTextStream& output) const
{
    if (ports_.isEmpty())
    {
        return;
    }

    QString informationLine = "    // Assignments for the ports of the encompassing component";

    output << informationLine << endl;

    foreach (QSharedPointer<MetaPort> mPort, ports_)
    {
        bool isOutPort =
            (mPort->port_->getDirection() == DirectionTypes::OUT || mPort->port_->getDirection() == DirectionTypes::INOUT);

        if (mPort->assignments_.size() < 1 && isOutPort)
        {
            output << "    assign " << mPort->port_->name() << " = "  << mPort->defaultValue_ << ";" << endl;

            continue;
        }

        foreach (QSharedPointer<MetaPortAssignMent> mpa, mPort->assignments_)
        {
            QString outputLine = assignmentForHierPort(mpa, mPort, isOutPort);

            if (!outputLine.isEmpty())
            {
                output << outputLine << endl;
            }
        }
    }

    output << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::assignmentFsorHierPort()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::assignmentForHierPort(QSharedPointer<MetaPortAssignMent> mpa,
    QSharedPointer<MetaPort> mPort, bool isOutPort)
{
    QString assignmentString;

    if (isOutPort)
    {
        assignmentString = "    assign <portName>[<portLeft>:<portRight>] = <wireName>[<wireLeft>:<wireRight>];";
    }
    else
    {
        assignmentString = "    assign <wireName>[<wireLeft>:<wireRight>] = <portName>[<portLeft>:<portRight>];";
    }

    QPair<QString,QString> portBounds = mpa->physicalBounds_;

    // Use bounds only if they are not the same.
    if (portBounds.first == portBounds.second)
    {
        assignmentString.remove("[<portLeft>:<portRight>]");
    }
    else
    {
        assignmentString.replace("<portLeft>", portBounds.first);
        assignmentString.replace("<portRight>", portBounds.second);
    }

    assignmentString.replace("<portName>", mPort->port_->name());

    if (mpa->wire_)
    {
        QPair<QString,QString> wireBounds = mpa->logicalBounds_;

        // Use bounds only if they are not the same.
        if (wireBounds.first == wireBounds.second)
        {
            assignmentString.remove("[<wireLeft>:<wireRight>]");
        }
        else
        {
            assignmentString.replace("<wireLeft>", wireBounds.first);
            assignmentString.replace("<wireRight>", wireBounds.second);
        }

        assignmentString.replace("<wireName>", mpa->wire_->name_);
    }
    else if (isOutPort)
    {
        // If a default value is assigned to a physical port, it shall be used.
        assignmentString.remove("[<wireLeft>:<wireRight>]");
        assignmentString.replace("<wireName>", mpa->defaultValue_);
    }
    else
    {
        assignmentString = "";
    }

    return assignmentString;
}