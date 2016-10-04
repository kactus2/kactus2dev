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
ComponentInstanceVerilogWriter::ComponentInstanceVerilogWriter(QSharedPointer<GenerationInstance> instance,
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

    instanceString.replace("<component>", instance_->componentInstance_->getComponentRef()->getName());
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

	if (instance_->parameters.count() < 1)
	{
		return "";
	}

	foreach(QSharedPointer<Parameter> parameter, instance_->parameters)
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
// Function: ComponentInstanceVerilogWriter::createPortMaps()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::portConnections() const
{
    QStringList portAssignments;
    if (sorter_.isNull())
    {
		return "";
	}

	QString previousInterfaceName = "";

    if (useInterfaces_)
    {
        foreach(QSharedPointer<GenerationInterfaceAssignment> gifa, instance_->interfaceAssignments_)
        {
            if (gifa->interConnection_->topInterface_)
            {
                portAssignments.append( "." + gifa->interface_->name + "(" + gifa->interConnection_->topInterface_->name + ")");
            }
            else
            {
                portAssignments.append( "." + gifa->interface_->name + "("+ gifa->interConnection_->name + ")");
            }
        }
    }

	foreach(QString portName, sorter_->sortedPortNames(instance_->component->component))
    {
        // Seek for a port assignment.
        QSharedPointer<GenerationPortAssignMent> gpa = instance_->portAssignments_.value(portName);

        // If none found, or is not applicable, skip.
        if (!gpa || (useInterfaces_ && !gpa->adhoc))
        {
            continue;
        }
        
        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
        instance_->component->component->getInterfacesUsedByPort(portName);
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
		portAssignment.replace("<connection>", assignmentForPort(gpa));

		portAssignments.append(portAssignment);
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
QString ComponentInstanceVerilogWriter::assignmentForPort(QSharedPointer<GenerationPortAssignMent> gab) const
{
    QString assignment;

	// If an tied value is assigned to a physical port, it shall be used.
    if (!gab->tieOff.isEmpty())
    {
		// Format its value as needed.
        assignment = gab->tieOff;
        if (assignment.isEmpty())
        {
            assignment = " ";
        }
    }
    else
    {
		assignment = "<signalName>[<left>:<right>]";

		QPair<QString,QString> connectionBounds = gab->bounds;

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

		// Connect the physical port to either corresponding top port or a wire.
		// If neither is available, return empty.
		if (!gab->topPort.isEmpty())
		{
			assignment.replace("<signalName>", gab->topPort);
		}
		else if (gab->wire)
		{
			assignment.replace("<signalName>", gab->wire->name);
		}
		else
		{
			return "";
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
