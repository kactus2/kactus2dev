#include "InterconnectDataModel.h"

#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/common/Document.h>
#include <common/graphicsItems/ComponentItem.h>

InterconnectDataModel::InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library)
    : designWidget_(designWidget),
    library_(library)
{
}

void InterconnectDataModel::gatherBusAndAbstractionData(
    const QSet<General::InterfaceMode>& initiatorModes,
    const QSet<General::InterfaceMode>& targetModes)
{
    initiatorModes_ = initiatorModes;
    targetModes_ = targetModes;

    getBusesFromInstances();
    getBusesFromTopComponent();
    filterValidAbstractionReferences();
}

QSet<QSharedPointer<ConfigurableVLNVReference>> InterconnectDataModel::getValidAbstractionRefs() const
{
    return validAbsRefs_;
}

QHash<QString, QSet<QSharedPointer<BusInterface>>> InterconnectDataModel::getInstanceBusMap() const
{
    return instanceBusesHash_;
}

QHash<QSharedPointer<BusInterface>, QSet<QString>> InterconnectDataModel::getInterfaceAbstractionHash() const
{
    return interfaceAbsDefsHash_;
}

void InterconnectDataModel::getBusesFromInstances()
{
    QList<ComponentItem*> componentItems = designWidget_->getInstances();
    for (ComponentItem* componentItem : componentItems)
    {
        QString instanceName = componentItem->name();
        instanceBusesHash_.insert(instanceName, QSet<QSharedPointer<BusInterface>>());

        QList<ConnectionEndpoint*> endpoints = componentItem->getEndpoints();
        for (ConnectionEndpoint* endpoint : endpoints)
        {
            QSharedPointer<BusInterface> bus = endpoint->getBusInterface();
            if (!bus)
                continue;

            instanceBusesHash_[instanceName].insert(bus);
            getAbstractionDefinitions(bus);
        }
    }
}

void InterconnectDataModel::getBusesFromTopComponent()
{
    QSharedPointer<Component> topComponent = designWidget_->getEditedComponent();
    if (!topComponent)
        return;

    QString name = topComponent->getVlnv().getName();
    QSharedPointer<QList<QSharedPointer<BusInterface>>> buses = topComponent->getBusInterfaces();
    if (!buses)
        return;

    instanceBusesHash_.insert(name, QSet<QSharedPointer<BusInterface>>());
    for (QSharedPointer<BusInterface> bus : *buses)
    {
        instanceBusesHash_[name].insert(bus);
        getAbstractionDefinitions(bus, false);
    }
}

void InterconnectDataModel::getAbstractionDefinitions(QSharedPointer<BusInterface> bus, bool fromInstance)
{
    QSharedPointer<QList<QSharedPointer<AbstractionType>>> absTypes = bus->getAbstractionTypes();
    if (!absTypes)
        return;

    for (const QSharedPointer<AbstractionType>& type : *absTypes)
    {
        if (!type)
            continue;

        QSharedPointer<ConfigurableVLNVReference> absRef = type->getAbstractionRef();
        if (!absRef)
            continue;

        allAbsRefs_.insert(absRef);

        if (!interfaceAbsDefsHash_.contains(bus))
            interfaceAbsDefsHash_.insert(bus, QSet<QString>());

        interfaceAbsDefsHash_[bus].insert(absRef->getName());
    }
}

void InterconnectDataModel::filterValidAbstractionReferences()
{
    validAbsRefs_.clear();

    for (const QSharedPointer<ConfigurableVLNVReference>& absRef : allAbsRefs_)
    {
        bool hasInitiator = false;
        bool hasTarget = false;

        for (auto it = instanceBusesHash_.begin(); it != instanceBusesHash_.end(); ++it)
        {
            const QSet<QSharedPointer<BusInterface>>& busSet = it.value();

            for (const QSharedPointer<BusInterface>& bus : busSet)
            {
                if (interfaceAbsDefsHash_.contains(bus) &&
                    interfaceAbsDefsHash_.value(bus).contains(absRef->getName()))
                {
                    General::InterfaceMode mode = bus->getInterfaceMode();

                    if (initiatorModes_.contains(mode))
                        hasInitiator = true;

                    if (targetModes_.contains(mode))
                        hasTarget = true;
                }

                if (hasInitiator && hasTarget)
                {
                    validAbsRefs_.insert(absRef);
                    break;
                }
            }

            if (hasInitiator && hasTarget)
                break;
        }
    }
}

QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> InterconnectDataModel::createInstanceBusesLookup() const
{
    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> lookup;

    for (auto it = instanceBusesHash_.constBegin(); it != instanceBusesHash_.constEnd(); ++it)
    {
        QHash<QString, QSharedPointer<BusInterface>> nameToInterfaceMap;
        for (const QSharedPointer<BusInterface>& busInterface : it.value())
        {
            nameToInterfaceMap.insert(busInterface->name(), busInterface);
        }
        lookup.insert(it.key(), nameToInterfaceMap);
    }

    return lookup;
}
