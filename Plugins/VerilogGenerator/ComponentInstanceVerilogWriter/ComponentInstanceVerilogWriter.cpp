//-----------------------------------------------------------------------------
// File: ComponentInstanceVerilogWriter.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 04.08.2014
//
// Description:
// Class for writing a component instance as a Verilog instance.
//-----------------------------------------------------------------------------

#include "ComponentInstanceVerilogWriter.h"

#include <Plugins/VerilogGenerator/CommentWriter/CommentWriter.h>
#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/BusInterface.h>

namespace
{
    const int ALL_BITS = -1; //! Indicator for connecting all bits in a connection.
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::ComponentInstanceVerilogWriter()
//-----------------------------------------------------------------------------
ComponentInstanceVerilogWriter::ComponentInstanceVerilogWriter(QSharedPointer<MetaInstance> instance,
	QSharedPointer<const PortSorter> sorter,
	bool useInterfaces) :
instance_(instance), 
sorter_(sorter),
useInterfaces_(useInterfaces)
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

    instanceString.replace("<component>", instance_->moduleName_);
    instanceString.replace("<parameters>", parameterAssignments());
    instanceString.replace("<instanceName>", formattedInstanceName());
    instanceString.replace("<portConnections>", portConnections());

    outputStream << indentation() << instanceString << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::nothingToWrite()
//-----------------------------------------------------------------------------
bool ComponentInstanceVerilogWriter::nothingToWrite() const
{
    return false;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::formattedInstanceName()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::formattedInstanceName() const
{
    QString instanceName = instance_->componentInstance_->getInstanceName();

    if (!instance_->componentInstance_->getConfigurableElementValues()->isEmpty())
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
	QString instanceParameters("#(\n<namesAndValues>)\n");

	QStringList assignments;

	if (instance_->parameters_.count() < 1)
	{
		return "";
	}

	foreach(QSharedPointer<Parameter> parameter, instance_->parameters_)
	{
        // If the parameters are not user resolved nor generated, then there cannot be any override by the design.
        if (parameter->getValueResolve() != "user" && parameter->getValueResolve() != "generated")
        {
            continue;
        }

		QString assignment(indentation().repeated(2) + ".<parameter>(<value>)");
		assignment.replace("<parameter>", parameter->name().leftJustified(20));
		assignment.replace("<value>", parameter->getValue());
		assignments.append(assignment);
	}

    instanceParameters.replace("<namesAndValues>", assignments.join(",\n"));
    return instanceParameters;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::portConnections()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::portConnections() const
{
    QStringList portAssignments;
    if (sorter_.isNull())
    {
		return "";
	}

	QString previousInterfaceName = "";

    foreach(QSharedPointer<MetaPort> mPort, instance_->ports_)
    {
        // If not applicable, skip.
        if (mPort->assignments_.size() < 1)
        {
            continue;
        }

        QSharedPointer<MetaPortAssignMent> mpa = mPort->assignments_.first();
        QString interfaceName;

        QString interfaceSeparatorLine = createInterfaceSeparator(interfaceName, previousInterfaceName);
        previousInterfaceName = interfaceName;

        QString portAssignment = interfaceSeparatorLine + indentation().repeated(2) + ".<port>(<connection>)";
        portAssignment.replace("<port>", mPort->port_->name().leftJustified(20));
        // TODO: use the list of port assignments rather than a single assignment,
        portAssignment.replace("<connection>", assignmentForPort(mpa));

        portAssignments.append(portAssignment);
    }

	/*foreach(QString portName, sorter_->sortedPortNames(instance_->component_))
    {
        // Seek for a port assignment.
        QSharedPointer<MetaPort> mPort = instance_->ports_.value(portName);

        // If none found, or is not applicable, skip.
        if (!mPort || mPort->assignments_.size() < 1)
        {
            continue;
        }

        QSharedPointer<MetaPortAssignMent> mpa = mPort->assignments_.first();
        
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
        instance_->component_->getInterfacesUsedByPort(portName);
		QString interfaceName;

		if (busInterfaces->size() == 1)
		{
			interfaceName = busInterfaces->first()->name();
		}
		else if (!busInterfaces->isEmpty())
		{
			interfaceName = QLatin1String("several");
		}
		else
		{
			interfaceName = QLatin1String("none");
		}

		QString interfaceSeparatorLine = createInterfaceSeparator(interfaceName, previousInterfaceName);
		previousInterfaceName = interfaceName;

		QString portAssignment = interfaceSeparatorLine + indentation().repeated(2) + ".<port>(<connection>)";
		portAssignment.replace("<port>", portName.leftJustified(20));
        // TODO: use the list of port assignments rather than a single assignment,
		portAssignment.replace("<connection>", assignmentForPort(mpa));

		portAssignments.append(portAssignment);
    }*/

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
QString ComponentInstanceVerilogWriter::assignmentForPort(QSharedPointer<MetaPortAssignMent> mpa) const
{
    QString assignment;

    if (mpa->wire_)
    {
		assignment = "<signalName>[<left>:<right>]";

		QPair<QString,QString> connectionBounds = mpa->bounds_;

		// Use bounds only if they are not the same.
		if (connectionBounds.first == connectionBounds.second)
		{
			assignment = "<signalName>";
		}
		else
		{
			assignment.replace("<left>", connectionBounds.first);
			assignment.replace("<right>", connectionBounds.second);
		}

        // Pry for the hierarchical ports.
        // TODO: Take into account the possibility of multiple hierarchical ports.
        QSharedPointer<MetaPort> hierPort;

        if (mpa->wire_->hierPorts_.size() > 0)
        {
            // Connect the physical port to a corresponding top port.
            hierPort = mpa->wire_->hierPorts_.first();
            assignment.replace("<signalName>", hierPort->port_->name());
        }
        else
        {
            // No hierarchical ports means we must write a wire as a connection.
            assignment.replace("<signalName>", mpa->wire_->name_);
        }
    }
    else
    {
        // If a default value is assigned to a physical port, it shall be used.
        assignment = mpa->defaultValue_;
        if (assignment.isEmpty())
        {
            assignment = " ";
        }
    }

    return assignment;
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
