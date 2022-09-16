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
#include <Plugins/VerilogImport/VerilogSyntax.h>

#include <IPXACTmodels/Component/Port.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <QRegularExpression>

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

    instanceString.replace("<component>", VerilogSyntax::legalizeName(instance_->getModuleName()));
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
    QString instanceName = VerilogSyntax::legalizeName(instance_->getComponentInstance()->getInstanceName());

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

        if (!mPort || mPort->isTransactional_)
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
            QRegularExpression re(R"""(^(\b((\d+'(s|S){0,1}(b|h|o|d|B|H|O|D))[0-9xzXZa-fA-F_]+))|(\B(('(s|S){0,1}(b|h|o|d|B|H|O|D))[0-9xzXZa-fA-F_]+))$)""");
            QRegularExpressionMatch match = re.match(mPort->port_->getDefaultValue().trimmed());
            if (match.hasMatch()) {
                // If it's canonical Number Literals, keep it as is.
                return mPort->port_->getDefaultValue();
            }
            else
            {
                // Get the port width by mPort->vectorBounds_
                // NOTE: mPort->width_ and mPort->vectorBounds_ may empty
                bool boundsFirstOk = false;
                bool boundsSecondOk = false;
                bool defaultValueOk = false;
                qint64 boundsFirst(mPort->vectorBounds_.first.toULongLong(&boundsFirstOk));
                qint64 boundsSecond(mPort->vectorBounds_.second.toULongLong(&boundsSecondOk));
                qint64 defaultValue(mPort->defaultValue_.toULongLong(&defaultValueOk));
                qint64 vectorWidth(0);
                if (defaultValueOk)
                {
                    if (mPort->vectorBounds_.first == mPort->vectorBounds_.second)
                    {
                        // If they are the same, the vector width is 1.
                        vectorWidth = 1;
                    }
                    else
                    if (boundsFirstOk && boundsSecondOk && boundsFirst > boundsSecond)
                    {
                        // Calculate width based on legal bounds.
                        vectorWidth = abs(boundsFirst - boundsSecond) + 1;
                    }

                    if (vectorWidth > 0)
                    {
                        // At this point, the conversion is successful.
                        if (vectorWidth < 8)
                        {
                            // If the width is less than 8 bits, use the 'b format
                            return QString::number(vectorWidth) + "'b" + QString::number(defaultValue, 2);
                        }
                        else
                        {
                            // If the width is greater than 8 bits, use the 'h format
                            return QString::number(vectorWidth) + "'h" + QString::number(defaultValue, 16);
                        }
                    }
                }
            }
            // If it still doesn't work, use the calculated value.
            return mPort->defaultValue_;
        }

        return "";
    }

    // In case of an inout port, connect it directly the first encountered wire.
    if (mPort->port_->getDirection() == DirectionTypes::INOUT)
    {
        return getInOutAssignment(mPort);
    }

    return VerilogSyntax::legalizeName(instance_->getComponentInstance()->getInstanceName() + "_"
        + mPort->port_->name());
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::getInOutAssignment()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::getInOutAssignment(QSharedPointer<MetaPort> mPort) const
{
    // Search for the first port assignment with a wire.

    QStringList assignmentRows;
    for (auto assignment : mPort->upAssignments_)
    {
        QSharedPointer<MetaWire> assignmentWire = assignment->wire_;

        if (assignmentWire)
        {
            if (assignmentWire->hierPorts_.size() < 1)
            {
                assignmentRows.append(assignment->wire_->name_);
            }
            else
            {
                QString portName = mPort->port_->name();

                QPair<QSharedPointer<MetaPort>, QSharedPointer<MetaPortAssignment> > hierarchicalPortAndAssignment
                    = getConnectedHierarchicalPort(mPort, assignment, assignmentWire);

                QSharedPointer<MetaPort> hierarchicalPort = hierarchicalPortAndAssignment.first;
                QSharedPointer<MetaPortAssignment> hierarchicalAssignment = hierarchicalPortAndAssignment.second;
                if (hierarchicalPort && hierarchicalAssignment)
                {
                    QString bounds;

                    // If the assigned bounds are the same, different notation is needed.
                    if (hierarchicalAssignment->physicalBounds_.first ==
                        hierarchicalAssignment->physicalBounds_.second)
                    {
                        // If the vector bounds are not the same, use index instead.
                        if (hierarchicalPort->vectorBounds_.first != hierarchicalPort->vectorBounds_.second)
                        {
                            bounds = "[<left>]";

                            bounds.replace("<left>", hierarchicalAssignment->physicalBounds_.first);
                        }

                        // Else no bounds are needed.
                    }
                    else
                    {
                        // If the assigned bounds differ, they must be generated.
                        bounds = "[<left>:<right>]";

                        bounds.replace("<left>", hierarchicalAssignment->physicalBounds_.first);
                        bounds.replace("<right>", hierarchicalAssignment->physicalBounds_.second);
                    }

                    QString portAssignment(hierarchicalPort->port_->name() + bounds);
                    assignmentRows.append(portAssignment);
                }
            }
        }
    }

    if (assignmentRows.isEmpty())
    {
        return QString("");
    }
    else
    {
        return assignmentRows.first();
    }
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::getConnectedHierarchicalPort()
//-----------------------------------------------------------------------------
QPair<QSharedPointer<MetaPort>, QSharedPointer<MetaPortAssignment> > ComponentInstanceVerilogWriter::
    getConnectedHierarchicalPort(QSharedPointer<MetaPort> metaPort,
        QSharedPointer<MetaPortAssignment> metaAssignment, QSharedPointer<MetaWire> assignmentWire) const
{
    QPair<QSharedPointer<MetaPort>, QSharedPointer<MetaPortAssignment> > portAssignment;

    QString portName = metaPort->port_->name();

    QPair<QString, QString> portBounds = metaAssignment->logicalBounds_;
    qint64 portLeft = portBounds.first.toLongLong();
    qint64 portRight = portBounds.second.toLongLong();

    if (portRight < portLeft)
    {
        qint64 temporary = portLeft;
        portLeft = portRight;
        portRight = temporary;
    }

    qint64 portWidth = abs(portLeft - portRight) + 1;

    for (auto comparisonPort : assignmentWire->hierPorts_)
    {
        QString comparisonName = comparisonPort->port_->name();

        QSharedPointer<MetaPortAssignment> comparisonAssignment =
            getHierarchicalPortAssignmentConnectedToWire(comparisonPort, assignmentWire);
        if (comparisonAssignment)
        {
            QPair<QString, QString> comparisonBounds = comparisonAssignment->logicalBounds_;

            qint64 comparisonLeft = comparisonBounds.first.toLongLong();
            qint64 comparisonRight = comparisonBounds.second.toLongLong();
            if (comparisonRight < comparisonLeft)
            {
                qint64 temporary = comparisonLeft;
                comparisonLeft = comparisonRight;
                comparisonRight = temporary;
            }

            qint64 comparisonWidth = abs(comparisonLeft - comparisonRight) + 1;

            if (portLeft >= comparisonLeft && portLeft <= comparisonRight && portWidth <= comparisonWidth)
            {
                portAssignment.first = comparisonPort;
                portAssignment.second = comparisonAssignment;

                break;
            }
        }
    }

    return portAssignment;
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::portIsConnectedToWire()
//-----------------------------------------------------------------------------
QSharedPointer<MetaPortAssignment> ComponentInstanceVerilogWriter::getHierarchicalPortAssignmentConnectedToWire(
    QSharedPointer<MetaPort> hierarchicalPort, QSharedPointer<MetaWire> assignedWire) const
{
    for (auto comparisonAssignment : hierarchicalPort->downAssignments_)
    {
        if (comparisonAssignment->wire_ == assignedWire)
        {
            return comparisonAssignment;
        }
    }

    return QSharedPointer<MetaPortAssignment>();
}
