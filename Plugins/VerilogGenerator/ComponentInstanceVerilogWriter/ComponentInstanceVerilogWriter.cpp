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

    CommentWriter descriptionWriter(componentInstance_.getDescription());
    descriptionWriter.write(outputStream);

    QString instanceString = "<component> <parameters><instanceName>(<portConnections>);";

    instanceString.replace("<component>", componentInstance_.getComponentRef().getName());
    instanceString.replace("<parameters>", parameterAssignments());
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
// Function: ComponentInstanceVerilogWriter::parameterAssignments()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::parameterAssignments() const
{
    if (componentInstance_.getConfigurableElementValues().isEmpty())
    {
        return "";
    }

    QStringList assignments;
    foreach(QString parameterName, componentInstance_.getConfigurableElementValues().keys())
    {
        QString assignment("\n    .<parameter>(<value>)");
        assignment.replace("<parameter>", parameterName);
        assignment.replace("<value>", componentInstance_.getConfElementValue(parameterName));
        assignments.append(assignment);
    }

    QString instanceParameters("#(<namesAndValues>)\n");
    instanceParameters.replace("<namesAndValues>", assignments.join(","));

    return instanceParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::createPortMaps()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::portConnections() const
{
    QStringList portAssignments;
    if (!sorter_.isNull())
    {
        foreach(QString portName, sorter_->sortedPortNames(referencedComponent_))
        {
            QString portAssignment = "\n    .<port>(<connection>)";
            portAssignment.replace("<port>", portName);
            portAssignment.replace("<connection>", connectionForPort(portName));

            portAssignments.append(portAssignment);
        }
    }

    return portAssignments.join(",");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::assignmentForPort()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::connectionForPort(QString portName) const
{
    if (notConnected(portName))
    {
        return portDefaultValue(portName);
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
// Function: ComponentInstanceVerilogWriter::portDefaultValue()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::portDefaultValue(QString const& portName) const
{
    QString defaultValue = " ";

    QSharedPointer<Port> port =  referencedComponent_->getPort(portName);
    if (port->getDirection() == General::IN && !port->getDefaultValue().isEmpty())
    {
        defaultValue = port->getDefaultValue();
    }
    return defaultValue;
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
