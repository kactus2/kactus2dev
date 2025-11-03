#include "InterconnectDataModel.h"

#include <IPXACTmodels/Component/AbstractionType.h>
#include <IPXACTmodels/common/Document.h>
#include <common/graphicsItems/ComponentItem.h>

using namespace InterconnectGeneration;

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::InterconnectDataModel()
//-----------------------------------------------------------------------------
InterconnectDataModel::InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library, MessageMediator* messager)
    : designWidget_(designWidget),
    library_(library),
    messager_(messager)
{
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::gatherBusAndAbstractionData()
//-----------------------------------------------------------------------------
void InterconnectDataModel::gatherBusAndAbstractionData() {
    getBusesFromInstances();
    getBusesFromTopComponent();
    initConnectionRules();
    initAllowedInterfaceModeRules();
    createInterfaceAdjacencyRules();
    filterValidAbstractionReferences();
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getValidAbstractionRefs()
//-----------------------------------------------------------------------------
QSet<QSharedPointer<ConfigurableVLNVReference>> InterconnectDataModel::getValidAbstractionRefs() const
{
    return validAbsRefs_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getAllAbstractionRefs()
//-----------------------------------------------------------------------------
QSet<QSharedPointer<ConfigurableVLNVReference>> InterconnectDataModel::getAllAbstractionRefs() const
{
    return allAbsRefs_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getInstanceBusMap()
//-----------------------------------------------------------------------------
QHash<QString, QSet<QSharedPointer<BusInterface>>> InterconnectDataModel::getInstanceBusMap() const
{
    return instanceBusesHash_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getInterfaceAbstractionHash()
//-----------------------------------------------------------------------------
QHash<QSharedPointer<BusInterface>, QSet<QString>> InterconnectDataModel::getInterfaceAbstractionHash() const
{
    return interfaceAbsDefsHash_;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getLastInvalidConnectionMessage()
//-----------------------------------------------------------------------------
QString InterconnectDataModel::getLastInvalidConnectionMessage() const 
{ 
    return lastInvalidConnectionMessage_; 
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::interfaceIsTargetAdjacent()
//-----------------------------------------------------------------------------
bool InterconnectDataModel::interfaceIsTargetAdjacent(General::InterfaceMode ifMode, InterconnectType icType, EntityType componentType)
{
    return targetAdjacencyRules_.contains(InterfaceModeAdjacencyKey{ ifMode, icType, componentType });
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::interfaceIsInitiatorAdjacent()
//-----------------------------------------------------------------------------
bool InterconnectDataModel::interfaceIsInitiatorAdjacent(General::InterfaceMode ifMode, InterconnectType icType, EntityType componentType)
{
    return initiatorAdjacencyRules_.contains(InterfaceModeAdjacencyKey{ ifMode, icType, componentType });
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getBusesFromInstances()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getBusesFromTopComponent()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getAbstractionDefinitions()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::filterValidAbstractionReferences()
//-----------------------------------------------------------------------------
void InterconnectDataModel::filterValidAbstractionReferences()
{
    validAbsRefs_.clear();

    auto absToBuses = buildAbstractionToBusMap();
    auto modeEntityToBuses = buildModeEntityToBusMap();

    for (const auto& absRef : allAbsRefs_) {
        if (isAbstractionConnectable(absRef, absToBuses, modeEntityToBuses)) {
            validAbsRefs_.insert(absRef);
        }
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::buildAbstractionToBusMap()
//-----------------------------------------------------------------------------
QHash<QString, QSet<QSharedPointer<BusInterface>>> InterconnectDataModel::buildAbstractionToBusMap() const
{
    QHash<QString, QSet<QSharedPointer<BusInterface>>> map;
    for (auto it = interfaceAbsDefsHash_.cbegin(); it != interfaceAbsDefsHash_.cend(); ++it) {
        const auto& bus = it.key();
        for (const QString& absName : it.value()) {
            map[absName].insert(bus);
        }
    }
    return map;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::buildModeEntityToBusMap()
//-----------------------------------------------------------------------------
QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>>
InterconnectDataModel::buildModeEntityToBusMap() const
{
    QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>> map;

    auto topComponentName = designWidget_->getEditedComponent()->getVlnv().getName();

    for (auto it = instanceBusesHash_.cbegin(); it != instanceBusesHash_.cend(); ++it) {
        EntityType entity = (it.key() == topComponentName)
            ? EntityType::TopComponent : EntityType::Instance;

        for (const auto& bus : it.value()) {
            map.insert({ normalizeTo2022(bus->getInterfaceMode()), entity }, bus);
        }
    }

    return map;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::isAbstractionConnectable()
//-----------------------------------------------------------------------------
bool InterconnectDataModel::isAbstractionConnectable(
    QSharedPointer<ConfigurableVLNVReference> absRef,
    const QHash<QString, QSet<QSharedPointer<BusInterface>>>& absToBuses,
    const QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>>& modeEntityToBuses) const
{
    const QString absName = absRef->getName();
    const auto& sourceBuses = absToBuses.value(absName);

    for (const auto& sourceBus : sourceBuses) {
        General::InterfaceMode srcMode = normalizeTo2022(sourceBus->getInterfaceMode());
        EntityType srcEntity = getEntityTypeForBus(sourceBus);

        auto rules = getValidConnectionTargets(srcEntity, srcMode, InterconnectType::Bridge);
        rules += getValidConnectionTargets(srcEntity, srcMode, InterconnectType::Channel);

        for (const ConnectionRule& rule : rules) {
            auto targets = modeEntityToBuses.values({ rule.targetMode, rule.targetEntity });

            for (const auto& targetBus : targets) {
                if (!absToBuses.value(absName).contains(targetBus)) continue;

                if (normalizeTo2022(targetBus->getInterfaceMode()) == rule.targetMode) {
                    return true;
                }
            }
        }
    }

    return false;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getEntityTypeForBus()
//-----------------------------------------------------------------------------
EntityType InterconnectDataModel::getEntityTypeForBus(const QSharedPointer<BusInterface>& bus) const
{
    for (auto it = instanceBusesHash_.cbegin(); it != instanceBusesHash_.cend(); ++it) {
        if (it.value().contains(bus)) {
            return (it.key() == designWidget_->getEditedComponent()->getVlnv().getName())
                ? EntityType::TopComponent : EntityType::Instance;
        }
    }

    return EntityType::Instance;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::createInterfaceAdjacencyRules()
//-----------------------------------------------------------------------------
void InterconnectDataModel::createInterfaceAdjacencyRules()
{
    
    // The following rules define initiator-adjacent combinations for bridges:
    initiatorAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::INITIATOR, InterconnectType::Bridge, EntityType::Instance });
    initiatorAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::MIRRORED_TARGET, InterconnectType::Bridge, EntityType::Instance });
    initiatorAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::TARGET, InterconnectType::Bridge, EntityType::TopComponent });

    // The following rules define initiator-adjacent combinations for channels:
    initiatorAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::INITIATOR, InterconnectType::Channel, EntityType::Instance });
    initiatorAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::MIRRORED_INITIATOR, InterconnectType::Channel, EntityType::TopComponent });

    // The following rules define target-adjacent combinations for bridges:
    targetAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::TARGET, InterconnectType::Bridge, EntityType::Instance });
    targetAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::MIRRORED_INITIATOR, InterconnectType::Bridge, EntityType::Instance });
    targetAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::INITIATOR, InterconnectType::Bridge, EntityType::TopComponent });

    // The following rules define target-adjacent combinations for channels:
    targetAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::TARGET, InterconnectType::Channel, EntityType::Instance });
    targetAdjacencyRules_.insert(InterfaceModeAdjacencyKey{ General::InterfaceMode::MIRRORED_TARGET, InterconnectType::Channel, EntityType::TopComponent });
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::isModeValidForAllConnections()
//-----------------------------------------------------------------------------
bool InterconnectDataModel::isModeValidForAllConnections(
    const QHash<QString, QList<QSharedPointer<BusInterface>>>& startingPoints,
    const QHash<QString, QList<QSharedPointer<EndpointData>>>& endpoints,
    InterconnectType mode) const
{
    lastInvalidConnectionMessage_.clear(); // Reset previous message

    auto instanceBusesLookup = createInstanceBusesLookup();

    for (auto it = startingPoints.cbegin(); it != startingPoints.cend(); ++it) {
        QString initiatorInstance = it.key();
        EntityType initiatorEntity =
            (initiatorInstance == designWidget_->getEditedComponent()->getVlnv().getName()) ?
            EntityType::TopComponent :
            EntityType::Instance;

        for (const auto& initiatorBus : it.value()) {
            General::InterfaceMode initiatorMode = normalizeTo2022(initiatorBus->getInterfaceMode());
            const QList<ConnectionRule> validRules =
                getValidConnectionTargets(initiatorEntity, initiatorMode, mode);

            for (auto jt = endpoints.cbegin(); jt != endpoints.cend(); ++jt) {
                QString targetInstance = jt.key();
                EntityType targetEntity =
                    (targetInstance == designWidget_->getEditedComponent()->getVlnv().getName()) ?
                    EntityType::TopComponent :
                    EntityType::Instance;

                for (const auto& targetData : jt.value()) {
                    QSharedPointer<BusInterface> targetBus = targetData->endpointBus;
                    General::InterfaceMode targetMode = normalizeTo2022(targetBus->getInterfaceMode());

                    bool valid = std::any_of(validRules.begin(), validRules.end(),
                        [&](const ConnectionRule& rule) {
                            return rule.targetEntity == targetEntity && rule.targetMode == targetMode;
                        });

                    if (!valid) {
                        lastInvalidConnectionMessage_ =
                            QString("Can't connect selected interfaces: %1 [%2] -> %3 [%4]")
                            .arg(initiatorInstance,
                                General::interfaceMode2Str(initiatorMode),
                                targetInstance,
                                General::interfaceMode2Str(targetMode));
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::ConnectionKey()
//-----------------------------------------------------------------------------
bool ConnectionKey::operator==(const ConnectionKey& other) const {
    return sourceEntity == other.sourceEntity && sourceMode == other.sourceMode;
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::ConnectionKey()
//-----------------------------------------------------------------------------
bool ConnectionKey::operator!=(const ConnectionKey& other) const {
    return !(*this == other);
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getValidConnectionTargets()
//-----------------------------------------------------------------------------
QList<ConnectionRule> InterconnectDataModel::getValidConnectionTargets(
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

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::getConnectableInterfaceTypes()
//-----------------------------------------------------------------------------
QSet<General::InterfaceMode> InterconnectDataModel::getConnectableInterfaceTypes(AllowedInterfaceModesKey const& connectionKey) const
{
    return allowedInterfaceModeRules_.value(connectionKey);
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::hasAnyValidConnection()
//-----------------------------------------------------------------------------
bool InterconnectDataModel::hasAnyValidConnection(EntityType sourceEntity, General::InterfaceMode sourceMode) const
{
    ConnectionKey key{ sourceEntity, sourceMode };

    return connectionRules_.contains(key) && !connectionRules_.value(key).isEmpty();
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::normalizeTo2022()
//-----------------------------------------------------------------------------
General::InterfaceMode InterconnectDataModel::normalizeTo2022(General::InterfaceMode mode)
{
    switch (mode) {
    case General::MASTER: return General::INITIATOR;
    case General::SLAVE:  return General::TARGET;
    case General::MIRRORED_MASTER: return General::MIRRORED_INITIATOR;
    case General::MIRRORED_SLAVE:  return General::MIRRORED_TARGET;
    default: return mode;
    }
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::initConnectionRules()
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::initAllowedInterfaceModeRules()
//-----------------------------------------------------------------------------
void InterconnectDataModel::initAllowedInterfaceModeRules()
{
    allowedInterfaceModeRules_.clear();

    // Connection rules for editor where all types of interfaces can be selected.
    // Selectable interfaces depend only on type of interconnect and instance.


    // Valid interface types if bridge interconnect and interface is in instance
    allowedInterfaceModeRules_[{InterconnectType::Bridge, EntityType::Instance}] = {
        General::InterfaceMode::INITIATOR, General::InterfaceMode::TARGET,
        General::InterfaceMode::MIRRORED_INITIATOR, General::InterfaceMode::MIRRORED_TARGET,
        General::InterfaceMode::MASTER, General::InterfaceMode::SLAVE,
        General::InterfaceMode::MIRRORED_MASTER, General::InterfaceMode::MIRRORED_SLAVE
    };

    // Valid interface types if bridge interconnect and interface is in top component
    allowedInterfaceModeRules_[{InterconnectType::Bridge, EntityType::TopComponent}] = {
        General::InterfaceMode::INITIATOR, General::InterfaceMode::TARGET,
        General::InterfaceMode::MASTER, General::InterfaceMode::SLAVE
    };

    // Valid interface types if channel interconnect and interface is in instance
    allowedInterfaceModeRules_[{InterconnectType::Channel, EntityType::Instance}] = {
        General::InterfaceMode::INITIATOR, General::InterfaceMode::TARGET,
        General::InterfaceMode::MASTER, General::InterfaceMode::SLAVE
    };
    
    // Valid interface types if channel interconnect and interface is in top component
    allowedInterfaceModeRules_[{InterconnectType::Channel, EntityType::TopComponent}] = {
        General::InterfaceMode::MIRRORED_INITIATOR, General::InterfaceMode::MIRRORED_TARGET,
        General::InterfaceMode::MIRRORED_MASTER, General::InterfaceMode::MIRRORED_SLAVE
    };
}

//-----------------------------------------------------------------------------
// Function: InterconnectDataModel::createInstanceBusesLookup()
//-----------------------------------------------------------------------------
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
