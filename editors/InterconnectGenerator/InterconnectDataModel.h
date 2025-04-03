#ifndef INTERCONNECTDATAMODEL_H
#define INTERCONNECTDATAMODEL_H

#include <QHash>
#include <QSet>
#include <QSharedPointer>

#include <IPXACTmodels/Component/BusInterface.h>
#include <IPXACTmodels/Component/MemoryMap.h>
#include <IPXACTmodels/common/ConfigurableVLNVReference.h>

#include <KactusAPI/include/LibraryHandler.h>

#include <editors/common/DesignWidget.h>


class InterconnectDataModel
{
public:
    InterconnectDataModel(DesignWidget* designWidget, LibraryHandler* library);

    void gatherBusAndAbstractionData(const QSet<General::InterfaceMode>& initiatorModes,
        const QSet<General::InterfaceMode>& targetModes);

    QSet<QSharedPointer<ConfigurableVLNVReference>> getValidAbstractionRefs() const;

    QHash<QString, QSet<QSharedPointer<BusInterface>>> getInstanceBusMap() const;

    QHash<QSharedPointer<BusInterface>, QSet<QString>> getInterfaceAbstractionHash() const;

    QHash<QString, QHash<QString, QSharedPointer<BusInterface>>> createInstanceBusesLookup() const;

private:
    void getBusesFromInstances();
    void getBusesFromTopComponent();
    void getAbstractionDefinitions(QSharedPointer<BusInterface> bus, bool fromInstance = true);
    void filterValidAbstractionReferences();

    DesignWidget* designWidget_;
    LibraryHandler* library_;

    QSharedPointer<Component> currentComponent_;

    QSet<QSharedPointer<ConfigurableVLNVReference>> allAbsRefs_;
    QSet<QSharedPointer<ConfigurableVLNVReference>> validAbsRefs_;

    QHash<QString, QSet<QSharedPointer<BusInterface>>> instanceBusesHash_;
    QHash<QSharedPointer<BusInterface>, QSet<QString>> interfaceAbsDefsHash_;

    QSet<General::InterfaceMode> initiatorModes_;
    QSet<General::InterfaceMode> targetModes_;
};

#endif // INTERCONNECTDATAMODEL_H
