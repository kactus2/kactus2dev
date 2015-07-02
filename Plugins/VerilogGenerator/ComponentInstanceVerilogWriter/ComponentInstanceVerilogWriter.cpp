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
    QSharedPointer<const Component> referencedComponent, QSharedPointer<const PortSorter> sorter,
    QSharedPointer<ExpressionFormatter> expressionFormatter) :
componentInstance_(instance), 
referencedComponent_(referencedComponent), 
sorter_(sorter),
expressionFormatter_(expressionFormatter)
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

    QString instanceString = "<component> <parameters><instanceName>(<portConnections>);";

    instanceString.replace("<component>", componentInstance_.getComponentRef().getName());
    instanceString.replace("<parameters>", parameterAssignments());
    instanceString.replace("<instanceName>", formattedInstanceName());
    instanceString.replace("<portConnections>", portConnections());

    outputStream << indentation() << instanceString << endl;
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
// Function: ComponentInstanceVerilogWriter::formattedInstanceName()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::formattedInstanceName() const
{
    QString instanceName = componentInstance_.getInstanceName();

    if (!componentInstance_.getConfigurableElementValues().isEmpty())
    {
        instanceName.prepend(indentation());
    }

    return instanceName;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::indentation()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::indentation() const
{
    return "    ";
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

    QString instanceParameters("#(\n<namesAndValues>)\n");

    QStringList assignments;
    foreach(QString parameterID, componentInstance_.getConfigurableElementValues().keys())
    {
        QString assignment(indentation().repeated(2) + ".<parameter>(<value>)");
        assignment.replace("<parameter>", expressionFormatter_->formatReferringExpression(parameterID).
            leftJustified(20));
        assignment.replace("<value>", expressionFormatter_->formatReferringExpression(
            componentInstance_.getConfElementValue(parameterID)));
        assignments.append(assignment);
    }

    instanceParameters.replace("<namesAndValues>", assignments.join(",\n"));
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
        QString previousInterfaceName = "";

        foreach(QString portName, sorter_->sortedPortNames(referencedComponent_))
        {
            QString interfaceName = referencedComponent_->getInterfaceNameForPort(portName);
            QString interfaceSeparatorLine = createInterfaceSeparator(interfaceName, previousInterfaceName);
            previousInterfaceName = interfaceName;

            QString portAssignment = interfaceSeparatorLine + indentation().repeated(2) + ".<port>(<connection>)";
            portAssignment.replace("<port>", portName.leftJustified(20));
            portAssignment.replace("<connection>", connectionForPort(portName));

            portAssignments.append(portAssignment);
        }
    }

    return portAssignments.join(",\n");
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::createInterfaceSeparator()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::createInterfaceSeparator(QString const& interfaceName, 
    QString const& previousInteface) const
{
    QString interfaceIntroduction = "";

    if (interfaceName.compare(previousInteface) != 0)
    {
        interfaceIntroduction.append(indentation().repeated(2));

        if (interfaceName == "none")
        {
            interfaceIntroduction.append("// These ports are not in any interface\n");
        }
        else if (interfaceName == "several")
        {
            interfaceIntroduction.append("// There ports are contained in many interfaces\n");       
        }
        else
        {
            interfaceIntroduction.append("// Interface: " + interfaceName + "\n");
        }
    }	
    
    // Begin first introduction on own line.
    if (previousInteface.isEmpty())
    {
        interfaceIntroduction.prepend("\n");
    }

    return interfaceIntroduction;
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
        defaultValue = expressionFormatter_->formatReferringExpression(port->getDefaultValue());
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
