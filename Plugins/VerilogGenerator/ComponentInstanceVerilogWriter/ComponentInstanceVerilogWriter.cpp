//-----------------------------------------------------------------------------
// File: ComponentInstanceVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceVerilogWriter.h"

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>

namespace
{
    const int ALL_BITS = -1; //! Indicator for connecting all bits in a connection.
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::ComponentInstanceVerilogWriter()
//-----------------------------------------------------------------------------
ComponentInstanceVerilogWriter::ComponentInstanceVerilogWriter(ComponentInstance const& instance,
    QSharedPointer<const Component> referencedComponent, QSharedPointer<const PortSorter> sorter) :
componentInstance_(instance), 
    referencedComponent_(referencedComponent), 
    sorter_(sorter)
{

}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::~ComponentInstanceVerilogWriter()
//-----------------------------------------------------------------------------
ComponentInstanceVerilogWriter::~ComponentInstanceVerilogWriter()
{

}
//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::write()
//-----------------------------------------------------------------------------
void ComponentInstanceVerilogWriter::write(QTextStream& outputStream) const
{
    if (nothingToWrite())
    {
        return;
    }

    CommentWriter descriptionWriter(referencedComponent_->getDescription());
    descriptionWriter.write(outputStream);

    QString instanceString = "<component> <instanceName>(<portConnections>);";
    instanceString.replace("<component>", componentInstance_.getComponentRef().getName());
    instanceString.replace("<instanceName>", componentInstance_.getInstanceName());
    instanceString.replace("<portConnections>", portConnections());

    outputStream << instanceString << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignPortForFullWidth()
//-----------------------------------------------------------------------------
void ComponentInstanceVerilogWriter::assignPortForFullWidth(QString const& instancePortName, 
    QString const& assignedConnection)
{
    portAssignments_.insert(instancePortName, General::PortBounds(assignedConnection, ALL_BITS, ALL_BITS));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignPortForRange()
//-----------------------------------------------------------------------------
void ComponentInstanceVerilogWriter::assignPortForRange(QString const& instancePortName, 
    QString const& assignedConnection, int leftBound, int rightBound)
{
    portAssignments_.insert(instancePortName, General::PortBounds(assignedConnection, leftBound, rightBound));
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool ComponentInstanceVerilogWriter::nothingToWrite() const
{
    return referencedComponent_.isNull();
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::createPortMaps()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::portConnections() const
{
    QStringList portMaps;
    if (!sorter_.isNull())
    {
        foreach(QString portName, sorter_->sortedPortNames(referencedComponent_))
        {
            QString portAssignment = "\n    .<port>(<connection>)";
            portAssignment.replace("<port>", portName);
            portAssignment.replace("<connection>", connectionForPort(portName));

            portMaps.append(portAssignment);
        }
    }

    return portMaps.join(",");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignmentForPort()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::connectionForPort(QString portName) const
{
    if (notConnected(portName))
    {
        return " ";
    }

    General::PortBounds connectionBounds = portAssignments_.value(portName);

    if (assignAllBitsInConnection(connectionBounds))
    {
        return connectionBounds.portName_;
    }

    QString assignment = "<signalName>[<bits>]";
    assignment.replace("<signalName>", connectionBounds.portName_);

    if (assignSingleBitInConnection(connectionBounds))
    {
        assignment.replace("<bits>", QString::number(connectionBounds.left_));
    }
    else
    {
        assignment.replace("<bits>", QString::number(connectionBounds.left_) + ":" +
            QString::number(connectionBounds.right_));
    }

    return assignment;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::notConnected()
//-----------------------------------------------------------------------------
bool ComponentInstanceVerilogWriter::notConnected(QString portName) const
{
    return !portAssignments_.contains(portName);
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignWholePort()
//-----------------------------------------------------------------------------
bool ComponentInstanceVerilogWriter::assignAllBitsInConnection(General::PortBounds const& signalBounds) const
{
    return signalBounds.left_ == ALL_BITS;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignSingleBit()
//-----------------------------------------------------------------------------
bool ComponentInstanceVerilogWriter::assignSingleBitInConnection(General::PortBounds const& signalBounds) const
{
    return  signalBounds.left_ == signalBounds.right_;
}
