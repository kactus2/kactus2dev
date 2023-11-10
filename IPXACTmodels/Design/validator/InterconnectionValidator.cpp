//-----------------------------------------------------------------------------
// File: InterconnectionValidator.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Mikko Teuho
// Date: 15.01.2016
//
// Description:
// Validator for the ipxact:interconnection and ipxact:monitorInterconnection.
//-----------------------------------------------------------------------------

#include "InterconnectionValidator.h"

#include <IPXACTmodels/common/validators/CommonItemsValidator.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/PortMap.h>

#include <IPXACTmodels/Design/Interconnection.h>
#include <IPXACTmodels/Design/MonitorInterconnection.h>
#include <IPXACTmodels/Design/ActiveInterface.h>
#include <IPXACTmodels/Design/ComponentInstance.h>

#include <KactusAPI/include/ExpressionParser.h>

#include <KactusAPI/include/LibraryInterface.h>

#include <QRegularExpression>

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::InterconnectionValidator()
//-----------------------------------------------------------------------------
InterconnectionValidator::InterconnectionValidator(QSharedPointer<ExpressionParser> parser,
    LibraryInterface* library):
parser_(parser),
libraryHandler_(library)
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::~InterconnectionValidator()
//-----------------------------------------------------------------------------
InterconnectionValidator::~InterconnectionValidator()
{

}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::setComponentInstances()
//-----------------------------------------------------------------------------
void InterconnectionValidator::changeComponentInstances(
    QSharedPointer<QList<QSharedPointer<ComponentInstance> > > newInstances)
{
    availableComponentInstances_ = newInstances;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::validateInterconnection()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::validateInterconnection(QSharedPointer<Interconnection> interconnection) const
{
    return hasValidName(interconnection->name()) && hasValidIsPresent(interconnection->getIsPresent()) &&
        hasValidStartInterface(interconnection) && hasValidInterfaces(interconnection);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidName()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidName(QString const& name) const
{
    return CommonItemsValidator::hasValidName(name);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidIsPresent()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidIsPresent(QString const& isPresent) const
{
    return CommonItemsValidator::hasValidIsPresent(isPresent, parser_);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidStartInterface()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidStartInterface(QSharedPointer<Interconnection> interconnection) const
{
    if (interconnection->getStartInterface())
    {
        return activeInterfaceIsValid(interconnection->getStartInterface());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::activeInterfaceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::activeInterfaceIsValid(QSharedPointer<ActiveInterface> activeInterface) const
{
    QSharedPointer<ComponentInstance> referencedInstance =
        getReferencedComponentInstance(activeInterface->getComponentReference());

    if (referencedInstance)
    {
        QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);

        if (referencedComponent)
        {
            return busReferenceIsValid(referencedComponent, activeInterface->getBusReference()) &&
                hasValidIsPresent(activeInterface->getIsPresent()) &&
                excludePortsAreValid(referencedComponent, activeInterface);
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::getReferencedComponentInstance()
//-----------------------------------------------------------------------------
QSharedPointer<ComponentInstance> InterconnectionValidator::getReferencedComponentInstance(
    QString const& instanceReference) const
{
    QSharedPointer<ComponentInstance> foundInstance = nullptr;
    if (instanceReference.isEmpty() || availableComponentInstances_->isEmpty())
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
// Function: InterconnectionValidator::getReferencedComponent()
//-----------------------------------------------------------------------------
QSharedPointer<const Component> InterconnectionValidator::getReferencedComponent(
    QSharedPointer<ComponentInstance> referencingInstance) const
{
    if (referencingInstance && referencingInstance->getComponentRef())
    {
        QSharedPointer<const Component> referencedComponent =
            libraryHandler_->getModelReadOnly<Component>(*referencingInstance->getComponentRef());

        return referencedComponent;
    }

    return QSharedPointer<const Component> ();
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::busReferenecIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::busReferenceIsValid(QSharedPointer<const Component> component,
    QString const& busReference) const
{
    if (busReference.isEmpty() || component == nullptr)
    {
        return false;
    }
        
    return std::any_of(component->getBusInterfaces()->cbegin(), component->getBusInterfaces()->cend(),
        [&busReference](auto const& currentBus) { return currentBus->name() == busReference; });
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::excludePortsAreValid()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::excludePortsAreValid(QSharedPointer<const Component> component,
    QSharedPointer<ActiveInterface> activeInterface) const
{
    if (activeInterface->getExcludePorts()->isEmpty())
    {
        return true;
    }

    auto busInterface = component->getBusInterface(activeInterface->getBusReference());

    if (busInterface == nullptr)
    {
        return false;
    }

    if (busInterface->getAbstractionTypes()->isEmpty())
    {
        return false;
    }

    auto portMaps = busInterface->getAllPortMaps();
    return std::all_of(activeInterface->getExcludePorts()->cbegin(), activeInterface->getExcludePorts()->cend(),
        [this, &portMaps](auto const& excludePort) { return singleExcludePortIsValid(excludePort, portMaps); });
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::singleExcludePortIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::singleExcludePortIsValid(QString const& portReference,
    QSharedPointer<QList<QSharedPointer<PortMap> > > portMaps) const
{
    return std::any_of(portMaps->cbegin(), portMaps->cend(),
        [&portReference](auto const& portMap) { return portMap->getLogicalPort()->name_ == portReference; });
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidInterfaces()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidInterfaces(QSharedPointer<Interconnection> interConnection) const
{
    if (!interConnection->getStartInterface() || (interConnection->getActiveInterfaces()->isEmpty() && 
        interConnection->getHierInterfaces()->isEmpty()))
    {
        return false;
    }
    else
    {
        QMap<QString, QString> interfaceReferences;

        if (interConnection->getStartInterface())
        {
            interfaceReferences.insert(interConnection->getStartInterface()->getComponentReference(),
                interConnection->getStartInterface()->getBusReference());
        }

        for (auto const& currentInterface: *interConnection->getActiveInterfaces())
        {
            if (!activeInterfaceIsValid(currentInterface) || !referenceCombinationIsUnique(interfaceReferences,
                currentInterface->getComponentReference(), currentInterface->getBusReference()))
            {
                return false;
            }
            else
            {
                interfaceReferences.insert(currentInterface->getComponentReference(),
                    currentInterface->getBusReference());
            }
        }

        QVector<QString> usedHierBusInterfaces;
        for (auto const& currentInterface : *interConnection->getHierInterfaces())
        {
            if (usedHierBusInterfaces.contains(currentInterface->getBusReference()) ||
                !hasValidIsPresent(currentInterface->getIsPresent()))
            {
                return false;
            }
            else
            {
                usedHierBusInterfaces.append(currentInterface->getBusReference());
            }
        }

        return true;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::referenceCombinationIsUnique()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::referenceCombinationIsUnique(QMap<QString, QString> referenceCombinations,
    QString const& componentReference, QString const& busReference) const
{
    for (auto it = referenceCombinations.cbegin(); it != referenceCombinations.cend(); ++it)
    {
        if (it.key() == componentReference && it.value() == busReference)
        {
            return false;
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::validateMonitorInterconnection()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::validateMonitorInterconnection(QSharedPointer<MonitorInterconnection> connection)
    const
{
    return hasValidName(connection->name()) && hasValidIsPresent(connection->getIsPresent()) &&
        hasValidMonitoredActiveInterface(connection) && hasValidMonitorInterfaces(connection);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidMonitoredActiveInterface()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidMonitoredActiveInterface(QSharedPointer<MonitorInterconnection> connection)
    const
{
    if (connection->getMonitoredActiveInterface())
    {
        return monitorInterfaceIsValid(connection->getMonitoredActiveInterface());
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::monitorInterfaceIsValid()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::monitorInterfaceIsValid(QSharedPointer<MonitorInterface> monitorInterface) const
{
    if (QSharedPointer<ComponentInstance> referencedInstance = getReferencedComponentInstance(
            monitorInterface->getComponentReference()); 
        referencedInstance)
    {
        QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);

        if (referencedComponent)
        {
            return busReferenceIsValid(referencedComponent, monitorInterface->getBusReference());
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::hasValidMonitorInterfaces()
//-----------------------------------------------------------------------------
bool InterconnectionValidator::hasValidMonitorInterfaces(QSharedPointer<MonitorInterconnection> connection) const
{
    if (connection->getMonitorInterfaces()->isEmpty())
    {
        return false;
    }

    QMap<QString, QString> interfaceReferences;
    if (connection->getMonitoredActiveInterface())
    {
        interfaceReferences.insert(connection->getMonitoredActiveInterface()->getComponentReference(),
            connection->getMonitoredActiveInterface()->getBusReference());
    }

    for (auto const& monitorInterface : *connection->getMonitorInterfaces())
    {
        if (!monitorInterfaceIsValid(monitorInterface) ||
            !referenceCombinationIsUnique(interfaceReferences, monitorInterface->getComponentReference(),
                monitorInterface->getBusReference()))
        {
            return false;
        }
        else
        {
            interfaceReferences.insert(monitorInterface->getComponentReference(), monitorInterface->getBusReference());
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInInterconnection()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInInterconnection(QVector<QString>& errors,
    QSharedPointer<Interconnection> interconnection, QString const& context) const
{
    QString interconnectionContext = QObject::tr("interconnection '%1'").arg(interconnection->name());

    findErrorsInName(errors, interconnection->name(), QLatin1String("interconnection"), context);
    findErrorsInIsPresent(errors, interconnection->getIsPresent(), interconnectionContext, context);
    findErrorsInStartInterface(errors, interconnection, interconnectionContext, context);
    findErrorsInInterfaces(errors, interconnection, interconnectionContext, context);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInName()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInName(QVector<QString>& errors, QString const& name,
    QString const& element, QString const& context) const
{
    if (!hasValidName(name))
    {
        errors.append(QObject::tr("Invalid name '%1' set for %2 within %3").arg(name, element, context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInIsPresent()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInIsPresent(QVector<QString>& errors, QString const& isPresent,
    QString const& innerContext, QString const& context) const
{
    if (!hasValidIsPresent(isPresent))
    {
        errors.append(QObject::tr("Invalid isPresent set for %1 within %2").arg(innerContext, context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInStartInterface()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInStartInterface(QVector<QString>& errors,
    QSharedPointer<Interconnection> interconnection, QString const& innerContext, QString const& context) const
{
    if (interconnection->getStartInterface() && 
        (!interconnection->getStartInterface()->getComponentReference().isEmpty() ||
        !interconnection->getStartInterface()->getBusReference().isEmpty()))
    {
        findErrorsInActiveInterface(errors, interconnection->getStartInterface(), innerContext, context);
    }
    else
    {
        errors.append(QObject::tr("No active interface set for %1 within %2").arg(innerContext, context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInActiveInterface()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInActiveInterface(QVector<QString>& errors,
    QSharedPointer<ActiveInterface> activeInterface, QString const& innerContext, QString const& context) const
{
    QString elementName = QStringLiteral("active interface");

    QSharedPointer<ComponentInstance> referencedInstance =
        getReferencedComponentInstance(activeInterface->getComponentReference());

    findErrorsInComponentReference(errors, activeInterface->getComponentReference(), referencedInstance,
        elementName, innerContext, context);

    QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);
    findErrorsInBusReference(errors, activeInterface->getBusReference(), referencedComponent, elementName,
        innerContext, context);

    QString interfaceContext = QObject::tr("active interface");

    findErrorsInIsPresent(errors, activeInterface->getIsPresent(), interfaceContext, innerContext);
    findErrorsInExcludePorts(errors, activeInterface, referencedComponent, innerContext);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInComponentReference()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInComponentReference(QVector<QString>& errors,
    QString const& componentReference, QSharedPointer<ComponentInstance> referencedInstance,
    QString const& elementName, QString const& innerContext, QString const& context) const
{
    if (componentReference.isEmpty())
    {
        errors.append(QObject::tr("No component reference set for %1 in %2 within %3")
            .arg(elementName, innerContext, context));
    }

    else if (!referencedInstance)
    {
        errors.append(QObject::tr("Component instance '%1' referenced by the %2 in %3 within %4 was "
            "not found")
            .arg(componentReference, elementName, innerContext, context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInBusReference()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInBusReference(QVector<QString>& errors, QString const& busReference,
    QSharedPointer<const Component> referencedComponent, QString const& elementName, QString const& innerContext,
    QString const& context) const
{
    if (referencedComponent)
    {
        if (busReference.isEmpty())
        {
            errors.append(QObject::tr("No bus reference set for %1 in %2 within %3")
                .arg(elementName, innerContext, context));
        }
        else
        {
            if (!busReferenceIsValid(referencedComponent, busReference))
            {
                errors.append(QObject::tr("Bus interface '%1' referenced by the %2 in %3 within %4 was not found")
                    .arg(busReference, elementName, innerContext, context));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInExcludePorts()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInExcludePorts(QVector<QString>& errors,
    QSharedPointer<ActiveInterface> activeInterface, QSharedPointer<const Component> referencedComponent,
    QString const& innerContext) const
{
    if (!activeInterface->getExcludePorts()->isEmpty())
    {
        auto busInterface = referencedComponent->getBusInterface(activeInterface->getBusReference());
        if (busInterface == nullptr)
        {
            return;
        }

        auto portMaps = busInterface->getAllPortMaps();
        for (QString const& excludePort : *activeInterface->getExcludePorts())
        {
            if (!singleExcludePortIsValid(excludePort, portMaps))
            {
                errors.append(QObject::tr("Logical port referenced in active interface in %1 was not "
                    "found in the port maps of the referenced bus interface %2")
                    .arg(innerContext,busInterface->name()));
            }
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInInterfaces()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInInterfaces(QVector<QString>& errors,
    QSharedPointer<Interconnection> interConnection, QString const& innerContext, QString const& context) const
{
    if (!interConnection->getStartInterface() || (interConnection->getActiveInterfaces()->isEmpty() && 
        interConnection->getHierInterfaces()->isEmpty()))
    {
        errors.append(QObject::tr("No active interfaces or hierarchical interfaces set for %1 within %2")
            .arg(innerContext, context));        
    }
    else
    {
        QMap<QString, QString> interfaceReferences;

        if (interConnection->getStartInterface())
        {
            interfaceReferences.insert(interConnection->getStartInterface()->getComponentReference(),
                interConnection->getStartInterface()->getBusReference());
        }

        for (QSharedPointer<ActiveInterface> currentInterface : *interConnection->getActiveInterfaces())
        {
            if (!referenceCombinationIsUnique(interfaceReferences, currentInterface->getComponentReference(),
                currentInterface->getBusReference()))
            {
                errors.append(QObject::tr("Component reference '%1' and bus reference '%2' pair is not unique in "
                    "active interfaces of %3 within %4.")
                    .arg(currentInterface->getComponentReference(), currentInterface->getBusReference(),
                        innerContext, context));
            }
            else
            {
                interfaceReferences.insert(currentInterface->getComponentReference(),
                    currentInterface->getBusReference());
            }

            findErrorsInActiveInterface(errors, currentInterface, innerContext, context);
        }

        QVector<QString> usedHierBusReferences;
        for (QSharedPointer<HierInterface> hierarchicalInterface : *interConnection->getHierInterfaces())
        {
            if (usedHierBusReferences.contains(hierarchicalInterface->getBusReference()))
            {
                errors.append(QObject::tr("Bus reference '%1' is not unique in hierarchical interfaces of %2 "
                    "within %3.")
                    .arg(hierarchicalInterface->getBusReference(), innerContext, context));
            }
            else
            {
                usedHierBusReferences.append(hierarchicalInterface->getBusReference());
            }

            findErrorsInIsPresent(errors, hierarchicalInterface->getIsPresent(), innerContext, context);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInMonitorInterconnection()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInMonitorInterconnection(QVector<QString>& errors,
    QSharedPointer<MonitorInterconnection> connection, QString const& context) const
{
    QString monitorContext = QObject::tr("monitor interconnection '%1'").arg(connection->name());

    findErrorsInName(errors, connection->name(), QStringLiteral("monitor interconnection"), context);
    findErrorsInIsPresent(errors, connection->getIsPresent(), monitorContext, context);
    findErrorsInMonitoredActiveInterface(errors, connection, monitorContext, context);
    findErrorsInMonitorInterfaces(errors, connection, monitorContext, context);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInMonitoredActiveInterface()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInMonitoredActiveInterface(QVector<QString>& errors,
    QSharedPointer<MonitorInterconnection> connection, QString const& innerContext, QString const& context) const
{
    QString elementName = QStringLiteral("monitored active interface");

    if (connection->getMonitoredActiveInterface() &&
        (!connection->getMonitoredActiveInterface()->getComponentReference().isEmpty() ||
        !connection->getMonitoredActiveInterface()->getBusReference().isEmpty()))
    {
        findErrorsInSingleMonitorInterface(errors, connection->getMonitoredActiveInterface(), elementName,
            innerContext, context);
    }
    else
    {
        errors.append(QObject::tr("No %1 set for %2 within %3").arg(elementName, innerContext, context));
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInMonitorInterface()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInSingleMonitorInterface(QVector<QString>& errors,
    QSharedPointer<MonitorInterface> monitorInterface, QString const& elementName, QString const& innerContext,
    QString const& context) const
{
    QSharedPointer<ComponentInstance> referencedInstance =
        getReferencedComponentInstance(monitorInterface->getComponentReference());

    findErrorsInComponentReference(errors, monitorInterface->getComponentReference(), referencedInstance,
        elementName, innerContext, context);

    QSharedPointer<const Component> referencedComponent = getReferencedComponent(referencedInstance);
    findErrorsInBusReference(errors, monitorInterface->getBusReference(), referencedComponent, elementName,
        innerContext, context);
}

//-----------------------------------------------------------------------------
// Function: InterconnectionValidator::findErrorsInMonitorInterfaces()
//-----------------------------------------------------------------------------
void InterconnectionValidator::findErrorsInMonitorInterfaces(QVector<QString>& errors,
    QSharedPointer<MonitorInterconnection> connection, QString const& innerContext, QString const& context) const
{
    if (!connection->getMonitorInterfaces()->isEmpty())
    {
        QMap<QString, QString> interfaceReferences;
        if (connection->getMonitoredActiveInterface())
        {
            interfaceReferences.insert(connection->getMonitoredActiveInterface()->getComponentReference(),
                connection->getMonitoredActiveInterface()->getBusReference());
        }

        QString elementName = QLatin1String("monitor interface");
        for (QSharedPointer<MonitorInterface> monitorInterface: *connection->getMonitorInterfaces())
        {
            if (!referenceCombinationIsUnique(interfaceReferences, monitorInterface->getComponentReference(),
                monitorInterface->getBusReference()))
            {
                errors.append(QObject::tr("Component reference '%1' and bus reference '%2' pair is not unique in "
                    "monitor interfaces of %3 within %4.")
                    .arg(monitorInterface->getComponentReference(), monitorInterface->getBusReference(),
                    innerContext, context));
            }
            else
            {
                interfaceReferences.insert(monitorInterface->getComponentReference(),
                    monitorInterface->getBusReference());
            }

            findErrorsInSingleMonitorInterface(errors, monitorInterface, elementName, innerContext, context);
        }
    }
    else
    {
        errors.append(QObject::tr("No monitor interfaces set for %1 within %2").arg(innerContext, context));
    }
}
