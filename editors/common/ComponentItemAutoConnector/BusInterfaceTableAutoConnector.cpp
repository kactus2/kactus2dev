//-----------------------------------------------------------------------------
// File: BusInterfaceTableAutoConnector.cpp
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Mikko Teuho
// Date: 13.02.2019
//
// Description:
// Automatically connects bus interfaces of two components.
//-----------------------------------------------------------------------------

#include "BusInterfaceTableAutoConnector.h"

#include <IPXACTmodels/BusDefinition/BusDefinition.h>

#include <IPXACTmodels/Component/Component.h>
#include <IPXACTmodels/Component/BusInterface.h>

#include <editors/common/BusInterfaceUtilities.h>

#include <library/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::BusInterfaceTableAutoConnector()
//-----------------------------------------------------------------------------
BusInterfaceTableAutoConnector::BusInterfaceTableAutoConnector(LibraryInterface* library):
TableAutoConnector(),
library_(library)
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::~BusInterfaceTableAutoConnector()
//-----------------------------------------------------------------------------
BusInterfaceTableAutoConnector::~BusInterfaceTableAutoConnector()
{

}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::findPossibleCombinations()
//-----------------------------------------------------------------------------
QVector<QPair<QString, QVector<QString> > > BusInterfaceTableAutoConnector::findPossibleCombinations(
    QSharedPointer<Component> firstComponent, QSharedPointer<Component> secondComponent) const
{
    QVector<QPair<QString, QVector<QString> > > possibleCombinations;

    QSharedPointer<QList<QSharedPointer<BusInterface> > > firstInterfaces = firstComponent->getBusInterfaces();
    QSharedPointer<QList<QSharedPointer<BusInterface> > > secondInterface = secondComponent->getBusInterfaces();

    for (auto const& currentBus : *firstInterfaces)
    {
        if (currentBus->getInterfaceMode() != General::MONITOR)
        {
            QVector<QString> connectibleBuses = getConnectableBusInterfaceNames(currentBus, secondInterface);

            if (!connectibleBuses.isEmpty())
            {
                QPair<QString, QVector<QString> > newCombinationPair;
                newCombinationPair.first = currentBus->name();
                newCombinationPair.second = connectibleBuses;

                possibleCombinations.append(newCombinationPair);
            }
        }
    }

    return possibleCombinations;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::getConnectableBusInterfaceNames()
//-----------------------------------------------------------------------------
QVector<QString> BusInterfaceTableAutoConnector::getConnectableBusInterfaceNames(
    QSharedPointer<BusInterface> currentBus,
    QSharedPointer<QList<QSharedPointer<BusInterface>>> secondItemBusInterfaces) const
{
    QVector<QString> possibleInterfaces;

    ConfigurableVLNVReference busReference = currentBus->getBusType();
    if (busReference.isValid())
    {
        QSharedPointer<const BusDefinition> busType =
            library_->getModelReadOnly(busReference).dynamicCast<const BusDefinition>();
        if (busType)
        {
            General::InterfaceMode busMode = currentBus->getInterfaceMode();

            QVector<General::InterfaceMode> compatibleInterfaces;
            if (busMode == General::MONITOR)
            {
                compatibleInterfaces.append(currentBus->getMonitor()->interfaceMode_);
            }
            else
            {
                compatibleInterfaces = General::getCompatibleInterfaceModesForActiveInterface(busMode);
            }

            for (auto comparisonBus : *secondItemBusInterfaces)
            {
                if (interfacesAreCompatible(comparisonBus, compatibleInterfaces) &&
                    BusInterfaceUtilities::hasMatchingBusDefinitions(
                        busType, comparisonBus->getBusType(), library_))
                {
                    possibleInterfaces.append(comparisonBus->name());
                }
            }
        }
    }


    return possibleInterfaces;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::interfacesAreCompatible()
//-----------------------------------------------------------------------------
bool BusInterfaceTableAutoConnector::interfacesAreCompatible(QSharedPointer<BusInterface> comparisonBus,
    QVector<General::InterfaceMode> compatibleModes) const
{
    General::InterfaceMode comparisonMode = comparisonBus->getInterfaceMode();
    if (comparisonMode == General::MONITOR)
    {
        return false;
    }

    return compatibleModes.contains(comparisonMode);
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::createTableWidgetItem()
//-----------------------------------------------------------------------------
QTableWidgetItem* BusInterfaceTableAutoConnector::createTableWidgetItem(QString const& itemName,
    QSharedPointer<Component> containingComponent) const
{
    QTableWidgetItem* newTableItem = TableAutoConnector::createTableWidgetItem(itemName, containingComponent);

    QSharedPointer<BusInterface> itemBus = containingComponent->getBusInterface(itemName);
    QIcon busItemIcon(getIconPath(itemBus->getInterfaceMode()));

    newTableItem->setIcon(busItemIcon);
    return newTableItem;
}

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::getIconPath()
//-----------------------------------------------------------------------------
QString BusInterfaceTableAutoConnector::getIconPath(General::InterfaceMode busMode) const
{
    QString iconPath("");
    if (busMode == General::MASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMaster.png";
    }
    else if (busMode == General::MIRROREDMASTER)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredMaster.png";
    }
    else if (busMode == General::SLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceSlave.png";
    }
    else if (busMode == General::MIRROREDSLAVE)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSlave.png";
    }
    else if (busMode == General::SYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceSystem.png";
    }
    else if (busMode == General::MIRROREDSYSTEM)
    {
        iconPath = ":icons/common/graphics/busInterfaceMirroredSystem.png";
    }
    else if (busMode == General::MONITOR)
    {
        iconPath = ":icons/common/graphics/busInterfaceMonitor.png";
    }

    return iconPath;
}
