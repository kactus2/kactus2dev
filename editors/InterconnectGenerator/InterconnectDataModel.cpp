#include "InterconnectDataModel.h"

#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/common/Document.h>
#include <common/graphicsItems/ComponentItem.h>

InterconnectDataModel::InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library, MessageMediator* messager)
    : designWidget_(designWidget),
    library_(library),
    messager_(messager)
{
}

void InterconnectDataModel::gatherBusAndAbstractionData() {
    getBusesFromInstances();
    getBusesFromTopComponent();
    initConnectionRules();
    filterValidAbstractionReferences();
}

QSet<QSharedPointer<ConfigurableVLNVReference>> InterconnectDataModel::getValidAbstractionRefs() const
{
    return validAbsRefs_;
}

QSet<QSharedPointer<ConfigurableVLNVReference>> InterconnectDataModel::getAllAbstractionRefs() const
{
    return allAbsRefs_;
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
        getAbstractionDefinitions(bus);
    }
}

void InterconnectDataModel::getAbstractionDefinitions(QSharedPointer<BusInterface> bus)
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

    // Build a quick lookup of all buses grouped by abstraction name
    QHash<QString, QSet<QSharedPointer<BusInterface>>> absToBuses;
    for (auto it = interfaceAbsDefsHash_.cbegin(); it != interfaceAbsDefsHash_.cend(); ++it)
    {
        const QSharedPointer<BusInterface>& bus = it.key();
        const QSet<QString>& absNames = it.value();

        for (const QString& abs : absNames)
        {
            absToBuses[abs].insert(bus);
        }
    }

    // Build mode/entity hash for each bus once
    QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>> modeEntityToBuses;
    for (auto it = instanceBusesHash_.cbegin(); it != instanceBusesHash_.cend(); ++it)
    {
        const QString& instanceName = it.key();
        EntityType entityType = (instanceName == designWidget_->getEditedComponent()->getVlnv().getName())
            ? EntityType::TopComponent : EntityType::Instance;

        for (const QSharedPointer<BusInterface>& bus : it.value())
        {
            General::InterfaceMode mode = normalizeTo2014(bus->getInterfaceMode());
            modeEntityToBuses.insert({ mode, entityType }, bus);
        }
    }

    // Main abstraction filtering
    for (const QSharedPointer<ConfigurableVLNVReference>& absRef : allAbsRefs_)
    {
        const QString abs = absRef->getName();
        const auto& busesWithAbs = absToBuses.value(abs);

        bool abstractionIsConnectable = false;

        for (const QSharedPointer<BusInterface>& sourceBus : busesWithAbs)
        {
            General::InterfaceMode sourceMode = normalizeTo2014(sourceBus->getInterfaceMode());

            QString sourceInstance;
            EntityType sourceEntity = EntityType::Instance;

            // Find instance name
            for (auto it = instanceBusesHash_.cbegin(); it != instanceBusesHash_.cend(); ++it) {
                if (it.value().contains(sourceBus)) {
                    sourceInstance = it.key();
                    break;
                }
            }

            sourceEntity = (sourceInstance == designWidget_->getEditedComponent()->getVlnv().getName())
                ? EntityType::TopComponent : EntityType::Instance;

            QList<ConnectionRule> rules;
            rules += getValidConnectionTargets(sourceEntity, sourceMode, InterconnectType::Bridge);
            rules += getValidConnectionTargets(sourceEntity, sourceMode, InterconnectType::Channel);

            for (const ConnectionRule& rule : rules)
            {
                QPair<General::InterfaceMode, EntityType> targetKey = { rule.targetMode, rule.targetEntity };
                const auto matchingTargets = modeEntityToBuses.values(targetKey);

                for (const QSharedPointer<BusInterface>& targetBus : matchingTargets)
                {
                    if (!absToBuses.value(abs).contains(targetBus))
                        continue;

                    General::InterfaceMode actualTargetMode = normalizeTo2014(targetBus->getInterfaceMode());

                    if (actualTargetMode == rule.targetMode)
                    {
                        validAbsRefs_.insert(absRef);
                        abstractionIsConnectable = true;
                        break;
                    }
                }

                if (abstractionIsConnectable) break;
            }

            if (abstractionIsConnectable) break;
        }
    }
}

bool InterconnectDataModel::isModeValidForAllConnections(
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
    const QHash<QString, QList<QSharedPointer<TargetData>>>& endpoints,
    InterconnectType mode) const
{
    auto instanceBusesLookup = createInstanceBusesLookup();

    for (auto it = startingPoints.cbegin(); it != startingPoints.cend(); ++it) {
        QString initiatorInstance = it.key();
        EntityType initiatorEntity =
            (initiatorInstance == designWidget_->getEditedComponent()->getVlnv().getName()) ?
            EntityType::TopComponent :
            EntityType::Instance;

        for (const auto& initiatorBus : it.value()) {
            General::InterfaceMode initiatorMode = normalizeTo2014(initiatorBus->getInterfaceMode());

            const QList<ConnectionRule> validRules =
                getValidConnectionTargets(initiatorEntity, initiatorMode, mode);

            for (auto jt = endpoints.cbegin(); jt != endpoints.cend(); ++jt) {
                QString targetInstance = jt.key();
                EntityType targetEntity =
                    (targetInstance == designWidget_->getEditedComponent()->getVlnv().getName()) ?
                    EntityType::TopComponent :
                    EntityType::Instance;

                for (const auto& targetData : jt.value()) {
                    QSharedPointer<BusInterface> targetBus = targetData->targetBus;
                    General::InterfaceMode targetMode = normalizeTo2014(targetBus->getInterfaceMode());

                    bool valid = false;
                    for (const ConnectionRule& rule : validRules) {
                        if (rule.targetEntity == targetEntity && rule.targetMode == targetMode) {
                            valid = true;
                            break;
                        }
                    }

                    if (!valid) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

bool InterconnectDataModel::ConnectionKey::operator==(const ConnectionKey& other) const {
    return sourceEntity == other.sourceEntity && sourceMode == other.sourceMode;
}

bool InterconnectDataModel::ConnectionKey::operator!=(const ConnectionKey& other) const {
    return !(*this == other);
}

QList<InterconnectDataModel::ConnectionRule> InterconnectDataModel::getValidConnectionTargets(
    EntityType sourceEntity,
    General::InterfaceMode sourceMode,
    InterconnectType currentInterconnect) const
{   

    ConnectionKey key{ sourceEntity, sourceMode };
    QList<ConnectionRule> result;

    if (!connectionRules_.contains(key)) {
        return result;
    }

    for (const ConnectionRule& rule : connectionRules_.value(key)) {
        if (rule.interconnect == currentInterconnect) {
            result.append(rule);
        }
    }
    return result;
}

General::InterfaceMode InterconnectDataModel::normalizeTo2014(General::InterfaceMode mode)
{
    switch (mode) {
    case General::MASTER: return General::INITIATOR;
    case General::SLAVE:  return General::TARGET;
    case General::MIRRORED_MASTER: return General::MIRRORED_INITIATOR;
    case General::MIRRORED_SLAVE:  return General::MIRRORED_TARGET;
    default: return mode;
    }
}

void InterconnectDataModel::initConnectionRules()
{
    using ET = EntityType;
    using IM = General::InterfaceMode;
    using IC = InterconnectType;

    connectionRules_.clear();

    // === Instance ===
    connectionRules_[{ET::Instance, IM::INITIATOR}] = {
        {ET::Instance, IM::TARGET, IC::Bridge},
        {ET::Instance, IM::TARGET, IC::Channel},
        {ET::Instance, IM::MIRRORED_INITIATOR, IC::Bridge},
        {ET::TopComponent, IM::INITIATOR, IC::Bridge},
        {ET::TopComponent, IM::MIRRORED_TARGET, IC::Channel}
    };

    connectionRules_[{ET::Instance, IM::TARGET}] = {
        {ET::Instance, IM::INITIATOR, IC::Bridge},
        {ET::Instance, IM::INITIATOR, IC::Channel},
        {ET::Instance, IM::MIRRORED_TARGET, IC::Bridge},
        {ET::TopComponent, IM::TARGET, IC::Bridge},
        {ET::TopComponent, IM::MIRRORED_INITIATOR, IC::Channel}
    };

    connectionRules_[{ET::Instance, IM::MIRRORED_INITIATOR}] = {
        {ET::Instance, IM::INITIATOR, IC::Bridge},
        {ET::Instance, IM::MIRRORED_TARGET, IC::Bridge},
        {ET::TopComponent, IM::TARGET, IC::Bridge}
    };

    connectionRules_[{ET::Instance, IM::MIRRORED_TARGET}] = {
        {ET::Instance, IM::TARGET, IC::Bridge},
        {ET::Instance, IM::MIRRORED_INITIATOR, IC::Bridge},
        {ET::TopComponent, IM::TARGET, IC::Bridge}
    };

    // === TopComponent ===
    connectionRules_[{ET::TopComponent, IM::INITIATOR}] = {
        {ET::Instance, IM::INITIATOR, IC::Bridge},
        {ET::Instance, IM::MIRRORED_TARGET, IC::Bridge},
        {ET::TopComponent, IM::TARGET, IC::Bridge}
    };

    connectionRules_[{ET::TopComponent, IM::TARGET}] = {
        {ET::Instance, IM::TARGET, IC::Bridge},
        {ET::Instance, IM::MIRRORED_INITIATOR, IC::Bridge},
        {ET::TopComponent, IM::INITIATOR, IC::Bridge}
    };

    connectionRules_[{ET::TopComponent, IM::MIRRORED_INITIATOR}] = {
        {ET::Instance, IM::TARGET, IC::Channel},
        {ET::TopComponent, IM::MIRRORED_TARGET, IC::Channel}
    };

    connectionRules_[{ET::TopComponent, IM::MIRRORED_TARGET}] = {
        {ET::Instance, IM::INITIATOR, IC::Channel},
        {ET::TopComponent, IM::MIRRORED_INITIATOR, IC::Channel}
    };
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

QString InterconnectDataModel::toString(General::InterfaceMode mode) const
{
    switch (mode)
    {
    case General::MASTER: return "MASTER";
    case General::SLAVE: return "SLAVE";
    case General::INITIATOR: return "INITIATOR";
    case General::TARGET: return "TARGET";
    case General::SYSTEM: return "SYSTEM";
    case General::MIRRORED_MASTER: return "MIRRORED_MASTER";
    case General::MIRRORED_SLAVE: return "MIRRORED_SLAVE";
    case General::MIRRORED_INITIATOR: return "MIRRORED_INITIATOR";
    case General::MIRRORED_TARGET: return "MIRRORED_TARGET";
    case General::MIRRORED_SYSTEM: return "MIRRORED_SYSTEM";
    case General::MONITOR: return "MONITOR";
    default: return "UNKNOWN_MODE";
    }
}

uint qHash(const InterconnectDataModel::ConnectionKey& key, uint seed)
{
    return qHash(static_cast<int>(key.sourceEntity), seed) ^
        qHash(static_cast<int>(key.sourceMode), seed << 1);
}
