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
#include <IPXACTmodels/Design/Design.h>

#include <KactusAPI/include/BusInterfaceUtilities.h>

#include <KactusAPI/include/LibraryInterface.h>

//-----------------------------------------------------------------------------
// Function: BusInterfaceTableAutoConnector::BusInterfaceTableAutoConnector()
//-----------------------------------------------------------------------------
BusInterfaceTableAutoConnector::BusInterfaceTableAutoConnector(LibraryInterface* library):
TableAutoConnector(),
library_(library)
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
// Function: BusInterfaceTableAutoConnector::findAlreadyConnectedItems()
//-----------------------------------------------------------------------------
QList<QPair<QString, QString > > BusInterfaceTableAutoConnector::findAlreadyConnectedItems(
    QString const& firstInstanceName, QString const& secondInstanceName,
    QSharedPointer<Design> design) const
{
    QList<QPair<QString, QString > > foundConnections;

    // Find connections between component instance bus interfaces.
    for (auto const& interconnection : *design->getInterconnections())
    {
        auto startInterface = interconnection->getStartInterface();

        auto const& startComponentReference = startInterface->getComponentReference();

        if (startComponentReference == firstInstanceName ||
            startComponentReference == secondInstanceName)
        {
            bool startIsFirst = startComponentReference == firstInstanceName;

            for (auto const& activeInterface : *interconnection->getActiveInterfaces())
            {
                if (startIsFirst && activeInterface->getComponentReference() == secondInstanceName)
                {
                    foundConnections.emplace_back(startInterface->getBusReference(), activeInterface->getBusReference());
                }
                else if (!startIsFirst && activeInterface->getComponentReference() == firstInstanceName)
                {
                    foundConnections.emplace_back(activeInterface->getBusReference(), startInterface->getBusReference());
                }
            }
        }
    }

    return foundConnections;
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
                compatibleInterfaces = getCompatibleInterfaceModes(busMode);
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
// Function: BusInterfaceTableAutoConnector::getCompatibleInterfaceModes()
//-----------------------------------------------------------------------------
QVector<General::InterfaceMode> BusInterfaceTableAutoConnector::getCompatibleInterfaceModes(
    General::InterfaceMode const& busMode) const
{
    return General::getCompatibleInterfaceModesForActiveInterface(busMode);
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
    QIcon busItemIcon(BusInterfaceUtilities::getIconPath(itemBus->getInterfaceMode()));

    newTableItem->setIcon(busItemIcon);
    return newTableItem;
}
