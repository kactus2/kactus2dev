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
        if (mPort->port_->getDirection() == DirectionTypes::OUT || mPort->port_->getDirection() == DirectionTypes::INOUT)
        {
            QString outputLine = "    assign " + mPort->port_->name() + " = " + assignmentForPort(mPort, DirectionTypes::OUT) + ";";

            output << outputLine << endl;
        }
        else if (mPort->port_->getDirection() == DirectionTypes::IN)
        {
            foreach (QSharedPointer<MetaPortAssignMent> mpa, mPort->assignments_)
            {
                QString outputLine = assignmentForWire(mpa, mPort->port_->name());

                if (!outputLine.isEmpty())
                {
                    output << outputLine << endl;
                }
            }
        }
    }

    output << endl;
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::assignmentForPort()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::assignmentForPort(QSharedPointer<MetaPort> mPort,
    DirectionTypes::Direction inOut)
{
    bool isInOutPort =
        (mPort->port_->getDirection() == inOut || mPort->port_->getDirection() == DirectionTypes::INOUT);

    // Use the default value of port, if no assignments exist.
    if (mPort->assignments_.size() < 1)
    {
        if (isInOutPort)
        {
            return mPort->defaultValue_;
        }

        return "";
    }

    QString assignmentString;
    
    if (mPort->assignments_.size() > 1)
    {
       assignmentString = "{";
    }

    foreach (QSharedPointer<MetaPortAssignMent> mpa, mPort->assignments_)
    {
        QString subAssignment;

        if (mpa->wire_)
        {
		    subAssignment = "<wireName>[<left>:<right>]";

		    QPair<QString,QString> connectionBounds = mpa->physicalBounds_;

		    // Use bounds only if they are not the same.
		    if (connectionBounds.first == connectionBounds.second)
		    {
                subAssignment.remove("[<left>:<right>]");
		    }
		    else
		    {
			    subAssignment.replace("<left>", connectionBounds.first);
			    subAssignment.replace("<right>", connectionBounds.second);
		    }

            // Write a wire as a connection.
            subAssignment.replace("<wireName>", mpa->wire_->name_);
        }
        else if (isInOutPort)
        {
            // If a default value is assigned to a physical port, it shall be used.
            subAssignment = mpa->defaultValue_;
        }

        if (!subAssignment.isEmpty())
        {
            assignmentString += subAssignment;

            if (mpa != mPort->assignments_.last())
            {
                assignmentString += ", ";
            }
        }
    }

    if (mPort->assignments_.size() > 1)
    {
        assignmentString += "}";
    }

    return assignmentString;
}

//-----------------------------------------------------------------------------
// Function: VerilogAssignmentWriter::assignmentForWire()
//-----------------------------------------------------------------------------
QString VerilogAssignmentWriter::assignmentForWire(QSharedPointer<MetaPortAssignMent> mpa, QString portName)
{
    QString assignmentString;

    if (mpa->wire_)
    {
        assignmentString = "    assign <wireName>[<left>:<right>] = <portName>;";

        QPair<QString,QString> connectionBounds = mpa->physicalBounds_;

        // Use bounds only if they are not the same.
        if (connectionBounds.first == connectionBounds.second)
        {
            assignmentString.remove("[<left>:<right>]");
        }
        else
        {
            assignmentString.replace("<left>", connectionBounds.first);
            assignmentString.replace("<right>", connectionBounds.second);
        }

        // Write names.
        assignmentString.replace("<wireName>", mpa->wire_->name_);
        assignmentString.replace("<portName>", portName);
    }

    return assignmentString;
}