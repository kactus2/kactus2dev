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
	QSharedPointer<ExpressionParser> expressionParser,
	QSharedPointer<ExpressionFormatter> expressionFormatter) :
instance_(instance), 
sorter_(sorter),
expressionParser_(expressionParser),
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

	QList<QSharedPointer<Parameter> > parameters = QList<QSharedPointer<Parameter> >(*instance_->referencedComponent_->getParameters());
	parameters.append(*instance_->componentInstantiation_->getParameters());

	foreach(QSharedPointer<ModuleParameter> parameter, *instance_->componentInstantiation_->getModuleParameters())
	{
		parameters.append(parameter);
	}

	if (parameters.count() < 1)
	{
		return "";
	}

	foreach(QSharedPointer<Parameter> parameter, parameters)
	{
		if (parameter->getValueResolve() != "user")
		{
			continue;
		}

		QString paraValue = parameter->getValue();

		foreach(QSharedPointer<ConfigurableElementValue> cev, *instance_->componentInstance_->getConfigurableElementValues())
		{
			if (cev->getReferenceId() == parameter->getValueId())
			{
				paraValue = cev->getConfigurableValue();
				break;
			}
		}

		QString assignment(indentation().repeated(2) + ".<parameter>(<value>)");
		assignment.replace("<parameter>", 
			expressionParser_->parseExpression(parameter->name()).leftJustified(20));
		assignment.replace("<value>", 
			expressionFormatter_->formatReferringExpression(paraValue));
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

	foreach(QString portName, sorter_->sortedPortNames(instance_->referencedComponent_))
	{
		QSharedPointer<QList<QSharedPointer<BusInterface> > > busInterfaces =
			instance_->referencedComponent_->getInterfacesUsedByPort(portName);
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
		portAssignment.replace("<connection>", assignmentForPort(portName));

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
QString ComponentInstanceVerilogWriter::assignmentForPort(QString portName) const
{
    QString assignment;

    if (instance_->tieOffAssignments_.contains(portName))
    {
        assignment = expressionParser_->parseExpression(instance_->tieOffAssignments_.value(portName));
        if (assignment.isEmpty())
        {
            assignment = " ";
        }
    }
    else
    {
		QSharedPointer<GenerationPortAssignMent> gab = instance_->portAssignments_.value(portName);
		
		if (gab)
		{
			assignment = "<signalName>[<left>:<right>]";

			QPair<QString,QString> connectionBounds = gab->bounds;

			if (connectionBounds.first.isEmpty() || connectionBounds.second.isEmpty())
			{
				assignment = "<signalName>";
			}
			else
			{
				assignment.replace("<left>", connectionBounds.first);
				assignment.replace("<right>", connectionBounds.second);
			}

			if (gab->wire)
			{
				if (gab->wire->ports.size() > 1)
				{
					assignment.replace("<signalName>", gab->wire->name);
				}
				else
				{
					return "";
				}
			}
			else
			{
				assignment.replace("<signalName>", gab->otherName);
			}
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
