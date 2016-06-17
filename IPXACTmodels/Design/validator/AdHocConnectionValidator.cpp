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

#include <editors/ComponentEditor/common/ExpressionParser.h>

#include <library/LibraryManager/libraryinterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::AdHocConnectionValidator()
//-----------------------------------------------------------------------------
AdHocConnectionValidator::AdHocConnectionValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
parser_(parser),
libraryHandler_(library),
availableComponentInstances_(new QList<QSharedPointer<ComponentInstance> > ())
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
    whiteSpaceExpression.setPattern("^\\s*$");
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
bool AdHocConnectionValidator::hasValidIsPresent(QString const& isPresent) const
{
    if (!isPresent.isEmpty())
    {
        QString solvedValue = parser_->parseExpression(isPresent);

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
    if (!connection->getTiedValue().isEmpty())
    {
        QString tiedValue = connection->getTiedValue();
        bool toInt = true;
        parser_->parseExpression(tiedValue).toInt(&toInt);

        return toInt || tiedValue == QLatin1String("default") || tiedValue == QLatin1String("open");
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::hasValidPortReferences()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::hasValidPortReferences(QSharedPointer<AdHocConnection> connection) const
{
    if (!connection->getInternalPortReferences()->isEmpty() || !connection->getExternalPortReferences()->isEmpty())
    {
        foreach (QSharedPointer<PortReference> internalReference, *connection->getInternalPortReferences())
        {
            if (!internalPortReferenceIsValid(internalReference, connection->getTiedValue()))
            {
                return false;
            }
        }

        foreach (QSharedPointer<PortReference> externalReference, *connection->getExternalPortReferences())
        {
            if (!externalPortReferenceIsValid(externalReference))
            {
                return false;
            }
        }

        return true;
    }
    else
    {
        return false;
    }
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::internalPortReferenceIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::internalPortReferenceIsValid(QSharedPointer<PortReference> portReference,
    QString const& tiedValue) const
{
    if (!portReference->getComponentRef().isEmpty())
    {
        QSharedPointer<ComponentInstance> referencedInstance =
            getReferencedComponentInstance(portReference->getComponentRef());

        if (referencedInstance)
        {
            QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);
            if (referencedComponent)
            {
                QSharedPointer<Port> referencedComponentPort =
                    getReferencedPort(referencedComponent, portReference);
                if (referencedComponentPort)
                {
                    return tiedValueIsValidWithReferencedPort(tiedValue, referencedComponentPort) &&
                        hasValidIsPresent(portReference->getIsPresent()) &&
                        portReferencePartSelectIsValid(portReference->getPartSelect());
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::getReferencedComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> AdHocConnectionValidator::getReferencedComponentInstance(
    QString const& instanceReference) const
{
    if (!instanceReference.isEmpty() && !availableComponentInstances_->isEmpty())
    {
        foreach (QSharedPointer<ComponentInstance> instance, *availableComponentInstances_)
        {
            if (instance->getInstanceName() == instanceReference)
            {
                return instance;
            }
        }
    }

    return QSharedPointer<ComponentInstance> ();
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
    if (!portReference->getPortRef().isEmpty())
    {
        foreach (QSharedPointer<Port> currentPort, *component->getPorts())
        {
            if (currentPort->name() == portReference->getPortRef())
            {
                return currentPort;
            }
        }
    }

    return QSharedPointer<Port> ();
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::tiedValueIsValidWithReferencedPort()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::tiedValueIsValidWithReferencedPort(QString const& tiedValue,
    QSharedPointer<Port> referencedPort) const
{
    if (tiedValue == QLatin1String("default") && referencedPort->getDefaultValue().isEmpty())
    {
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: AdHocConnectionValidator::externalPortReferenceIsValid()
//-----------------------------------------------------------------------------
bool AdHocConnectionValidator::externalPortReferenceIsValid(QSharedPointer<PortReference> externalPort) const
{
    if (!externalPort->getPortRef().isEmpty())
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
    if (partSelect)
    {
        if ((!partSelect->getLeftRange().isEmpty() && !partSelect->getRightRange().isEmpty()) ||
            !partSelect->getIndices()->isEmpty())
        {
            if (!partSelect->getLeftRange().isEmpty() && !partSelect->getRightRange().isEmpty())
            {
                if (!unsignedIntExpressionIsValid(partSelect->getLeftRange()) ||
                    !unsignedIntExpressionIsValid(partSelect->getRightRange()))
                {
                    return false;
                }
            }

            foreach (QString index, *partSelect->getIndices())
            {
                if (!unsignedIntExpressionIsValid(index))
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }

    return true;
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
void AdHocConnectionValidator::findErrorsInIsPresent(QVector<QString>& errors, QString const& isPresent,
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
        foreach (QSharedPointer<PortReference> internalPort, *connection->getInternalPortReferences())
        {
            findErrorsInInternalPortReference(errors, internalPort, connection->getTiedValue(), internalElement,
                innerContext, context);
        }
        foreach (QSharedPointer<PortReference> externalPort, *connection->getExternalPortReferences())
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
    QSharedPointer<PortReference> internalPort, QString const& tiedValue, QString const& elementName,
    QString const& innerContext, QString const& context) const
{
    QSharedPointer<ComponentInstance> referencedInstance =
        getReferencedComponentInstance(internalPort->getComponentRef());

    if (internalPort->getComponentRef().isEmpty())
    {
        errors.append(QObject::tr("No component reference set for internal port reference in %1 within %2")
            .arg(innerContext).arg(context));
    }
    else if (!referencedInstance)
    {
        errors.append(QObject::tr("Component instance %1 referenced by internal port reference in %2 within %3 "
            "was not found")
            .arg(internalPort->getComponentRef()).arg(innerContext).arg(context));
    }

    QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);

    if (internalPort->getPortRef().isEmpty())
    {
        errors.append(QObject::tr("No port reference set for internal port reference in %1 within %2")
            .arg(innerContext).arg(context));
    }
    else if (referencedComponent)
    {
        QSharedPointer<Port> referencedComponentPort = getReferencedPort(referencedComponent, internalPort);
        if (referencedComponentPort)
        {
            if (!tiedValueIsValidWithReferencedPort(tiedValue, referencedComponentPort))
            {
                errors.append(QObject::tr("No default value found for port '%1' referenced by internal port "
                    "reference in %2 within %3")
                    .arg(internalPort->getPortRef()).arg(innerContext).arg(context));
            }
        }
        else
        {
            errors.append(QObject::tr("Port '%1' referenced by the internal port reference  in %2 within %3 was "
                "not found")
                .arg(internalPort->getPortRef()).arg(innerContext).arg(context));
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
    if (externalPort->getPortRef().isEmpty())
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
    if (partSelect)
    {
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
                foreach (QString index, *partSelect->getIndices())
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
}
