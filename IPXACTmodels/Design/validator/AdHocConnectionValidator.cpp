//-----------------------------------------------------------------------------
// File: AdHocConnectionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 19.01.2016
//
// Description:
// Validator for the ipxact:adHocConnection.
//-----------------------------------------------------------------------------

#include "AdHocConnectionValidator.h"

#include <IPXACTmodels/Component/Component.h>

#include <IPXACTmodels/Design/AdHocConnection.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::AdHocConnectionValidator()
//-----------------------------------------------------------------------------
AdHocConnectionValidator::AdHocConnectionValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
parser_(parser),
libraryHandler_(library)
{

}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::~AdHocConnectionValidator()
//-----------------------------------------------------------------------------
AdHocConnectionValidator::~AdHocConnectionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::changeComponentInstances()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::changeComponentInstances(
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances)
{
    availableComponentInstances_ = newInstances;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::validate()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::validate(QSharedPointer<AdHocConnection> connection) const
{
    return hasValidName(connection) && hasValidIsPresent(connection->getIsPresent()) &&
        hasValidTiedValue(connection) && hasValidPortReferences(connection);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::hasValidName(QSharedPointer<AdHocConnection> connection) const
{
    QRegularExpression whiteSpaceExpression;
    whiteSpaceExpression.setPattern(QStringLiteral("^\\s*$"));
    QRegularExpressionMatch whiteSpaceMatch = whiteSpaceExpression.match(connection->name());

    if (connection->name().isEmpty() || whiteSpaceMatch.hasMatch())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::hasValidIsPresent(std::string const& isPresent) const
{
    if (!isPresent.empty())
    {
        QString solvedValue = parser_->parseExpression(QString::fromStdString(isPresent));

        bool toIntOk = true;
        int intValue = solvedValue.toInt(&toIntOk);

        if (!toIntOk || intValue < 0 || intValue > 1)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::hasValidTiedValue()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::hasValidTiedValue(QSharedPointer<AdHocConnection> connection) const
{
    if (!connection->getTiedValue().empty())
    {
        auto tiedValue = connection->getTiedValue();
        bool toInt = true;
        parser_->parseExpression(QString::fromStdString(tiedValue)).toInt(&toInt);

        return toInt || tiedValue.compare("default") == 0 || tiedValue.compare("open") == 0;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::hasValidPortReferences()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::hasValidPortReferences(QSharedPointer<AdHocConnection> connection) const
{
    if (connection->getInternalPortReferences()->isEmpty() && connection->getExternalPortReferences()->isEmpty())
    {
        return false;
    }

    auto const& tiedValue = connection->getTiedValue();
    if (std::any_of(connection->getInternalPortReferences()->cbegin(), connection->getInternalPortReferences()->cend(),
        [this, &tiedValue](auto const& internalReference)
        {return internalPortReferenceIsValid(internalReference, tiedValue) == false; }))
    {
        return false;
    }

    if (std::any_of(connection->getExternalPortReferences()->cbegin(), connection->getExternalPortReferences()->cend(),
        [this](auto const& externalReference)
        {return externalPortReferenceIsValid(externalReference) == false; }))
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::internalPortReferenceIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::internalPortReferenceIsValid(QSharedPointer<PortReference> portReference,
    std::string const& tiedValue) const
{
    if (portReference->getComponentRef().empty())
    {
        return false;
    }

    auto referencedInstance = getReferencedComponentInstance(portReference->getComponentRef());

    if (referencedInstance)
    {
        auto referencedComponent = getReferencedComponent(referencedInstance);
        if (referencedComponent)
        {
            auto referencedComponentPort = getReferencedPort(referencedComponent, portReference);
            if (referencedComponentPort)
            {
                return tiedValueIsValidWithReferencedPort(tiedValue, referencedComponentPort) &&
                    hasValidIsPresent(portReference->getIsPresent()) &&
                    portReferencePartSelectIsValid(portReference->getPartSelect());
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::getReferencedComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> AdHocConnectionValidator::getReferencedComponentInstance(
    std::string const& instanceReference) const
{
    QSharedPointer<ComponentInstance> foundInstance = nullptr;
    if (instanceReference.empty() || availableComponentInstances_->isEmpty())
    {
        return foundInstance;
    }

    auto it = std::find_if(availableComponentInstances_->cbegin(), availableComponentInstances_->cend(),
        [&instanceReference](auto const& instance) {return instance->getInstanceName() == instanceReference; });
        
    if (it != availableComponentInstances_->cend())
    {
        foundInstance = *it;
    }
    
    return foundInstance;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::getReferencedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> AdHocConnectionValidator::getReferencedComponent(
    QSharedPointer<ComponentInstance> referencingInstance) const
{
    if (referencingInstance && referencingInstance->getComponentRef())
    {
        QSharedPointer<const Component> referencedComponent =
            libraryHandler_->getModelReadOnly(*referencingInstance->getComponentRef()).dynamicCast<Component const>();

        return referencedComponent;
    }

    return QSharedPointer<const Component> ();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::getReferencedPort()
//-----------------------------------------------------------------------------
QSharedPointer<Port> AdHocConnectionValidator::getReferencedPort(QSharedPointer<const Component> component,
    QSharedPointer<PortReference> portReference) const
{
    QSharedPointer<Port> foundPort = nullptr;

    if (portReference->getPortRef().empty())
    {
        return foundPort;
    }

    auto it = std::find_if(component->getPorts()->cbegin(), component->getPorts()->cend(),
        [&portReference](auto const& currentPort) { return currentPort->name().toStdString() == portReference->getPortRef(); });

    if (it != component->getPorts()->cend())
    {
        foundPort = *it;
    }

    return foundPort;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::tiedValueIsValidWithReferencedPort()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::tiedValueIsValidWithReferencedPort(std::string const& tiedValue,
    QSharedPointer<Port> referencedPort) const
{
    return (tiedValue != "default" || referencedPort->getDefaultValue().isEmpty() == false);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::externalPortReferenceIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::externalPortReferenceIsValid(QSharedPointer<PortReference> externalPort) const
{
    if (!externalPort->getPortRef().empty())
    {
        return hasValidIsPresent(externalPort->getIsPresent()) &&
            portReferencePartSelectIsValid(externalPort->getPartSelect());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::portReferencePartSelectIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::portReferencePartSelectIsValid(QSharedPointer<PartSelect> partSelect) const
{
    if (partSelect == nullptr)
    {
        return true;
    }

    if ((partSelect->getLeftRange().isEmpty() || partSelect->getRightRange().isEmpty()) &&
        partSelect->getIndices()->isEmpty())
    {
        return false;
    }

    if (!partSelect->getLeftRange().isEmpty() && !partSelect->getRightRange().isEmpty())
    {
        if (!unsignedIntExpressionIsValid(partSelect->getLeftRange()) ||
            !unsignedIntExpressionIsValid(partSelect->getRightRange()))
        {
            return false;
        }
    }

    return std::all_of(partSelect->getIndices()->cbegin(), partSelect->getIndices()->cend(),
        [this](auto const& index) { return unsignedIntExpressionIsValid(index); });
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::unsignedIntExpressionIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::unsignedIntExpressionIsValid(QString const& expression) const
{
    bool toIntOk = true;
    int parsedExpression = parser_->parseExpression(expression).toInt(&toIntOk);

    return toIntOk && parsedExpression >= 0;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsIn(QVector<QString>& errors, QSharedPointer<AdHocConnection> connection,
    QString const& context) const
{
    QString connectionContext = QObject::tr("ad hoc connection %1").arg(connection->name());

    findErrorsInName(errors, connection, context);
    findErrorsInIsPresent(errors, connection->getIsPresent(), connectionContext, context);
    findErrorsInTiedValue(errors, connection, context);
    findErrorsInPortReferences(errors, connection, connectionContext, context);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInName(QVector<QString>& errors,
    QSharedPointer<AdHocConnection> connection, QString const& context) const
{
    if (!hasValidName(connection))
    {
        errors.append(QObject::tr("Invalid name '%1' set for ad hoc connection within %3")
            .arg(connection->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInIsPresent(QVector<QString>& errors, std::string const& isPresent,
    QString const& innerContext, QString const& context) const
{
    if (!hasValidIsPresent(isPresent))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 within %2").arg(innerContext).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInTiedValue()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInTiedValue(QVector<QString>& errors,
    QSharedPointer<AdHocConnection> connection, QString const& context) const
{
    if (!hasValidTiedValue(connection))
    {
        errors.append(QObject::tr("Invalid tied value set for ad hoc connection %1 within %2")
            .arg(connection->name()).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInPortReferences()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInPortReferences(QVector<QString>& errors,
    QSharedPointer<AdHocConnection> connection, QString const& innerContext, QString const& context) const
{
    if (!connection->getInternalPortReferences()->isEmpty() || !connection->getExternalPortReferences()->isEmpty())
    {
        QString internalElement = QObject::tr("internal port reference");
        QString externalElement = QObject::tr("external port reference");
        
        for (QSharedPointer<PortReference> internalPort : *connection->getInternalPortReferences())
        {
            findErrorsInInternalPortReference(errors, internalPort, connection->getTiedValue(), internalElement,
                innerContext, context);
        }
        for (QSharedPointer<PortReference> externalPort : *connection->getExternalPortReferences())
        {
            findErrorsInExternalPortReference(errors, externalPort, externalElement, innerContext, context);
        }
    }
    else
    {
        errors.append(QObject::tr("No port references set for %1 within %2").arg(innerContext).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInInternalPortReference()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInInternalPortReference(QVector<QString>& errors,
    QSharedPointer<PortReference> internalPort, std::string const& tiedValue, QString const& elementName,
    QString const& innerContext, QString const& context) const
{
    QSharedPointer<ComponentInstance> referencedInstance =
        getReferencedComponentInstance(internalPort->getComponentRef());

    if (internalPort->getComponentRef().empty())
    {
        errors.append(QObject::tr("No component reference set for internal port reference in %1 within %2").arg(
            innerContext, context));
    }
    else if (!referencedInstance)
    {
        errors.append(QObject::tr("Component instance %1 referenced by internal port reference in %2 within %3 "
            "was not found")
            .arg(QString::fromStdString(internalPort->getComponentRef()), innerContext, context));
    }

    QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);

    if (internalPort->getPortRef().empty())
    {
        errors.append(QObject::tr("No port reference set for internal port reference in %1 within %2").arg(
            innerContext, context));
    }
    else if (referencedComponent)
    {
        QSharedPointer<Port> referencedComponentPort = getReferencedPort(referencedComponent, internalPort);
        if (referencedComponentPort)
        {
            if (!tiedValueIsValidWithReferencedPort(tiedValue, referencedComponentPort))
            {
                errors.append(QObject::tr("No default value found for port '%1' referenced by internal port "
                    "reference in %2 within %3").arg(
                        QString::fromStdString(internalPort->getPortRef()), innerContext, context));
            }
        }
        else
        {
            errors.append(QObject::tr("Port '%1' referenced by the internal port reference  in %2 within %3 was "
                "not found").arg(
                    QString::fromStdString(internalPort->getPortRef()), innerContext, context));
        }
    }

    findErrorsInIsPresent(errors, internalPort->getIsPresent(), elementName, innerContext);
    findErrorsInPortReferencePartSelect(errors, internalPort->getPartSelect(), elementName, innerContext, context);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInExternalPortReference()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInExternalPortReference(QVector<QString>& errors,
    QSharedPointer<PortReference> externalPort, QString const& elementName, QString const& innerContext,
    QString const& context) const
{
    if (externalPort->getPortRef().empty())
    {
        errors.append(QObject::tr("No port reference set for external port reference in %1 within %2")
            .arg(innerContext).arg(context));
    }

    findErrorsInIsPresent(errors, externalPort->getIsPresent(), elementName, innerContext);
    findErrorsInPortReferencePartSelect(errors, externalPort->getPartSelect(), elementName, innerContext, context);
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::findErrorsInPortReferencePartSelect()
//-----------------------------------------------------------------------------
void AdHocConnectionValidator::findErrorsInPortReferencePartSelect(QVector<QString>& errors,
    QSharedPointer<PartSelect> partSelect, QString const& elementName, QString const& innerContext,
    QString const& context) const
{
    if (partSelect == nullptr)
    {
        return;
    }

    if (partSelect->getLeftRange().isEmpty() && partSelect->getRightRange().isEmpty() &&
        (!partSelect->getIndices() || partSelect->getIndices()->isEmpty()))
    {
        errors.append(QObject::tr("No range or index set for part select in %1 in %2 within %3")
            .arg(elementName).arg(innerContext).arg(context));
    }
    else
    {
        if (!partSelect->getLeftRange().isEmpty() || !partSelect->getRightRange().isEmpty())
        {
            if (!unsignedIntExpressionIsValid(partSelect->getLeftRange()))
            {
                errors.append(QObject::tr("Invalid left value '%1' set for %2 part select in %3 within %4")
                    .arg(partSelect->getLeftRange()).arg(elementName).arg(innerContext).arg(context));
            }
            if (!unsignedIntExpressionIsValid(partSelect->getRightRange()))
            {
                errors.append(QObject::tr("Invalid right value '%1' set for %2 part select in %3 within %4")
                    .arg(partSelect->getRightRange()).arg(elementName).arg(innerContext).arg(context));
            }
        }
        if (!partSelect->getIndices()->isEmpty())
        {
            for (QString const& index : *partSelect->getIndices())
            {
                if (!unsignedIntExpressionIsValid(index))
                {
                    errors.append(QObject::tr("Invalid index value '%1' set for %2 part select in %3 within %4")
                        .arg(index).arg(elementName).arg(innerContext).arg(context));
                }
            }
        }
    }
}

