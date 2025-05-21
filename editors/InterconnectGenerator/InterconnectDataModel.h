#ifndef INTERCONNECTDATAMODEL_H
#define INTERCONNECTDATAMODEL_H

#include <QHash>
#include <QSet>
#include <QList>
#include <QSharedPointer>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <KactusAPI/include/LibraryHandler.h>
#include <editors/common/DesignWidget.h>

class InterconnectDataModel
{
public:
    enum class EntityType {
        Instance,
        TopComponent
    };

    enum class InterconnectType {
        Bridge,
        Channel
    };

    struct ConnectionKey {
        EntityType sourceEntity;
        General::InterfaceMode sourceMode;

        bool operator==(const ConnectionKey& other) const;
        bool operator!=(const ConnectionKey& other) const;
    };

    struct ConnectionRule {
        EntityType targetEntity;
        General::InterfaceMode targetMode;
        InterconnectType interconnect;
    };

public:
    InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library, MessageMediator* messager);

    void gatherBusAndAbstractionData();

    QSet<QSharedPointer<ConfigurableVLNVReference>> getValidAbstractionRefs() const;
    QSet<QSharedPointer<ConfigurableVLNVReference>> getAllAbstractionRefs() const;

    QHash<QString, QSet<QSharedPointer<BusInterface>>> getInstanceBusMap() const;
    QHash<QSharedPointer<BusInterface>, QSet<QString>> getInterfaceAbstractionHash() const;
    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> createInstanceBusesLookup() const;

    QList<ConnectionRule> getValidConnectionTargets(
        EntityType sourceEntity,
        General::InterfaceMode sourceMode,
        InterconnectType currentInterconnect) const;

private:
    void initConnectionRules();
    void getBusesFromInstances();
    void getBusesFromTopComponent();
    void getAbstractionDefinitions(QSharedPointer<BusInterface> bus);
    void filterValidAbstractionReferences();
    General::InterfaceMode normalizeTo2014(General::InterfaceMode mode);
    QString toString(General::InterfaceMode mode) const;

    DesignWidget* designWidget_;
    LibraryHandler* library_;
    MessageMediator* messager_;

    QSharedPointer<Component> currentComponent_;

    QSet<QSharedPointer<ConfigurableVLNVReference>> allAbsRefs_;
    QSet<QSharedPointer<ConfigurableVLNVReference>> validAbsRefs_;

    QHash<QString, QSet<QSharedPointer<BusInterface>>> instanceBusesHash_;
    QHash<QSharedPointer<BusInterface>, QSet<QString>> interfaceAbsDefsHash_;

    QHash<QString, QSet<QSharedPointer<BusInterface>>> absToBuses;
    QMultiHash<QPair<General::InterfaceMode, EntityType>, QSharedPointer<BusInterface>> modeEntityToBuses;

    QHash<ConnectionKey, QList<ConnectionRule>> connectionRules_;
};

uint qHash(const InterconnectDataModel::ConnectionKey& key, uint seed = 0);

#endif // INTERCONNECTDATAMODEL_H
