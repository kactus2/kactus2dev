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

    outputStream << indentation() << instanceString << Qt::endl;
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
        return getDefaultValueAssignment(mPort);
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
// Function: ComponentInstanceVerilogWriter::getDefaultValueAssignment()
//-----------------------------------------------------------------------------
QString ComponentInstanceVerilogWriter::getDefaultValueAssignment(QSharedPointer<MetaPort> mPort) const
{
    if (mPort->port_->getDirection() == DirectionTypes::IN)
    {
        bool defaultValueOk = false;
        qint64 calculatedDefaultValue(mPort->defaultValue_.toULongLong(&defaultValueOk));
        if (!defaultValueOk)
        {
            return "";
        }

        QString defaultValue = mPort->port_->getDefaultValue();

        ComponentInstanceVerilogWriter::DefaultValueData defaultValueBasePair =
            getDefaultValueBits(defaultValue, mPort);

        int vectorWidth = defaultValueBasePair.bitvalue;
        int base = defaultValueBasePair.base;

        QString numberFormatLetter(QLatin1String("d"));
        if (base == 2)
        {
            numberFormatLetter = QLatin1String("b");
        }
        else if (base == 8)
        {
            numberFormatLetter = QLatin1String("o");
        }
        else if (base == 16)
        {
            numberFormatLetter = QLatin1String("h");
        }

        QString vectorWidthString = "";
        if (vectorWidth != 0)
        {
            vectorWidthString = QString::number(vectorWidth);
        }

        QString portName = mPort->port_->name();

        QString numberFormat("");
        if (vectorWidth != 0 || numberFormatLetter != QStringLiteral("d"))
        {
            numberFormat.append("'");
            if (defaultValueBasePair.isSigned)
            {
                numberFormat.append("s");
            }
            
            numberFormat.append(numberFormatLetter);
        }

        return vectorWidthString + numberFormat + QString::number(calculatedDefaultValue, base);
    }

    return "";
}

//-----------------------------------------------------------------------------
// Function: ComponentInstanceVerilogWriter::getDefaultValueBits()
//-----------------------------------------------------------------------------
ComponentInstanceVerilogWriter::DefaultValueData ComponentInstanceVerilogWriter::getDefaultValueBits(
    QString const& portDefaultValue, QSharedPointer<MetaPort> port) const
{
    QString portName = port->port_->name();

    bool calculateBitValue = true;
    quint64 bitValue = 0;
    int base = 0;
    bool isSigned = false;

    QRegularExpression numberExpression("\\d+");
    QRegularExpression digitExpression("(\\w|')+");

    QStringList matchList;

    QRegularExpressionMatchIterator matchIterator = digitExpression.globalMatch(portDefaultValue);
    while (matchIterator.hasNext())
    {
        QRegularExpressionMatch match = matchIterator.next();
        matchList.append(match.captured());
    }

    for (auto matchedWord : matchList)
    {
        if (matchedWord.contains("'"))
        {
            int markIndex = matchedWord.indexOf("'");

            QString bitSideString = matchedWord.left(markIndex);
            if (!bitSideString.isEmpty())
            {
                quint64 bitSide = bitSideString.toULongLong();
                if (bitSide > bitValue)
                {
                    bitValue = bitSide;
                    calculateBitValue = false;
                }
            }

            QString format = matchedWord.mid(markIndex + 1, 2).toLower();
            if (format.size() == 2)
            {
                bool newValueIsSigned = false;

                int newBase = 0;
                QString baseString = format.at(0);
                if (baseString == ("s"))
                {
                    baseString = format.at(1);
                    newValueIsSigned = true;
                }

                if (baseString == "b")
                {
                    newBase = 2;
                }
                else if (baseString == "o")
                {
                    newBase = 8;
                }
                else if (baseString == "d")
                {
                    newBase = 10;
                }
                else if (baseString == "h")
                {
                    newBase = 16;
                }

                if (newBase > base)
                {
                    base = newBase;
                    isSigned = newValueIsSigned;
                }
            }
        }
        else
        {
            QRegularExpressionMatch match = numberExpression.match(matchedWord);
            if (match.captured() == matchedWord && base < 10)
            {
                base = 10;
                calculateBitValue = false;
            }
        }
    }

    if (calculateBitValue)
    {
        bool boundsFirstOk = false;
        bool boundsSecondOk = false;
        qint64 boundsFirst(port->vectorBounds_.first.toULongLong(&boundsFirstOk));
        qint64 boundsSecond(port->vectorBounds_.second.toULongLong(&boundsSecondOk));

        quint64 vectorWidth(0);

        if (boundsFirstOk && boundsSecondOk)
        {
            vectorWidth = abs(boundsFirst - boundsSecond) + 1;
        }

        bitValue = vectorWidth;
    }

    if (base == 0)
    {
        if (bitValue < 8)
        {
            base = 2;
        }
        else if (bitValue < 16)
        {
            base = 10;
        }
        else
        {
            base = 16;
        }
    }

    ComponentInstanceVerilogWriter::DefaultValueData defaultData;
    defaultData.bitvalue = bitValue;
    defaultData.base = base;
    defaultData.isSigned = isSigned;

    return defaultData;
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
