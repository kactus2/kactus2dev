//-----------------------------------------------------------------------------
// File: InterconnectionConfigurationValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 11.01.2016
//
// Description:
// Validator for the ipxact:interconnectionConfiguration.
//-----------------------------------------------------------------------------

#include "InterconnectionConfigurationValidator.h"

#include <IPXACTmodels/DesignConfiguration/InterconnectionConfiguration.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <IPXACTmodels/Design/Design.h>
#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/MonitorInterconnection.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <KactusAPI/include/ExpressionParser.h>

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::InterconnectionConfigurationValidator()
//-----------------------------------------------------------------------------
InterconnectionConfigurationValidator::InterconnectionConfigurationValidator(
    QSharedPointer<ExpressionParser> parser, LibraryInterface* library ):
parser_(parser),
libraryHandler_(library)
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::designChanged()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::designChanged(QSharedPointer<Design> newDesign)
{
    availableInterconnections_ = newDesign->getInterconnections();
    availableMonitorInterconnections_ = newDesign->getMonitorInterconnecions();
    availableComponentInstances_ = newDesign->getComponentInstances();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::validate()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::validate(QSharedPointer<InterconnectionConfiguration> configuration)
{
    return hasValidInterconnectionReference(configuration) && hasValidIsPresent(configuration->getIsPresent()) &&
        hasValidMultipleAbstractorInstances(configuration);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidInterconnectionReference()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::hasValidInterconnectionReference(
    QSharedPointer<InterconnectionConfiguration> configuration) const
{
    auto matchReference = [configuration](auto const& connection)
    {
        return (configuration->getInterconnectionReference() == connection->name().toStdString());
    };

    if (availableInterconnections_)
    {
        auto hasConnection = std::any_of(availableInterconnections_->cbegin(), availableInterconnections_->cend(), matchReference);
        if (hasConnection)
        {
            return true;
        }
    }

    if (availableMonitorInterconnections_)
    {
        auto hasMonitorConnection = std::any_of(availableMonitorInterconnections_->cbegin(), 
            availableMonitorInterconnections_->cend(), matchReference);

        return hasMonitorConnection;
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::hasValidIsPresent(std::string const& isPresent) const
{
    if (isPresent.empty() == false)
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
// Function: InterconnectionConfigurationValidator::hasValidAbstractorInstances()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::hasValidMultipleAbstractorInstances(
    QSharedPointer<InterconnectionConfiguration> configuration)
{
    if (!configuration->getAbstractorInstances() || configuration->getAbstractorInstances()->isEmpty())
    {
        return false;
    }

    for (QSharedPointer<MultipleAbstractorInstances> instance : *configuration->getAbstractorInstances())
    {
        if (!multipleAbstractorInstanceIsValid(instance))
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidMultipleAbstractorInstance()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::multipleAbstractorInstanceIsValid(
    QSharedPointer<MultipleAbstractorInstances> multipleInstance)
{
    if (!multipleInstance->getAbstractorInstances() || multipleInstance->getAbstractorInstances()->isEmpty() ||
        !hasValidIsPresent(multipleInstance->getIsPresent()) || !hasValidInterfaceReferences(multipleInstance))
    {
        return false;
    }

    QVector<std::string> instanceNames;
    for (QSharedPointer<AbstractorInstance> instance : *multipleInstance->getAbstractorInstances())
    {
        if (instanceNames.contains(instance->getInstanceName()) || !abstractorInstanceIsValid(instance))
        {
            return false;
        }
        else
        {
            instanceNames.append(instance->getInstanceName());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::interfaceReferenceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::hasValidInterfaceReferences(
    QSharedPointer<MultipleAbstractorInstances> multipleInstance)
{
    if (multipleInstance->getInterfaceReferences())
    {
        for (QSharedPointer<InterfaceRef> interfaceReference : *multipleInstance->getInterfaceReferences())
        {
            if (!interfaceReferenceComponentReferenceIsValid(interfaceReference) ||
                !interfaceReferenceBusReferenceIsValid(interfaceReference) ||
                !hasValidIsPresent(interfaceReference->getIsPresent()))
            {
                return false;
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::interfaceReferenceComponentReferenceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::interfaceReferenceComponentReferenceIsValid(
    QSharedPointer<InterfaceRef> interfaceReference) const
{
    if (interfaceReference->getComponentRef().empty() == false && availableComponentInstances_)
    {
        for (QSharedPointer<ComponentInstance> instance : *availableComponentInstances_)
        {
            if (interfaceReference->getComponentRef() == instance->getInstanceName().toStdString())
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::interfaceReferenceBusReferenceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::interfaceReferenceBusReferenceIsValid(
    QSharedPointer<InterfaceRef> interfaceReference)
{
    changeAvailableBusInterfaces(interfaceReference);

    if (interfaceReference->getBusRef().empty() == false && availableBusInterfaces_)
    {
        for (QSharedPointer<BusInterface> currentInterface : *availableBusInterfaces_)
        {
            if (interfaceReference->getBusRef() == currentInterface->name().toStdString())
            {
                return true;
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::changeAvailableBusInterfaces()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::changeAvailableBusInterfaces(
    QSharedPointer<InterfaceRef> interfaceReference)
{
    if (availableComponentInstances_)
    {
        for (QSharedPointer<ComponentInstance> instance : *availableComponentInstances_)
        {
            if (instance->getInstanceName().toStdString() == interfaceReference->getComponentRef())
            {
                if (instance->getComponentRef() && instance->getComponentRef()->isValid())
                {
                    QSharedPointer<Component> referencedComponent =
                        libraryHandler_->getModel(*instance->getComponentRef().data()).dynamicCast<Component>();

                    if (referencedComponent)
                    {
                        availableBusInterfaces_ = referencedComponent->getBusInterfaces();
                        return;
                    }
                }

                break;
            }
        }
    }

    if (availableBusInterfaces_)
    {
        availableBusInterfaces_->clear();
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::abstractorInstanceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::abstractorInstanceIsValid(QSharedPointer<AbstractorInstance> instance)
    const
{
    return hasValidName(instance->getInstanceName()) &&
        instance->getAbstractorRef()->getType() == VLNV::ABSTRACTOR && instance->getAbstractorRef()->isValid() &&
        hasValidName(instance->getViewName());
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::hasValidName()
//-----------------------------------------------------------------------------
bool InterconnectionConfigurationValidator::hasValidName(std::string const& name) const
{
    return (name.empty() == false && std::all_of(name.begin(), name.end(), isspace) == false);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsIn()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsIn(QVector<QString>& errors,
    QSharedPointer<InterconnectionConfiguration> configuration, QString const& context)
{
    QString interconnectionContext = QObject::tr("interconnection configuration %1")
        .arg(QString::fromStdString(configuration->getInterconnectionReference()));

    findErrorsInInterconnectionReference(errors, configuration, context);
    findErrorsInIsPresent(errors, configuration->getIsPresent(), interconnectionContext, context);
    findErrorsInMultipleAbstractorInstances(errors, configuration, interconnectionContext, context);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsInInterconnectionReference()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsInInterconnectionReference(QVector<QString>& errors,
    QSharedPointer<InterconnectionConfiguration> configuration, QString const& context) const
{
    if (!hasValidInterconnectionReference(configuration))
    {
        errors.append(QObject::tr("Invalid interconnection reference '%1' given for interconnection configuration "
            "within %2.").arg(QString::fromStdString(configuration->getInterconnectionReference()), context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsInIsPresent(QVector<QString>& errors,
    std::string const& isPresent, QString const& innerContext, QString const& context) const
{
    if (!hasValidIsPresent(isPresent))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 within %2").arg(innerContext).arg(context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsInMultipleAbstractorInstances()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsInMultipleAbstractorInstances(QVector<QString>& errors,
    QSharedPointer<InterconnectionConfiguration> configuration, QString const& innerContext,
    QString const& context)
{
    if (!configuration->getAbstractorInstances() || configuration->getAbstractorInstances()->isEmpty())
    {
        errors.append(QObject::tr("No abstractor instances found in interconnection configuration '%1' within %2")
            .arg(QString::fromStdString(configuration->getInterconnectionReference()), context));
    }

    for (QSharedPointer<MultipleAbstractorInstances> multipleInstance : *configuration->getAbstractorInstances())
    {
        findErrorsInOneMultipleAbstractorInstance(errors, multipleInstance, innerContext);
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsInOneMultipleAbstractorInstance()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsInOneMultipleAbstractorInstance(QVector<QString>& errors,
    QSharedPointer<MultipleAbstractorInstances> multipleInstance, QString const& context)
{
    QString multipleInstanceContext = QObject::tr("abstractor instance group");
    findErrorsInIsPresent(errors, multipleInstance->getIsPresent(), multipleInstanceContext, context);

    for (QSharedPointer<InterfaceRef> interfaceReference : *multipleInstance->getInterfaceReferences())
    {
        if (!interfaceReferenceComponentReferenceIsValid(interfaceReference))
        {
            errors.append(QObject::tr("Invalid component instance reference '%1' set for %2 within %3")
                .arg(QString::fromStdString(interfaceReference->getComponentRef())).arg(multipleInstanceContext).arg(context));
        }
        if (!interfaceReferenceBusReferenceIsValid(interfaceReference))
        {
            errors.append(QObject::tr("Invalid bus interface reference '%1' set for %2 within %3")
                .arg(QString::fromStdString(interfaceReference->getBusRef())).arg(multipleInstanceContext).arg(context));
        }
        if (!hasValidIsPresent(interfaceReference->getIsPresent()))
        {
            errors.append(QObject::tr("Invalid isPresent set for interface reference in %1 within %2")
                .arg(multipleInstanceContext).arg(context));
        }
    }

    if (!multipleInstance->getAbstractorInstances() || multipleInstance->getAbstractorInstances()->isEmpty())
    {
        errors.append(QObject::tr("No abstractor instances found in %1 within %2")
            .arg(multipleInstanceContext).arg(context));
    }
    else
    {
        QVector<std::string> instanceNames;
        QVector<std::string> duplicateNames;
        for (QSharedPointer<AbstractorInstance> instance : *multipleInstance->getAbstractorInstances())
        {
            if (instanceNames.contains(instance->getInstanceName()) &&
                !duplicateNames.contains(instance->getInstanceName()))
            {
                errors.append(QObject::tr("Abstractor instance name '%1' within %2 is not unique.")
                    .arg(QString::fromStdString(instance->getInstanceName())).arg(context));

                duplicateNames.append(instance->getInstanceName());
            }

            instanceNames.append(instance->getInstanceName());
            findErrorsInAbstractorInstance(errors, instance, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionConfigurationValidator::findErrorsInAbstractorInstance()
//-----------------------------------------------------------------------------
void InterconnectionConfigurationValidator::findErrorsInAbstractorInstance(QVector<QString>& errors,
    QSharedPointer<AbstractorInstance> instance, QString const& context) const
{
    if (!hasValidName(instance->getInstanceName()))
    {
        errors.append(QObject::tr("Invalid instance name '%1' set for abstractor instance within %2")
            .arg(QString::fromStdString(instance->getInstanceName())).arg(context));
    }

    QString abstractorContext = QObject::tr("abstractor instance %1").arg(QString::fromStdString(instance->getInstanceName()));

    instance->getAbstractorRef()->isValid(errors, abstractorContext);

    if (!hasValidName(instance->getViewName()))
    {
        errors.append(QObject::tr("Invalid view name '%1' set for abstractor instance %2 within %3").arg(
            QString::fromStdString(instance->getViewName()),
            QString::fromStdString(instance->getInstanceName()),
            context));
    }
}