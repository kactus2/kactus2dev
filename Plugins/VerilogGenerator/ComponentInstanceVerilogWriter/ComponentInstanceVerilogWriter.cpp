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
    QString instanceString = "<component> <parameters><instanceName>(<portConnections>);";

    instanceString.replace("<component>", instance_->getModuleName());
    instanceString.replace("<parameters>", parameterAssignments());
    instanceString.replace("<instanceName>", formattedInstanceName());
    instanceString.replace("<portConnections>", portConnections());

    outputStream << indentation() << instanceString << endl;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::formattedInstanceName()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::formattedInstanceName() const
{
    QString instanceName = instance_->getComponentInstance()->getInstanceName();

    if (!instance_->getParameters()->isEmpty())
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
    QList<QSharedPointer<Parameter> > parameterToWrite;

    foreach(QSharedPointer<Parameter> parameter, *instance_->getParameters())
    {
        // If the parameters are not user resolved nor generated, then there cannot be any override by the design.
        if (parameter->getValueResolve() == "user" || parameter->getValueResolve() == "generated")
        {
            parameterToWrite.append(parameter);
        }
    }

    if (parameterToWrite.isEmpty())
    {
        return "";
    }

	QString instanceParameters("#(\n<namesAndValues>)\n");

	QStringList assignments;

	foreach(QSharedPointer<Parameter> parameter, parameterToWrite)
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

    // Pick the ports in sorted order.
    QList<QSharedPointer<Port> > ports = sorter_->sortedPorts(instance_->getComponent());

    foreach(QSharedPointer<Port> cPort, ports)
    {
        if (cPort->getDirection() == DirectionTypes::DIRECTION_PHANTOM)
        {
            ports.removeOne(cPort);
        }
    }

    foreach(QSharedPointer<Port> cPort, ports)
    {
        QSharedPointer<MetaPort> mPort = instance_->getPorts()->value(cPort->name());

        if (!mPort)
        {
            // TODO: error
            continue;
        }

        QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
            instance_->getComponent()->getInterfacesUsedByPort(cPort->name());
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
        portAssignment.replace("<port>", mPort->port_->name().leftJustified(20));
        portAssignment.replace("<connection>", assignmentForInstancePort(mPort));

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
QString ComponentInstanceVerilogWriter::assignmentForInstancePort(QSharedPointer<MetaPort> mPort) const
{
    // Use the default value of port, if no assignments exist.
    if (mPort->upAssignments_.size() < 1)
    {
        if (mPort->port_->getDirection() == DirectionTypes::IN)
        {
            return mPort->defaultValue_;
        }

        return "";
    }

    // In case of an inout port, connect it directly the first encountered wire.
    if (mPort->port_->getDirection() == DirectionTypes::INOUT)
    {
        foreach (QSharedPointer<MetaPortAssignment> mpa, mPort->upAssignments_)
        {
            if (mpa->wire_)
            {
                // If it is hierarchical, use the corresponding hierarchical port instead.
                if (mpa->wire_->hierPorts_.size() > 0)
                {
                    return mpa->wire_->hierPorts_.first()->port_->name();
                }

                return mpa->wire_->name_;
            }
        }

        return "";
    }

    return instance_->getComponentInstance()->getInstanceName() + "_" + mPort->port_->name();
}