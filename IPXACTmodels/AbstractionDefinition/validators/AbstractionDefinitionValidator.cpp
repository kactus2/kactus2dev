//-----------------------------------------------------------------------------
// AbstractionDefinition: AbstractionDefinitionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 04.12.2015
//
// Description:
// Validator for ipxact:AbstractionDefinition.
//-----------------------------------------------------------------------------

#include "AbstractionDefinitionValidator.h"

#include <KactusAPI/include/LibraryInterface.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <IPXACTmodels/AbstractionDefinition/AbstractionDefinition.h>
#include <IPXACTmodels/AbstractionDefinition/PortAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/TransactionalAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/WireAbstraction.h>
#include <IPXACTmodels/AbstractionDefinition/Packet.h>

#include <IPXACTmodels/common/Choice.h>
#include <IPXACTmodels/common/validators/ChoiceValidator.h>

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/common/Enumeration.h>
#include <IPXACTmodels/common/DirectionTypes.h>
#include <IPXACTmodels/common/TransactionalTypes.h>

#include <IPXACTmodels/common/validators/CellSpecificationValidator.h>
#include <IPXACTmodels/common/validators/TimingConstraintValidator.h>
#include <IPXACTmodels/common/validators/QualifierValidator.h>
#include <IPXACTmodels/AbstractionDefinition/validators/PacketValidator.h>

#include <IPXACTmodels/utilities/BusDefinitionUtils.h>

#include <QRegularExpression>
#include <QStringList>

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::AbstractionDefinitionValidator()
//-----------------------------------------------------------------------------
AbstractionDefinitionValidator::AbstractionDefinitionValidator(LibraryInterface* library,
    QSharedPointer<ExpressionParser> expressionParser) : 
library_(library), 
expressionParser_(expressionParser),
portsValidator_(library_, expressionParser_)
{

}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::validateInstantiation()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::validate(QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		return false;
	}

    if (!parameterValidator_)
    {
        parameterValidator_ = QSharedPointer<ParameterValidator>(
            new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(), 
                abstractionDefinition->getRevision()));
    }

	// The defined bus must exist in the library as a component.
    QSharedPointer<const BusDefinition> busDefinition = getBusDefinition(abstractionDefinition);
    if (!busDefinition)
    {
        return false;
    }

	// If this is an extension to another abstraction definition, it must exist.
    if (!hasValidExtend(abstractionDefinition) || !hasValidExtendPorts(abstractionDefinition))
	{
		return false;
	}

	// Any parameters must be valid.
	for (auto const& parameter : *abstractionDefinition->getParameters())
	{
		if (!parameterValidator_->hasValidValue(parameter))
		{
			return false;
		}
	}

	// Must have at least one logical port.
	if (abstractionDefinition->getLogicalPorts()->isEmpty())
	{
		return false;
	}

	// Collection of names of logical ports.
	QVector<QString> logicalNames;

	// Validate each logical port.
	for (auto const& portAbstraction : *abstractionDefinition->getLogicalPorts())
	{
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(portAbstraction->getLogicalName()))
        {
            return false;
        }

		if (!portsValidator_.validate(portAbstraction, abstractionDefinition, busDefinition))
		{
			return false;
		}

        // Logical name is valid: Mark it as used one!
        logicalNames.append(portAbstraction->getLogicalName());
	}

    if (!hasValidChoices(abstractionDefinition))
    {
        return false;
    }

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsIn(QVector<QString>& errors,
	QSharedPointer<AbstractionDefinition> abstractionDefinition)
{
    if (!parameterValidator_)
    {
        parameterValidator_ = QSharedPointer<ParameterValidator>(
            new ParameterValidator(expressionParser_, QSharedPointer<QList<QSharedPointer<Choice> > >(),
                abstractionDefinition->getRevision()));
    }

	// Must have a valid VLNV.
	if (!abstractionDefinition->getVlnv().isValid())
	{
		errors.append(QObject::tr("The VLNV %1 is invalid.").arg(abstractionDefinition->getVlnv().toString()));
	}

	// The defined bus must exist in the library as a component.
    QSharedPointer<const BusDefinition> busDefinition = getBusDefinition(abstractionDefinition);
    if (!busDefinition)
    {
        errors.append(QObject::tr("The bus definition %1 referenced in %2 is not found in the library.")
            .arg(abstractionDefinition->getBusType().toString(), abstractionDefinition->getVlnv().toString()));
    }

    QString context = QObject::tr("abstraction definition %1").arg(abstractionDefinition->getVlnv().toString());

    // If this is an extension to another abstraction definition, it must exist.
    findErrorsInExtend(errors, context, abstractionDefinition);
    
    // Any parameters must be valid.
	for (auto const& currentPara : *abstractionDefinition->getParameters())
	{
		parameterValidator_->findErrorsIn(errors, currentPara, context);
	}

	// Must have at least one logical port.
	if (abstractionDefinition->getLogicalPorts()->isEmpty())
	{
		errors.append(QObject::tr("Abstraction definition must define at least one port."));
	}

	// Collection of names of logical ports.
	QVector<QString> logicalNames;

	// Validate each logical port.
	for (auto const& port : *abstractionDefinition->getLogicalPorts())
    {	
        // Logical name must unique within the abstraction definition.
        if (logicalNames.contains(port->getLogicalName()))
        {
            errors.append(QObject::tr("Port name %1 is not unique within %2.").arg(port->getLogicalName(),
               context));
        }

        portsValidator_.findErrorsIn(errors, port, abstractionDefinition, busDefinition);

        logicalNames.append(port->getLogicalName());
	}

    if (auto choices = abstractionDefinition->getChoices(); !choices->isEmpty())
    {
        findErrorsInChoices(errors, context, choices);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidName(QString const& name) const
{
	QRegularExpression whiteSpaceExpression(QStringLiteral("^\\s*$"));

	if (name.isEmpty() || whiteSpaceExpression.match(name).hasMatch())
	{
		return false;
	}

	return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getBusDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<const BusDefinition> AbstractionDefinitionValidator::getBusDefinition(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<const Document> busDefinitionDocument = library_->getModelReadOnly(abstraction->getBusType());
    if (busDefinitionDocument)
    {
        QSharedPointer<const BusDefinition> busDefinition =
            busDefinitionDocument.dynamicCast<const BusDefinition>();

        return busDefinition;
    }

    return QSharedPointer<const BusDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidExtendPorts()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidExtendPorts(QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (extendAbstraction)
    {
        for (auto portAbstraction : *abstraction->getLogicalPorts())
        {
            QSharedPointer<PortAbstraction> extendPort = getMatchingExtendPort(portAbstraction, extendAbstraction);
            if (extendPort &&
                (*portAbstraction->getQualifier() != *extendPort->getQualifier() ||
                portAbstraction->getMatch() != extendPort->getMatch() ||
                ((portAbstraction->hasWire() && !extendWirePortIsValid(portAbstraction, extendPort)) ||
                (portAbstraction->hasTransactional() &&
                    !extendTransactionalPortAbstractionIsValid(portAbstraction, extendPort)))))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendWirePortIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendWirePortIsValid(QSharedPointer<PortAbstraction> abstractionPort,
    QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasWire())
    {
        return false;
    }

    QSharedPointer<WireAbstraction> portWire = abstractionPort->getWire();
    QSharedPointer<WireAbstraction> extendWire = extendPort->getWire();

    if ((extendWire->getMasterPort() && (!portWire->getMasterPort() ||
        portWire->getMasterPort()->getDirection() != extendWire->getMasterPort()->getDirection())) ||
        (extendWire->getSlavePort() && (!portWire->getSlavePort() ||
            portWire->getSlavePort()->getDirection() != extendWire->getSlavePort()->getDirection())))
    {
        return false;
    }

    for (auto extendSystem : *extendWire->getSystemPorts())
    {
        QSharedPointer<WirePort> matchingSystem;
        for (auto portSystem : *portWire->getSystemPorts())
        {
            if (portSystem->getSystemGroup() == extendSystem->getSystemGroup())
            {
                matchingSystem = portSystem;
                break;
            }
        }

        if (!matchingSystem || extendSystem->getDirection() != matchingSystem->getDirection())
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendTransactionalPortAbstractionIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendTransactionalPortAbstractionIsValid(
    QSharedPointer<PortAbstraction> abstractionPort, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasTransactional())
    {
        return false;
    }

    QSharedPointer<TransactionalAbstraction> portTransactional = abstractionPort->getTransactional();
    QSharedPointer<TransactionalAbstraction> extendTransactional = extendPort->getTransactional();

    if (extendTransactional->getMasterPort() &&
        !extendTransactionalPortIsValid(portTransactional->getMasterPort(), extendTransactional->getMasterPort()))
    {
        return false;
    }

    if (extendTransactional->getSlavePort() &&
        !extendTransactionalPortIsValid(portTransactional->getSlavePort(), extendTransactional->getSlavePort()))
    {
        return false;
    }

    for (auto extendSystem : *extendTransactional->getSystemPorts())
    {
        QSharedPointer<TransactionalPort> portSystem;
        for (auto transactionalSystemPort : *portTransactional->getSystemPorts())
        {
            if (transactionalSystemPort->getSystemGroup() == extendSystem->getSystemGroup())
            {
                portSystem = transactionalSystemPort;
                break;
            }
        }

        if (!extendTransactionalPortIsValid(portSystem, extendSystem))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::extendTransactionalPortIsValid()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::extendTransactionalPortIsValid(QSharedPointer<TransactionalPort> port,
    QSharedPointer<TransactionalPort> extendPort) const
{
    if (!port)
    {
        return false;
    }

    QSharedPointer<Protocol> portProtocol = port->getProtocol();
    QSharedPointer<Protocol> extendPortProtocol = extendPort->getProtocol();

    return port->getInitiative() == extendPort->getInitiative() &&
        port->getKind() == extendPort->getKind() && port->getBusWidth() == extendPort->getBusWidth() &&
        ((!portProtocol && !extendPortProtocol) || (portProtocol && extendPortProtocol &&
        portProtocol->getProtocolType() == extendPortProtocol->getProtocolType() &&
            portProtocol->getPayloadName() == extendPortProtocol->getPayloadName() &&
            portProtocol->getPayloadType() == extendPortProtocol->getPayloadType() &&
            portProtocol->getPayloadExtension() == extendPortProtocol->getPayloadExtension()));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getMatchingExtendPort()
//-----------------------------------------------------------------------------
QSharedPointer<PortAbstraction> AbstractionDefinitionValidator::getMatchingExtendPort(
    QSharedPointer<PortAbstraction> logicalPort, QSharedPointer<AbstractionDefinition> extendAbstraction) const
{
    for (auto extendPort : *extendAbstraction->getLogicalPorts())
    {
        if (extendPort->name() == logicalPort->name())
        {
            return extendPort;
        }
    }

    return QSharedPointer<PortAbstraction>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidExtend()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidExtend(QSharedPointer<AbstractionDefinition> abstraction) const
{
    if (!abstraction->getExtends().isEmpty() && (!library_->contains(abstraction->getExtends()) ||
        !busTypeDefinesExtendedAbstractionBusType(abstraction)))
    {
        return false;
    }
    else
    {
        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::busTypeDefinesExtendedAbstractionBusType()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::busTypeDefinesExtendedAbstractionBusType(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (!extendAbstraction)
    {
        return false;
    }

    return busTypeIsExtendedFromExtendBusType(getBusDefinition(abstraction), getBusDefinition(extendAbstraction));
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getExtendedAbstractionDefinition()
//-----------------------------------------------------------------------------
QSharedPointer<AbstractionDefinition> AbstractionDefinitionValidator::getExtendedAbstractionDefinition(
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    if (abstraction->getExtends().isValid())
    {
        QSharedPointer<Document> extendDocument = library_->getModel(abstraction->getExtends());
        if (extendDocument)
        {
            QSharedPointer<AbstractionDefinition> extendAbstraction =
                extendDocument.dynamicCast<AbstractionDefinition>();
            if (extendAbstraction)
            {
                return extendAbstraction;
            }
        }
    }

    return QSharedPointer<AbstractionDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::busTypeIsExtendedFrom()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::busTypeIsExtendedFromExtendBusType(
    QSharedPointer<const BusDefinition> busType, QSharedPointer<const BusDefinition> extendBustype) const
{
    if (busType == extendBustype)
    {
        return true;
    }

    QSharedPointer<const BusDefinition> busTypeExtend = getBusTypeExtend(busType);
    if (busTypeExtend && busTypeIsExtendedFromExtendBusType(busTypeExtend, extendBustype))
    {
        return true;
    }

    return false;
}


//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::getBusTypeExtend()
//-----------------------------------------------------------------------------
QSharedPointer<const BusDefinition> AbstractionDefinitionValidator::getBusTypeExtend(
    QSharedPointer<const BusDefinition> busType) const
{
    if (!busType->getExtends().isEmpty() && busType->getExtends().isValid() &&
        busType->getExtends().getType() == VLNV::BUSDEFINITION)
    {
        QSharedPointer<const Document> extendDocument = library_->getModelReadOnly(busType->getExtends());
        if (extendDocument)
        {
            QSharedPointer<const BusDefinition> extendBusType = extendDocument.dynamicCast<const BusDefinition>();
            if (extendBusType)
            {
                return extendBusType;
            }
        }
    }

    return QSharedPointer<const BusDefinition>();
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtend()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtend(QVector<QString>& errors, QString const& context,
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    VLNV extendVLNV = abstraction->getExtends();
    if (!extendVLNV.isEmpty())
    {
        if (!library_->contains(extendVLNV))
        {
            errors.append(QObject::tr("The bus definition %1 extended in %2 is not found in the library")
                .arg(extendVLNV.toString(), context));
        }
        else
        {
            if (!busTypeDefinesExtendedAbstractionBusType(abstraction))
            {
                errors.append(
                    QObject::tr("The bus definition %1 extended in %2 does not define extended abstraction definition"
                        " bus type").arg(abstraction->getBusType().toString(), context));
            }

            findErrorsInExtendPorts(errors, context, abstraction);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendPorts()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendPorts(QVector<QString>& errors, QString const& context,
    QSharedPointer<AbstractionDefinition> abstraction) const
{
    QSharedPointer<AbstractionDefinition> extendAbstraction = getExtendedAbstractionDefinition(abstraction);
    if (extendAbstraction)
    {
        for (auto portAbstraction : *abstraction->getLogicalPorts())
        {
            QSharedPointer<PortAbstraction> extendPort = getMatchingExtendPort(portAbstraction, extendAbstraction);
            if (extendPort)
            {
                if (*portAbstraction->getQualifier() != *extendPort->getQualifier())
                {
                    errors.append(QObject::tr("The qualifier of extended port %1 in %2 cannot be edited")
                        .arg(extendPort->name(), context));
                }

                if (portAbstraction->getMatch() != extendPort->getMatch())
                {
                    errors.append(QObject::tr("Match of extended port %1 in %2 cannot be edited")
                        .arg(extendPort->name(), context));
                }

                if (portAbstraction->hasWire())
                {
                    findErrorsInExtendWireAbstraction(errors, context, portAbstraction, extendPort);
                }
                if (portAbstraction->hasTransactional())
                {
                    findErrorsInExtendTransactionalAbstraction(errors, context, portAbstraction, extendPort);
                }
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendWireAbstraction(QVector<QString>& errors, QString const& context,
    QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasWire())
    {
        errors.append(QObject::tr("The extended port %1 in %2 is not wire").arg(extendPort->name(), context));
        return;
    }

    QSharedPointer<WireAbstraction> portWire = port->getWire();
    QSharedPointer<WireAbstraction> extendWire = extendPort->getWire();

    findErrorsInExtendWirePort(errors, context, portWire->getMasterPort(), extendWire->getMasterPort(),
        extendPort->name(), General::interfaceMode2Str(General::MASTER));
    findErrorsInExtendWirePort(errors, context, portWire->getSlavePort(), extendWire->getSlavePort(),
        extendPort->name(), General::interfaceMode2Str(General::SLAVE));

    for (auto extendSystem : *extendWire->getSystemPorts())
    {
        QSharedPointer<WirePort> matchingSystem;
        for (auto portSystem : *portWire->getSystemPorts())
        {
            if (portSystem->getSystemGroup() == extendSystem->getSystemGroup())
            {
                matchingSystem = portSystem;
                break;
            }
        }

        if (!matchingSystem)
        {
            errors.append(QObject::tr("The extended system %1 port %2 in %3 was not found")
                .arg(extendSystem->getSystemGroup(), extendPort->name(), context));
        }
        else if (matchingSystem->getDirection() != extendSystem->getDirection())
        {
            errors.append(QObject::tr("The direction of extended system %1 port %2 in %3 cannot be edited")
                .arg(extendSystem->getSystemGroup(), extendPort->name(), context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendWirePort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendWirePort(QVector<QString>& errors, QString const& context,
    QSharedPointer<WirePort> port, QSharedPointer<WirePort> extendPort, QString const& portName,
    QString const& modeName) const
{
    if (extendPort)
    {
        if (!port)
        {
            errors.append(QObject::tr("The extended %1 port %2 in %3 was not found")
                .arg(modeName, portName, context));
        }
        else if (port->getDirection() != extendPort->getDirection())
        {
            errors.append(QObject::tr("The direction of extended %1 port %2 in %3 cannot be edited")
                .arg(modeName, portName, context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendTransactionalAbstraction(QVector<QString>& errors,
    QString const& context, QSharedPointer<PortAbstraction> port, QSharedPointer<PortAbstraction> extendPort) const
{
    if (!extendPort->hasTransactional())
    {
        errors.append(QObject::tr("The extended port %1 in %2 is not transactional")
            .arg(extendPort->name(), context));
        return;
    }

    QSharedPointer<TransactionalAbstraction> portTransactional = port->getTransactional();
    QSharedPointer<TransactionalAbstraction> extendTransactional = extendPort->getTransactional();

    QString masterContext = QObject::tr("extended %1 port %2 in %3")
        .arg(General::interfaceMode2Str(General::MASTER), port->name(), context);
    QString slaveContext = QObject::tr("extended %1 port %2 in %3")
        .arg(General::interfaceMode2Str(General::SLAVE), port->name(), context);

    findErrorsInExtendTransactionalPort(errors, masterContext, portTransactional->getMasterPort(),
        extendTransactional->getMasterPort());
    findErrorsInExtendTransactionalPort(errors, slaveContext, portTransactional->getSlavePort(),
        extendTransactional->getSlavePort());

    for (auto extendSystem : *extendTransactional->getSystemPorts())
    {
        QSharedPointer<TransactionalPort> portSystem;
        for (auto transactionalSystemPort : *portTransactional->getSystemPorts())
        {
            if (transactionalSystemPort->getSystemGroup() == extendSystem->getSystemGroup())
            {
                portSystem = transactionalSystemPort;
                break;
            }
        }

        QString systemContext = QObject::tr("extended %1 %2 port %3 in %4").arg(
            General::interfaceMode2Str(General::SYSTEM), portSystem->getSystemGroup(),  port->name(), context);
        findErrorsInExtendTransactionalPort(errors, systemContext, portSystem, extendSystem);
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInExtendTransactionalPort(QVector<QString>& errors,
    QString const& context, QSharedPointer<TransactionalPort> port, QSharedPointer<TransactionalPort> extendPort)
    const
{
    if (!extendPort)
    {
        return;
    }

    if (!port)
    {
        errors.append(QObject::tr("The %1 was not found").arg(context));
    }

    if (port->getInitiative() != extendPort->getInitiative())
    {
        errors.append(QObject::tr("The initiative of %1 cannot be edited").arg(context));
    }
    if (port->getKind() != extendPort->getKind())
    {
        errors.append(QObject::tr("The kind of %1 cannot be edited").arg(context));
    }
    if (port->getBusWidth() != extendPort->getBusWidth())
    {
        errors.append(QObject::tr("The bus width of %1 cannot be edited").arg(context));
    }

    QSharedPointer<Protocol> portProtocol = port->getProtocol();
    QSharedPointer<Protocol> extendPortProtocol = extendPort->getProtocol();

    if ((portProtocol && !extendPortProtocol) || (!portProtocol && extendPortProtocol))
    {
        errors.append(QObject::tr("The %1 does not contain protocol data").arg(context));
    }
    else if (portProtocol && extendPortProtocol)
    {
        if (portProtocol->getProtocolType() != extendPortProtocol->getProtocolType() ||
            (portProtocol->getProtocolType() == QLatin1String("custom") &&
            extendPortProtocol->getProtocolType() == QLatin1String("custom") &&
            portProtocol->getCustomProtocolType() != extendPortProtocol->getCustomProtocolType()))
        {
            errors.append(QObject::tr("The protocol type of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadName() != extendPortProtocol->getPayloadName())
        {
            errors.append(QObject::tr("The payload name of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadType() != extendPortProtocol->getPayloadType())
        {
            errors.append(QObject::tr("The payload type of %1 cannot be edited").arg(context));
        }
        if (portProtocol->getPayloadExtension() != extendPortProtocol->getPayloadExtension())
        {
            errors.append(QObject::tr("The payload extension of %1 cannot be edited").arg(context));
        }
    }
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::hasValidChoices()
//-----------------------------------------------------------------------------
bool AbstractionDefinitionValidator::hasValidChoices(QSharedPointer<AbstractionDefinition> abstraction) const
{
    auto choices = abstraction->getChoices();

    if (choices->isEmpty())
    {
        return true;
    }

    if (abstraction->getRevision() != Document::Revision::Std22)
    {
        return false;
    }

    ChoiceValidator choiceValidator(expressionParser_);

    for (auto const& choice : *choices)
    {
        if (!choiceValidator.validate(choice))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AbstractionDefinitionValidator::findErrorsInChoices()
//-----------------------------------------------------------------------------
void AbstractionDefinitionValidator::findErrorsInChoices(QVector<QString>& errors, QString const& context,
    QSharedPointer<QList<QSharedPointer<Choice> > >choices) const
{
    ChoiceValidator choiceValidator(expressionParser_);

    for (auto const& choice : *choices)
    {
        choiceValidator.findErrorsIn(errors, choice, context);
    }
}
